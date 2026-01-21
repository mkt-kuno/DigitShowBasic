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

/**
 * @file step_completion.hpp
 * @brief Small predicates to determine control step completion
 */

#pragma once

#include "../math_constexpr.hpp"
#include "control_output.hpp"
#include "iis_motor.hpp"
#include "measurement.hpp"
#include "params.hpp"
#include "patterns.hpp"
#include <chrono>

namespace control
{

inline constexpr bool is_out_of_stroke(const PhysicalInput &input) noexcept
{
    const double current_es = input.shear_displacement_mm;
    return current_es <= MinStroke || MaxStroke <= current_es;
}

inline constexpr bool is_target_stress_reached(const double motor_rpm, const double target_tau_kpa,
                                               const PhysicalInput &input) noexcept
{
    const double tau = input.shear_stress_kpa();
    return (motor_rpm >= 0.0 && tau > target_tau_kpa) || (motor_rpm < 0.0 && tau < target_tau_kpa);
}

inline constexpr bool is_target_displacement_reached(const double motor_rpm, const double target_es_mm,
                                                     const PhysicalInput &input) noexcept
{
    const double current_es = input.shear_displacement_mm;
    const auto complete =
        (motor_rpm >= 0.0 && current_es > target_es_mm) || (motor_rpm < 0.0 && current_es < target_es_mm);
    if !consteval
    {
        if (complete)
        {
            spdlog::info("Target displacement reached: motor_rpm={:.6} RPM, target_es={:.6} mm, current_es={:.6} mm",
                         motor_rpm, target_es_mm, current_es);
        }
    }
    return complete;
}

inline constexpr bool is_cycles_completed(const size_t num_cyclic, const size_t target_cycles) noexcept
{
    return num_cyclic > target_cycles;
}

inline constexpr bool is_timer_expired(std::chrono::steady_clock::duration elapsed,
                                       std::chrono::steady_clock::duration target) noexcept
{
    return elapsed >= target;
}

/**
 * @brief Evaluate whether a control step should complete
 *
 * Centralized completion logic for all control patterns. Checks pattern-specific
 * termination conditions including stroke limits, target stress/displacement,
 * cycle counts, and elapsed time.
 *
 * @param params Control step parameters including pattern type and targets
 * @param input Current physical measurements
 * @param state Current control output state (for cycle counter)
 * @param elapsed Time elapsed since step started
 * @return true if step should complete, false otherwise
 */
inline constexpr bool evaluate_step_completion(const ControlParams &params, const PhysicalInput &input,
                                               const ControlOutput &state,
                                               std::chrono::steady_clock::duration elapsed) noexcept
{
    // Stroke limit check applies to all motor-based patterns
    bool completed = is_out_of_stroke(input);

    switch (params.pattern)
    {
    case ControlPattern::MonotonicLoadingConstantPressure:
    case ControlPattern::MonotonicLoadingConstantVolume:
        completed = completed || is_target_stress_reached(params.monotonic_loading.motor_rpm,
                                                          params.monotonic_loading.target_tau_kpa, input);
        break;
    case ControlPattern::MonotonicLoadingDisplacementConstantPressure:
    case ControlPattern::MonotonicLoadingDisplacementConstantVolume:
        completed = completed || is_target_displacement_reached(params.monotonic_loading.motor_rpm,
                                                                params.monotonic_loading.target_displacement_mm, input);
        break;
    case ControlPattern::CyclicLoadingConstantPressure:
    case ControlPattern::CyclicLoadingConstantVolume:
    case ControlPattern::CyclicLoadingDisplacementConstantPressure:
    case ControlPattern::CyclicLoadingDisplacementConstantVolume:
        completed = completed || is_cycles_completed(state.num_cyclic, params.cyclic_loading.cycles);
        break;
    case ControlPattern::CreepConstantPressure:
    case ControlPattern::CreepConstantVolume:
    case ControlPattern::CreepConstantPressureFast:
    case ControlPattern::CreepConstantPressureFastRef:
    case ControlPattern::RelaxationConstantPressure:
    case ControlPattern::RelaxationConstantVolume: {
        const auto target_duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(params.duration);
        completed = completed || is_timer_expired(elapsed, target_duration);
        break;
    }
    case ControlPattern::ConstantTauConsolidation: {
        completed =
            completed || math_constexpr::abs(input.vertical_stress_kpa() - params.vertical_stress_kpa.setpoint) <
                             params.vertical_stress_kpa.error;
        break;
    }
    case ControlPattern::NoControl:
    case ControlPattern::BeforeConsolidation:
    case ControlPattern::AfterConsolidation:
        completed = true;
        break;
    default:
        break;
    }
    return completed;
}

} // namespace control
