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
 * @file stateless.hpp
 * @brief Stateless control strategies
 *
 * This file contains control strategies that don't require internal state
 * beyond what's provided in the ControlContext. These are implemented as
 * free functions.
 */

#pragma once

#include "../../Constants.h"
#include "../../Variables.hpp"
#include "../control.hpp"
#include "../measurement.hpp"
#include "../patterns.hpp"
#include "../strategy_types.hpp"
#include "../utils.hpp"
#include <spdlog/spdlog.h>

namespace control::strategies
{

/**
 * @brief No control strategy - motor off, outputs hold
 */
inline constexpr ControlOutput no_control_strategy(const ControlContext &ctx) noexcept
{
    using variables::CH_Motor;
    using variables::CH_MotorSpeed;
    using variables::DAVout;

    if !consteval
    {
        spdlog::trace("NoControl strategy: stopping motor");
    }
    stop_motor(DAVout[CH_Motor], DAVout[CH_MotorSpeed]);

    auto output = ctx.control_output;
    return output;
}

/**
 * @brief Rebase reference strategy - resets displacement reference
 */
inline ControlOutput rebase_reference_strategy(const ControlContext &ctx) noexcept
{
    spdlog::trace("RebaseReference strategy: rebasing measurements");

    auto expected = ctx.physical_input.load();

    // Atomically update the physical input with rebased measurements
    while (!ctx.physical_input.compare_exchange_weak(
        expected,
        // Rebase to new present (zero-sets displacements relative to new present)
        control::rebase(expected, control::present_specimen(expected))))
    {
        // Loop continues until successful compare-exchange
    }

    auto output = ctx.control_output;
    return output;
}

} // namespace control::strategies
