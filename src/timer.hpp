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

#include <chrono>
#include <cstdint>

namespace timer
{
/** @brief Timer ID for UI refresh (AD/Cal/Show) */
inline constexpr std::uintptr_t kTimerId_UI = 1;

/** @brief Timer ID for closed-loop control tick */
inline constexpr std::uintptr_t kTimerId_Control = 2;

/** @brief Timer ID for periodic logging tick */
inline constexpr std::uintptr_t kTimerId_Log = 3;

// Timer intervals using std::chrono for type safety
inline constexpr std::chrono::milliseconds TimeInterval_1{50};   // Time interval to display output data
inline constexpr std::chrono::milliseconds TimeInterval_2{500};  // Time interval to feed back
inline constinit std::chrono::milliseconds TimeInterval_3{1000}; // Time interval to save the data

} // namespace timer
