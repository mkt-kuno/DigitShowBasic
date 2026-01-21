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

#include "StdAfx.h"

#include "Variables.hpp"

namespace variables
{
std::expected<void, std::string> set_output_voltages(const std::array<float, MAX_DA_CHANNELS> &voltages) noexcept
{
    // Copy all voltages to DAVout
    DAVout = voltages;

    if (auto result = digitshow::write_analog_outputs(); !result)
    {
        return std::unexpected(std::format("Hardware write failed: {}", result.error()));
    }

    physical::update();
    return {};
}
} // namespace variables