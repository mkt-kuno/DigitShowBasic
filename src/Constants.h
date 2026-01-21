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
 * @file Constants.h
 * @brief Small, scoped constants to reduce magic numbers
 *
 * This header introduces inline constexpr values to replace timer IDs
 * and common unit factors in a non-invasive way.
 */

#pragma once

/**
 * @namespace dsb
 * @brief DigitShowDST namespace containing constants and utilities
 */
namespace dsb
{
/** @brief The Number of A/D Board ( NUMAD=1-2 ) */
inline constexpr int NUMAD = 1;
/** @brief The Number of D/A Board ( NUMDA=1 ) */
inline constexpr int NUMDA = 1;

} // namespace dsb
