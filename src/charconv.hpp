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

#include "StdAfx.h"

#include <string>
#include <string_view>

inline auto to_wstring_utf8(std::string_view text)
{
    return std::wstring{CA2W(text.data(), CP_UTF8)};
}

inline auto to_utf8(const CStringW &text)
{
    return std::string{CW2A(text.GetString(), CP_UTF8)};
}

inline constexpr std::string normalize_to_crlf(std::string_view text)
{
    std::string out;
    out.reserve(text.size() + 8);
    char prev = '\0';
    for (const char ch : text)
    {
        if (ch == '\n')
        {
            if (prev != '\r')
                out.push_back('\r');
            out.push_back('\n');
        }
        else if (ch != '\r')
        {
            out.push_back(ch);
        }
        prev = ch;
    }
    return out;
}

inline constexpr std::string normalize_to_lf(std::string_view text)
{
    std::string out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i)
    {
        if (text[i] == '\r')
        {
            if (i + 1 < text.size() && text[i + 1] == '\n')
                continue; // skip '\r' in CRLF pairs
            continue;     // skip stray '\r'
        }
        out.push_back(text[i]);
    }
    return out;
}