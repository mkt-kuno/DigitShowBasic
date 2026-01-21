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
 * @file math_constexpr.hpp
 * @brief constexpr version for <cmath>
 *
 * This file provides constexpr implementations for common mathematical functions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits> // 追加

static constexpr auto NEWTON_RAPHSON_ITERATIONS = 80;

namespace math_constexpr
{

inline constexpr auto abs(auto x) noexcept
{
    if consteval
    {

        return (x < 0) ? -x : x;
    }
    else
    {
        return std::abs(x);
    }
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static_assert(abs(-5.0) == 5.0);
static_assert(abs(5.0f) == 5.0f);
static_assert(abs(0.0) == 0.0);
static_assert(abs(-5) == 5);
static_assert(abs(5) == 5);
static_assert(abs(0) == 0);
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

template <typename T>
concept float_or_double = std::is_same_v<T, float> || std::is_same_v<T, double>;

inline constexpr auto sqrt(float_or_double auto x) noexcept
{
    if consteval
    {
        using T = decltype(x);

        // NaN passthrough
        if (!(x == x))
            return x;

        // Negative -> NaN
        if (x < T(0))
            return std::numeric_limits<T>::quiet_NaN();

        // Zero and positive
        if (x == T(0))
            return T(0);

        // Initial guess
        T guess = std::max(x, T(1));
        if (guess == T(0))
            guess = T(1);

        // Relative tolerance scaled by value
        const T eps = std::numeric_limits<T>::epsilon() * T(4) * ((x > T(1)) ? x : T(1));

        // Newton-Raphson iterations
        for (int i = 0; i < NEWTON_RAPHSON_ITERATIONS; ++i)
        {
            T next = (guess + x / guess) / T(2);
            if (math_constexpr::abs(next - guess) <= eps)
                return next;
            guess = next;
        }

        return guess;
    }
    else
    {
        return std::sqrt(x);
    }
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static_assert(sqrt(0.0) == 0.0);
static_assert(sqrt(4.0) == 2.0);
static_assert(sqrt(2.0) == 1.414213562373094923);
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

inline constexpr auto copysign(float_or_double auto x, float_or_double auto y) noexcept
{
    if consteval
    {
        return (y < 0) ? -abs(x) : abs(x);
    }
    else
    {
        return std::copysign(x, y);
    }
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static_assert(copysign(3.0, -1.0) == -3.0);
static_assert(copysign(-3.0, 1.0) == 3.0);
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

} // namespace math_constexpr
