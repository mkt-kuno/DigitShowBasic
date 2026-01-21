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
 * @file version_info.hpp
 * @brief Version information utilities for configuration files
 *
 * Provides functionality to retrieve build-time git version information for embedding
 * in configuration files to track which version of the program created them.
 *
 * This header uses the auto-generated git_version.hpp which is created at build time
 * by scripts/generate_git_version.ps1.
 */

#pragma once

#include "../generated/git_version.hpp"
#include <string>
#include <string_view>

namespace version_info
{

/**
 * @brief Get version string for configuration files
 * @return Version string containing commit hash and dirty state indicator
 *
 * Returns the full git commit hash from build time. If the working directory
 * was dirty (had uncommitted changes) at build time, appends "-dirty" suffix.
 *
 * Example outputs:
 *   - "d30dfc77241073d695648a5c6ec894790a3d1bbd" (clean build)
 *   - "d30dfc77241073d695648a5c6ec894790a3d1bbd-dirty" (dirty build)
 */
inline std::string get_version_string() noexcept
{
    std::string version{git_version::COMMIT_FULL};
    if (git_version::DIRTY)
    {
        version += "-dirty";
    }
    return version;
}

/**
 * @brief Get compact version string for configuration files
 * @return Compact version string with short hash and dirty state
 *
 * Returns a shorter version string using the short commit hash (7 chars).
 * Useful when space is limited or for display purposes.
 *
 * Example outputs:
 *   - "d30dfc7" (clean build)
 *   - "d30dfc7-dirty" (dirty build)
 */
inline std::string get_version_string_short() noexcept
{
    std::string version{git_version::COMMIT_SHORT};
    if (git_version::DIRTY)
    {
        version += "-dirty";
    }
    return version;
}

/**
 * @brief Check if the build was from a dirty working directory
 * @return true if working directory had uncommitted changes at build time
 */
inline constexpr bool is_dirty() noexcept
{
    return git_version::DIRTY;
}

/**
 * @brief Get the full commit hash from build time
 * @return Full 40-character git commit hash
 */
inline constexpr std::string_view get_commit_hash() noexcept
{
    return git_version::COMMIT_FULL;
}

/**
 * @brief Get the short commit hash from build time
 * @return Short (7-character) git commit hash
 */
inline constexpr std::string_view get_commit_hash_short() noexcept
{
    return git_version::COMMIT_SHORT;
}

} // namespace version_info
