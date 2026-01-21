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
 * @file digitshow_operations.cpp
 * @brief Implementation of free functions for hardware I/O and control operations
 *
 * Implements free functions extracted from CDigitShowDSTDoc for board control,
 * analog I/O, physical state updates, and control step execution.
 */
#include "StdAfx.h"

#include "Board.hpp"
#include "aio_error_logger.hpp"
#include "board_control.hpp"
#include "digitshow_operations.hpp"
#include "physical_variables.hpp"
#include <array>
#include <span>
#include <spdlog/spdlog.h>
#include <string_view>

namespace digitshow
{
std::expected<void, std::string> read_analog_inputs() noexcept
{
    if (!board::Flag_SetBoard)
    {
        return std::unexpected("Board not initialized");
    }

    if (const auto result = board_control::ReadAnalogInputs(); !result)
    {
        aio::log_error("read_analog_inputs via board_control", result.error());
        return std::unexpected(result.error());
    }

    return {};
}

std::expected<void, std::string> write_analog_outputs() noexcept
{
    if (!board::Flag_SetBoard)
    {
        return std::unexpected("Board not initialized");
    }

    if (const auto result = board_control::WriteAnalogOutputs(); !result)
    {
        aio::log_error("write_analog_outputs via board_control", result.error());
        return std::unexpected(result.error());
    }

    return {};
}

} // namespace digitshow
