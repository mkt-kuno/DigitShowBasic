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
 * @file strategy_factory.hpp
 * @brief Factory function for creating control strategies
 *
 * This file declares the factory function that creates appropriate control
 * strategies based on the control pattern. The factory encapsulates the
 * logic for selecting and instantiating the right strategy implementation.
 */

#pragma once

#include "patterns.hpp"
#include "strategy_types.hpp"

namespace control
{

/**
 * @brief Create a control strategy for the given pattern
 *
 * This factory function creates and returns an appropriate ControlStrategy
 * for the specified control pattern. The returned strategy can be stateless
 * (function pointer) or stateful (function object).
 *
 * @param pattern The control pattern identifier
 * @param ctx The control context (used for initializing stateful strategies)
 * @return A ControlStrategy function that can execute the pattern
 */
[[nodiscard]] ControlStrategy create_strategy(ControlPattern pattern, const ControlContext &ctx);

} // namespace control
