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
 * @file strategy_types.hpp
 * @brief Type definitions for control strategy implementation
 *
 * This file defines the core types used in the control strategy pattern:
 * - ControlContext: Encapsulates all state needed for control execution
 * - ControlStrategy: Function type for control strategies
 */

#pragma once

#include "../Variables.hpp"
#include "control.hpp"
#include "control_output.hpp"
#include "measurement.hpp"
#include "params.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <functional>

namespace control
{
struct ControlStep;

/**
 * @brief Context passed to control strategies
 *
 * Contains all the read-only state needed to execute a control strategy.
 * Strategies should not mutate the context.
 */
struct ControlContext
{
    // Input: Current physical state
    std::atomic<control::PhysicalInput> &physical_input;
    const std::atomic<control::PhysicalOutput<>> &physical_output;

    // Input: Control step parameters (read-only)
    const ControlStep &current_step;

    // Input: Current control output state (read-only)
    const ControlOutput &control_output;

    // Input: Elapsed time in current step
    const std::chrono::steady_clock::duration &elapsed_time;

    // Input: Control step interval (for rate calculations)
    const std::chrono::steady_clock::duration &step_interval;
};

/**
 * @brief Strategy function type
 *
 * A control strategy is a callable that takes a context and returns updated ControlOutput.
 * Strategies can be implemented as:
 * - Free functions (for stateless strategies)
 * - Function objects (for stateful strategies with member variables)
 */
using ControlStrategy = std::function<ControlOutput(const ControlContext &)>;

} // namespace control
