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

#include "control/measurement.hpp"
#include "digitshow_operations.hpp"
#include "physical_variables.hpp"
#include <array>
#include <expected>
#include <format>
#include <string>

namespace variables
{

inline constexpr size_t MAX_AI_CHANNELS = 64;
inline constexpr size_t MAX_DA_CHANNELS = 8;

// D/A channel indices shared across modules
inline constexpr size_t CH_Motor = 0;      // D/A Channel of Motor On / Off
inline constexpr size_t CH_MotorCruch = 1; // D/A Channel of Motor Cruch (Loading/Unloading)
inline constexpr size_t CH_MotorSpeed = 2; // D/A Channel of Motor Speed
inline constexpr size_t CH_EP_Cell_f = 3;  // D/A Channel of EP (Bellofram_f)
inline constexpr size_t CH_EP_Cell_r = 4;  // D/A Channel of EP (Bellofram_r)

//---Array---
inline std::array<float, MAX_AI_CHANNELS> Vout = {};    // Output Voltage from A/D board
inline std::array<double, MAX_AI_CHANNELS> Phyout = {}; // Physical Value Calculated from Vout
inline std::array<double, MAX_AI_CHANNELS> Cal_a = {};  // A/D Calibration Factor
inline std::array<double, MAX_AI_CHANNELS>
    Cal_b = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // A/D Calibration Factor
inline std::array<double, MAX_AI_CHANNELS> Cal_c = {};                                        // A/D Calibration Factor
inline std::array<float, MAX_AI_CHANNELS> AmpPB = {};  // Physical value at base point
inline std::array<float, MAX_AI_CHANNELS> AmpPO = {};  // Physical value at offset point
inline std::array<float, MAX_DA_CHANNELS> DAVout = {}; // Output Voltage to D/A board
inline std::array<double, MAX_DA_CHANNELS> DA_Cal_a = {0.0,         0.0, 0.0033333, 0.017854906,
                                                       0.018384256, 0.0, 0.0,       0.0}; // D/A Calibration Factor
inline std::array<double, MAX_DA_CHANNELS> DA_Cal_b = {0.0,          0.0, 0.0, -0.286962967,
                                                       -0.335375138, 0.0, 0.0, 0.0}; // D/A Calibration Factor

//---SpecimenData---
// Structure of Initial Specimen Data
inline control::SpecimenSnapshot SpecimenData = {
    120.0,   // Height [mm]
    14400.0, // Area [mm²] (120*120)
    0.0,     // Weight [g]
    10000.0  // BoxWeight [g]
};

/// @brief Set all D/A output voltages and synchronize hardware/state.
/// @param voltages Array of voltage values to set (must have MAX_DA_CHANNELS elements)
/// @return std::expected<void, std::string> Success or error message
[[nodiscard]] std::expected<void, std::string> set_output_voltages(
    const std::array<float, MAX_DA_CHANNELS> &voltages) noexcept;

inline constexpr void calc_physical() noexcept
{

    for (size_t i = 0; i < MAX_AI_CHANNELS; ++i)
    {
        Phyout[i] = Cal_a[i] * Vout[i] * Vout[i] + Cal_b[i] * Vout[i] + Cal_c[i];
    }
}
} // namespace variables
