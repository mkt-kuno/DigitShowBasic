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
 * @file control/params.hpp
 * @brief Named parameter structure for control steps
 *
 * This file defines ControlParams, which replaces the legacy CFPARA array-based
 * representation with named fields for better maintainability and type safety.
 * Conversion functions provide backward compatibility with existing code.
 */

#pragma once

#include "../chrono_alias.hpp"
#include "patterns.hpp"
#include <string>

namespace control
{

/**
 * @brief Named parameters for a control step
 *
 * This structure represents all control parameters with meaningful names
 * instead of array indices. Fields correspond to the CFPARA[0..17] array
 * as documented in knowledge/control_specifications.md.
 *
 * Usage across different control patterns:
 * - motor_rpm: CFPARA[0] - Motor speed in RPM with sign indicating direction: positive=load/compression,
 * negative=unload/dilation
 * - target_tau_kpa: CFPARA[1] - Target shear stress (kPa)
 * - target_sigma_kpa: CFPARA[2] - Target confining pressure (kPa)
 * - duration: CFPARA[3] or CFPARA[0] - Duration for creep/relaxation (minutes, as minutes_d)
 * - cycles: CFPARA[4] - Number of cycles for cyclic loading
 * - Sensitivity overrides: CFPARA[10..17]
 */
struct ControlParams
{
    // Control pattern identifier
    ControlPattern pattern = ControlPattern::NoControl;

    // Primary control parameters (CFPARA[0..9])
    std::chrono::minutes_d duration{0.0}; // [2] or [0] Duration for time-based patterns (minutes)

    // Additional pattern-specific parameters
    double consolidation_rate_kpa_per_min = 0.0; // [3] Consolidation rate

    struct MonotonicLoading
    {
        double motor_rpm = 0.0;              // [0] Motor speed in RPM
        double target_tau_kpa = 0.0;         // [1] Target shear stress (kPa)
        double target_displacement_mm = 0.0; // [1] Target displacement (mm)
    } monotonic_loading;

    /**
     * @brief Parameters for cyclic motor control (stress-based)
     */
    struct CyclicMotorControlParams
    {
        double motor_rpm = 0.0; // Motor speed in RPM, sign indicates direction: positive=loading, negative=unloading
        double tau_lower_kpa = 0.0;         // [2] Tau lower bound for cyclic patterns
        double tau_upper_kpa = 0.0;         // [3] Tau upper bound for cyclic patterns
        double displacement_lower_mm = 0.0; // [2] Displacement lower bound for cyclic displacement patterns
        double displacement_upper_mm = 0.0; // [3] Displacement upper bound for cyclic displacement patterns
        size_t cycles;                      // Number of cycles
    } cyclic_loading;

    /**
     * @brief Parameters for motor acceleration control
     */
    struct MotorAccelerationParams
    {
        double start_rpm;                         // Starting RPM
        double target_rpm;                        // Target RPM
        double acceleration;                      // Acceleration rate (RPM/min), sign indicates direction
        double err_rpm = 1.0;                     // RPM error tolerance
        std::chrono::minutes_d time_interval_min; // Time interval in minutes
    } motor_acceleration;

    /**
     * @brief Parameters for K-consolidation (Linear stress path) (CFNUM=16)
     */
    struct PathMLoadingParams
    {
        double target_tau{};  // [2] Target tau (kPa)
        double sigma_start{}; // [3] Initial sigma (kPa)
        double sigma_end{};   // [4] Final sigma (kPa)
        double k_value{};     // [5] K value (stress path slope) TODO: 計算中で使っていないバグあり、後日直す
        double motor_rpm{};   // [6] Motor speed (RPM)
    } path_mloading;

    struct VerticalStress
    {
        double setpoint = 0.0;     // [10] Stress setpoint (kPa)
        double error = 0.5;        // [11] Stress error (kPa)
        double ki = 0.5;           // [12] Stress gain
        double cv_limit_kpa = 1.2; // EP output (control variable) limit a tick (kPa)
    } vertical_stress_kpa;

    struct ShearStress
    {
        double setpoint = 0.0;
        double error = 0.5;
        double kp_rpm_per_kpa = 0.5;
        double cv_limit_rpm = 3000.0; // Motor output (control variable) limit a tick (RPM)
    } shear_stress_kpa;

    struct NormalDisplacement
    {
        double setpoint = 0.0; // [2] Target displacement for displacement-based patterns
        double error = 0.002;
        double ki_kpa_per_mm = 0.5;
        double cv_limit_kpa = 1.2; // EP output (control variable) limit a tick (kPa)
    } normal_displacement_mm;

    struct Tilt
    {
        double setpoint = 0.0;
        double error = 0.002;
        double ki_kpa_per_mm = 0.5;
        double cv_limit_kpa = 1.2; // EP output (control variable) limit a tick (kPa)
    } tilt_mm;

    // Name field for step identification
    std::string name;
};

} // namespace control
