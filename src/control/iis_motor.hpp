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
#include "../math_constexpr.hpp"
#include "control_output.hpp"
#include "measurement.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

// Constants used across multiple patterns
static constexpr double MinStroke = -15.0; // Minimum stroke limit (mm)
static constexpr double MaxStroke = 15.0;  // Maximum stroke limit (mm)

/**
 * @brief Apply motor control with tau as setpoint (P-control on deviator stress)
 *
 * Controls motor clutch based on tau (deviator stress):
 * - If tau > target + ERR: Unload (clutch = 5.0f)
 * - If tau < target - ERR: Load (clutch = 0.0f)
 * - Otherwise: Stop motor (rpm = 0)
 *
 * @param params Control parameters
 */
inline constexpr ControlOutput apply_motor_tau_control(const control::ControlParams::ShearStress &params,
                                                       const control::PhysicalInput &input,
                                                       const ControlOutput &output) noexcept
{
    // 位置型P制御で動かす
    const auto tau_error = apply_tolerance(params.setpoint - input.shear_stress_kpa(), params.error);
    const auto max_rpm = math_constexpr::abs(params.cv_limit_rpm);

    // 制御偏差がerr_stress * 2以上で最大回転数、err_stress * 0.9で0となるようにKpを設定する
    // 軸対象にするため、rpmは絶対値で計算し、tau_errorの符号で方向を決定する
    const auto kp = max_rpm / (params.error * 2.0 - params.error * 0.9);
    auto new_output = output;
    new_output.motor_rpm =
        tau_error != 0.0
            ? math_constexpr::copysign(
                  std::clamp(kp * (math_constexpr::abs(tau_error) - params.error * 0.9), -max_rpm, max_rpm), tau_error)
            : 0.0;

    if !consteval
    {
        spdlog::debug("Motor Tau Control: target_tau={:.6} kPa, shear_stress={:.6} kPa, "
                      "tau_error={:.6} kPa, kp={:.6} RPM/kPa, next_rpm={:.6} RPM",
                      params.setpoint, input.shear_stress_kpa(), tau_error, kp, new_output.motor_rpm);
    }
    return new_output;
}

/**
 * @brief Stop motor (turn off motor and set speed to 0)
 *
 * @param davout_motor Motor on/off voltage
 * @param davout_motor_speed Motor speed voltage
 */
inline constexpr void stop_motor(float &davout_motor, float &davout_motor_speed) noexcept
{
    davout_motor = 0.0f;
    davout_motor_speed = 0.0f;
}

/**
 * @brief Apply motor acceleration control
 *
 * Ramps motor speed from start_rpm to target_rpm at specified acceleration rate.
 * Returns true when target is reached (indicating step should advance).
 *
 * @param params Control parameters
 * @param davout_motor_speed Motor speed voltage (input/output)
 * @return true if target RPM reached, false otherwise
 */
inline constexpr ControlOutput apply_motor_acceleration(const control::ControlParams::MotorAccelerationParams &params,
                                                        const control::PhysicalInput &,
                                                        const ControlOutput &prev_state) noexcept
{
    const auto rpm_diff = apply_tolerance(params.target_rpm - prev_state.motor_rpm, params.err_rpm);

    auto output = prev_state;
    if (
        // Check if accelerating upward
        (params.start_rpm < params.target_rpm && params.acceleration > 0.0 && rpm_diff >= 0.0) ||
        // Check if decelerating downward
        (params.start_rpm > params.target_rpm && params.acceleration < 0.0 && rpm_diff <= 0.0))
    {
        // Still accelerating upward/downward
        output.motor_rpm += params.acceleration * params.time_interval_min.count();
        return output;
    }

    return output;
}

/**
 * @brief Apply cyclic motor control (stress-based)
 *
 * Controls motor to cycle between tau_lower and tau_upper for specified number of cycles.
 * - Manages cycle counter (num_cyclic) and direction flag (flag_cyclic)
 * - Returns true when all cycles are completed
 *
 * @param params Control parameters
 * @param num_cyclic Input/Output: current cycle counter
 * @param flag_cyclic Input/Output: current cycle direction flag
 * @param davout_motor Output: motor on/off voltage
 * @param davout_motor_speed Output: motor speed voltage
 * @param davout_motor_cruch Output: motor clutch voltage
 * @return true if all cycles completed, false otherwise
 */
inline constexpr ControlOutput apply_cyclic_motor_control(
    const control::ControlParams::CyclicMotorControlParams &params, const control::PhysicalInput &input,
    const ControlOutput &prev_state) noexcept
{
    auto state = prev_state;
    state.motor_rpm = params.motor_rpm;
    if (state.num_cyclic == 0)
    {
        state.num_cyclic++;
        // Initial phase: true = loading phase, false = unloading phase
        state.flag_cyclic = params.motor_rpm >= 0.0;
    }

    if (!state.flag_cyclic)
    {
        state.motor_rpm = -math_constexpr::abs(params.motor_rpm); // Unloading
        if (input.shear_stress_kpa() <= params.tau_lower_kpa)
        {
            state.flag_cyclic = !state.flag_cyclic;
            if (params.motor_rpm >= 0.0) // Initial sign was positive (loading start)
                state.num_cyclic++;
        }
    }
    else
    {
        state.motor_rpm = math_constexpr::abs(params.motor_rpm); // Loading
        if (input.shear_stress_kpa() >= params.tau_upper_kpa)
        {
            state.flag_cyclic = !state.flag_cyclic;
            if (params.motor_rpm < 0.0) // Initial sign was negative (unloading start)
                state.num_cyclic++;
        }
    }
    return state;
}

/**
 * @brief Apply cyclic motor control (displacement-based)
 *
 * Controls motor to cycle between displacement_lower and displacement_upper for specified number of cycles.
 * - Manages cycle counter (num_cyclic) and direction flag (flag_cyclic)
 * - Returns true when all cycles are completed
 *
 * @param params Control parameters
 * @param num_cyclic Input/Output: current cycle counter
 * @param flag_cyclic Input/Output: current cycle direction flag
 * @param davout_motor Output: motor on/off voltage
 * @param davout_motor_speed Output: motor speed voltage
 * @param davout_motor_cruch Output: motor clutch voltage
 * @return true if all cycles completed, false otherwise
 */
inline constexpr ControlOutput apply_cyclic_motor_control_disp(
    const control::ControlParams::CyclicMotorControlParams &params, const control::PhysicalInput &input,
    const ControlOutput &prev_state) noexcept
{
    auto state = prev_state;
    state.motor_rpm = params.motor_rpm;
    if (state.num_cyclic == 0)
    {
        state.num_cyclic++;
        // Initial phase: true = loading phase, false = unloading phase
        state.flag_cyclic = params.motor_rpm >= 0.0;
    }

    const auto current_es = input.shear_displacement_mm;

    if (!state.flag_cyclic)
    {
        state.motor_rpm = -math_constexpr::abs(params.motor_rpm); // Unloading
        if (current_es <= params.displacement_lower_mm)
        {
            state.flag_cyclic = !state.flag_cyclic;
            if (params.motor_rpm >= 0.0) // Initial sign was positive (loading start)
                state.num_cyclic++;
        }
    }
    else
    {
        state.motor_rpm = math_constexpr::abs(params.motor_rpm); // Loading
        if (current_es >= params.displacement_upper_mm)
        {
            state.flag_cyclic = !state.flag_cyclic;
            if (params.motor_rpm < 0.0) // Initial sign was negative (unloading start)
                state.num_cyclic++;
        }
    }
    return state;
}
