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

#include "Variables.hpp"
#include "control/measurement.hpp"
#include <array>
#include <atomic>
#include <expected>
#include <string>

namespace variables::physical
{

inline std::atomic<control::PhysicalInput> latest_physical_input{};
inline std::atomic<control::PhysicalOutput<>> latest_physical_output{};

/// @brief Update the latest physical state from analog inputs and outputs.
void update() noexcept;

/// @brief Set physical output values (converts to voltage) and synchronize hardware/state.
/// @param physical Physical output values (front/rear EP, motor RPM)
/// @return std::expected<void, std::string> Success or error message
[[nodiscard]] std::expected<void, std::string> set_output(const control::PhysicalOutput<> &physical) noexcept;

} // namespace variables::physical