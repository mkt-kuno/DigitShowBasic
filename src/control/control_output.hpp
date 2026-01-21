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
 * @file control_state.hpp
 * @brief Output state structure for control patterns
 *
 * This file defines the ControlOutput structure that all control patterns
 * return, containing motor control voltages, EP cell voltages, and state flags.
 */

#pragma once
#include "measurement.hpp"

/**
 * @brief Output state from control pattern execution
 *
 * This structure encapsulates all outputs and state changes from a control
 * pattern function, making the functions pure and testable.
 */
struct ControlOutput
{
    double front_ep_kpa = 0.0;
    double rear_ep_kpa = 0.0;
    double motor_rpm = 0.0;

    // Cyclic control state (only used by cyclic patterns)
    size_t num_cyclic = 0;    // Cycle counter
    bool flag_cyclic = false; // Cyclic state flag (loading/unloading phase)
};
