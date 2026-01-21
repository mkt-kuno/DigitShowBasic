/*
 * DigitShowDST - Direct Shear Test Machine Control Software
 * Copyright (C) 2025 Makoto KUNO, Takuto ISHII
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
 * @file DataConvert.h
 * @brief Data conversion utilities for A/D and D/A operations
 *
 * モジュール DataConvert
 * GetRangeValue, BinaryToVolt, VoltToBinaryメソッドを公開します。
 */

#pragma once

/**
 * @brief Get range maximum and minimum values from range code
 *
 * AioSetAiRange, AioSetAoRangeで使用するレンジ値から、
 * レンジの最大値と最小値を取得します。
 *
 * @param RangeData Range code defined by AioSetAiRange, AioSetAoRange
 * @param Max Pointer to store maximum range value
 * @param Min Pointer to store minimum range value
 * @return 0 on success, -1 if RangeData is invalid or Max/Min are NULL
 */
inline constexpr long GetRangeValue(short RangeData, float *Max, float *Min) noexcept
{
    if ((Max == NULL) || (Min == NULL))
    {
        return -1;
    }
    switch (RangeData)
    {
    case 0:
        *Max = (float)10;
        *Min = (float)-10;
        break;
    case 1:
        *Max = (float)5;
        *Min = (float)-5;
        break;
    case 2:
        *Max = (float)2.5;
        *Min = (float)-2.5;
        break;
    case 3:
        *Max = (float)1.25;
        *Min = (float)-1.25;
        break;
    case 4:
        *Max = (float)1;
        *Min = (float)-1;
        break;
    case 5:
        *Max = (float)0.625;
        *Min = (float)-0.625;
        break;
    case 6:
        *Max = (float)0.5;
        *Min = (float)-0.5;
        break;
    case 7:
        *Max = (float)0.3125;
        *Min = (float)-0.3125;
        break;
    case 8:
        *Max = (float)0.25;
        *Min = (float)-0.25;
        break;
    case 9:
        *Max = (float)0.125;
        *Min = (float)-0.125;
        break;
    case 10:
        *Max = (float)0.1;
        *Min = (float)-0.1;
        break;
    case 11:
        *Max = (float)0.05;
        *Min = (float)-0.05;
        break;
    case 12:
        *Max = (float)0.025;
        *Min = (float)-0.025;
        break;
    case 13:
        *Max = (float)0.0125;
        *Min = (float)-0.0125;
        break;
    case 50:
        *Max = (float)10;
        *Min = (float)0;
        break;
    case 51:
        *Max = (float)5;
        *Min = (float)0;
        break;
    case 52:
        *Max = (float)4.095;
        *Min = (float)0;
        break;
    case 53:
        *Max = (float)2.5;
        *Min = (float)0;
        break;
    case 54:
        *Max = (float)1.25;
        *Min = (float)0;
        break;
    case 55:
        *Max = (float)1;
        *Min = (float)0;
        break;
    case 56:
        *Max = (float)0.5;
        *Min = (float)0;
        break;
    case 57:
        *Max = (float)0.25;
        *Min = (float)0;
        break;
    case 58:
        *Max = (float)0.1;
        *Min = (float)0;
        break;
    case 59:
        *Max = (float)0.05;
        *Min = (float)0;
        break;
    case 60:
        *Max = (float)0.025;
        *Min = (float)0;
        break;
    case 61:
        *Max = (float)0.0125;
        *Min = (float)0;
        break;
    case 100:
        *Max = (float)20;
        *Min = (float)0;
        break;
    case 101:
        *Max = (float)20;
        *Min = (float)4;
        break;
    case 150:
        *Max = (float)5;
        *Min = (float)1;
        break;
    default:
        *Max = (float)0;
        *Min = (float)0;
        return -1;
    }
    return 0;
}

/**
 * @brief Convert binary data to voltage or current
 *
 * バイナリデータを電圧や電流に変換します。
 *
 * @param Max Maximum range value
 * @param Min Minimum range value
 * @param Bits Resolution (12 or 16 bits)
 * @param Binary Binary data to convert
 * @return Converted voltage/current value as float
 */
inline constexpr float BinaryToVolt(float Max, float Min, short Bits, long Binary) noexcept
{
    long Resolution = 0;
    switch (Bits)
    {
    case 12:
        Resolution = 4095;
        break;
    case 16:
        Resolution = 65535;
        break;
    default:
        Resolution = 4095;
        break;
    }
    return static_cast<float>(Binary) * (Max - Min) / static_cast<float>(Resolution) + Min;
}

/**
 * @brief Convert voltage or current to binary data
 *
 * 電圧や電流をバイナリデータに変換します。
 *
 * @param Max Maximum range value
 * @param Min Minimum range value
 * @param Bits Resolution (12 or 16 bits)
 * @param Volt Voltage or current data to convert
 * @return Converted binary data as long
 */
inline constexpr long VoltToBinary(float Max, float Min, short Bits, float Volt) noexcept
{
    long Resolution = 0;
    if (Max == Min)
    {
        return 0;
    }
    switch (Bits)
    {
    case 12:
        Resolution = 4095;
        break;
    case 16:
        Resolution = 65535;
        break;
    default:
        Resolution = 4095;
        break;
    }
    return static_cast<long>(static_cast<float>(Resolution) * (Volt - Min) / (Max - Min));
}
