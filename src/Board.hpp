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

#include "chrono_alias.hpp"
#include <array>
#include <chrono>

// Forward declaration for MFC types (to be removed in future refactoring)
#include <atltime.h>

namespace board
{

inline std::array<short, 2> AdChannels = {0};    // The Number of Channel
inline std::array<short, 1> DaChannels = {0};    // The Number of Channel
inline std::array<short, 2> AdRange = {0};       // Range of A/D and D/A boards
inline std::array<short, 1> DaRange = {0};       // Range of A/D and D/A boards
inline std::array<short, 2> AdResolution = {0};  // Resolution of A/D and D/A Boards
inline std::array<short, 1> DaResolution = {0};  // Resolution of A/D and D/A Boards
inline std::array<short, 2> AdInputMethod = {0}; // Input Method of A/D
inline size_t AdMaxCH = 0;                       // Maximum Channel Number
inline int AvSmplNum = 0;

//---Flag---
inline bool Flag_SetBoard = false;

// Time tracking using std::chrono for type safety
inline std::chrono::system_clock::time_point StartTime{}; // Data logging start time
inline std::chrono::system_clock::time_point NowTime{};   // Current time
// Monotonic start time for relative elapsed computations (SequentTime1)
inline std::chrono::steady_clock::time_point StartSteadyTime{};
// Application start (wall) and monotonic anchors for synthetic wall time reconstruction
inline std::chrono::system_clock::time_point WallClockStart{};   // Captured once at app/view init
inline std::chrono::steady_clock::time_point SteadyClockStart{}; // Captured once at same moment
inline std::string SNowTime;                                     // Formatted current time string

/**
 * @brief Get synthetic stable wall-clock time (system_clock basis without jumps)
 * Combines the initial real wall clock with monotonic elapsed time.
 */
inline auto SyntheticNow() noexcept
{
    if (WallClockStart.time_since_epoch().count() == 0)
    {
        // Fallback: if not initialized yet, return system_clock::now()
        return std::chrono::system_clock::now();
    }
    auto elapsed = std::chrono::steady_clock::now() - SteadyClockStart;
    // Cast monotonic elapsed to system_clock::duration to keep time_point duration consistent
    auto elapsed_sys = std::chrono::duration_cast<std::chrono::system_clock::duration>(elapsed);
    return WallClockStart + elapsed_sys;
}

/**
 * @brief Calculate sampling times based on time interval and sampling clock
 * @param interval Time interval for sampling
 * @param samplingClock A/D sampling clock period in microseconds
 * @return Number of sampling times
 */
inline unsigned long CalculateSamplingTimes(std::chrono::microseconds interval, float samplingClock) noexcept
{
    // samplingClock is in microseconds
    return static_cast<unsigned long>(std::lround(static_cast<double>(interval.count()) / samplingClock));
}

} // namespace board
