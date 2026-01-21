/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Takuto ISHII
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../chrono_alias.hpp"
#include "params.hpp"
#include "step_completion.hpp"
#include "strategy_factory.hpp"
#include "strategy_types.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
#include <vector>

namespace control
{

/**
 * @brief Struct representing a single control step
 *
 * Each step contains a ControlParams instance which includes both the control pattern
 * and all associated parameters.
 */
struct ControlStep
{
    ControlParams parameters; // Control parameters (includes pattern field)
};

inline std::chrono::seconds SequentTime1{0};

// Time elapsed in current control tick (steady_clock duration)
inline std::chrono::steady_clock::duration CtrlStepTime{0};

inline size_t current_step_index = 0;
inline std::vector<ControlStep> control_steps; // Flexible-size array of control steps (128-step limit removed)
inline size_t num_cyclic = 0;

// Total elapsed time in current control step (steady_clock duration)
inline std::chrono::steady_clock::duration step_elapsed{0};

// Control execution state: true when control is running, false when stopped
inline bool is_control_running = false;

/**
 * @brief Safely set the current step index with bounds checking
 * @param new_index The desired step index
 * @return true if the index was set successfully, false if out of bounds or control_steps is empty
 *
 * This function ensures that current_step_index never exceeds the valid range
 * of control_steps, preventing out-of-bounds access and program crashes.
 */
inline bool set_current_step_index(const size_t new_index) noexcept
{
    if (control_steps.empty() || new_index >= control_steps.size())
    {
        return false;
    }
    current_step_index = new_index;
    return true;
}

/**
 * @brief Safely increment the current step index
 * @return true if incremented successfully, false if already at the last step or control_steps is empty
 */
inline bool increment_current_step_index() noexcept
{
    // Check: empty vector, or at/beyond last valid index (size-1)
    // Compare current_step_index >= size - 1 to detect if at last step
    // Empty check first prevents underflow when size is 0
    const size_t size = control_steps.size();
    if (size == 0 || current_step_index >= size - 1)
    {
        return false;
    }
    current_step_index++;
    return true;
}

/**
 * @brief Safely decrement the current step index
 * @return true if decremented successfully, false if already at the first step or control_steps is empty
 */
inline bool decrement_current_step_index() noexcept
{
    // Check empty and common failure case (index == 0) for early exit
    if (control_steps.empty() || current_step_index == 0)
    {
        return false;
    }
    current_step_index--;
    return true;
}

/**
 * @brief Reset the current step index to 0
 *
 * This unconditionally sets the index to 0. Callers should ensure control_steps
 * will be populated before accessing elements, or check control_steps.empty() separately.
 */
inline void reset_current_step_index() noexcept
{
    current_step_index = 0;
}

/// @brief Execute a single closed-loop control step.
inline void execute() noexcept
{
    if (control::current_step_index >= control::control_steps.size())
    {
        spdlog::error("execute: current_step_index={} out of bounds (size={}), stopping control",
                      control::current_step_index, control::control_steps.size());
        return;
    }

    const auto &current_step = control::control_steps[control::current_step_index];

    // Consolidated state management: single static control_output instead of separate static variables
    static ControlOutput control_output;
    // Strategy cache: create strategy once per step to maintain state across ticks
    static ControlStrategy current_strategy;
    static size_t strategy_step_index = static_cast<size_t>(-1);

    using namespace std::chrono;
    control::step_elapsed += control::CtrlStepTime;

    spdlog::debug("execute tick: CtrlStepTime={}, step_elapsed={}, current_step_index={}, pattern={}",
                  duration_cast<milliseconds>(control::CtrlStepTime), seconds_d{control::step_elapsed},
                  control::current_step_index, get_use_label(current_step.parameters.pattern));

    using variables::physical::latest_physical_input;
    using variables::physical::latest_physical_output;

    // Initialize control output from current D/A state
    const auto &output = latest_physical_output.load();
    control_output.front_ep_kpa = output.front_ep_kpa;
    control_output.rear_ep_kpa = output.rear_ep_kpa;
    control_output.motor_rpm = output.motor_rpm;

    // Build context for strategy execution
    const control::ControlContext ctx{.physical_input = latest_physical_input,
                                      .physical_output = latest_physical_output,
                                      .current_step = current_step,
                                      .control_output = control_output,
                                      .elapsed_time = control::step_elapsed,
                                      .step_interval = control::CtrlStepTime};

    // Create or reuse strategy for this step
    if (strategy_step_index != control::current_step_index)
    {
        spdlog::debug("Creating strategy for step {} pattern={}", control::current_step_index,
                      get_use_label(current_step.parameters.pattern));
        current_strategy = control::create_strategy(current_step.parameters.pattern, ctx);
        strategy_step_index = control::current_step_index;
    }

    const auto measured_input = latest_physical_input.load();

    const auto &params = current_step.parameters;

    const bool step_completed =
        control::evaluate_step_completion(params, measured_input, control_output, control::step_elapsed);
    const bool needs_side_effects = params.pattern == ControlPattern::NoControl ||
                                    params.pattern == ControlPattern::BeforeConsolidation ||
                                    params.pattern == ControlPattern::AfterConsolidation;

    // Update control output only if step not completed or side effects are needed
    if (!step_completed || needs_side_effects)
    {
        // Execute the strategy - it returns updated ControlOutput
        control_output = current_strategy(ctx);

        control::num_cyclic = control_output.num_cyclic;
        if (const auto result = variables::physical::set_output(
                PhysicalOutput<>{control_output.front_ep_kpa, control_output.rear_ep_kpa, control_output.motor_rpm});
            !result)
        {
            spdlog::error("Failed to apply control output: {}", result.error());
        }
    }
    if (step_completed)
    {
        spdlog::info("Control step {} ({}) completed, advancing to next step", control::current_step_index,
                     get_use_label(current_step.parameters.pattern));
        control::step_elapsed = decltype(control::step_elapsed){};
        if (!control::increment_current_step_index())
        {
            spdlog::info("Control completed: reached end of control steps");
            // When at the last step, don't increment - control will stop naturally
        }
        // Note: control_output is preserved to maintain hardware state continuity.
        // The next step will read current DAVout values (line ~181) to initialize itself,
        // ensuring smooth transition without abrupt voltage changes that could damage equipment.
        // Motor is already stopped (motor_rpm = 0.0 at line 259), and EP pressure is maintained.
    }
}

} // namespace control
