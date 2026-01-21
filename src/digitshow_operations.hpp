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
 * @file digitshow_operations.hpp
 * @brief Free functions for hardware I/O and control operations
 *
 * Provides free functions extracted from CDigitShowDSTDoc for board control,
 * analog I/O, physical state updates, and control step execution.
 */
#pragma once

#include <expected>
#include <string>

namespace digitshow
{
// Hardware I/O operations

/// @brief Read analog inputs via board_control layer.
/// @return std::expected<void, std::string> Success on void, or error message on failure.
[[nodiscard]] std::expected<void, std::string> read_analog_inputs() noexcept;

/// @brief Write analog outputs via board_control layer.
/// @return std::expected<void, std::string> Success on void, or error message on failure.
[[nodiscard]] std::expected<void, std::string> write_analog_outputs() noexcept;

} // namespace digitshow
