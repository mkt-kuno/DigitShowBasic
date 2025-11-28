/*
 * DigitShowBasic - Triaxial Test Machine Control Software
 * Copyright (C) 2025 Makoto KUNO
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

#ifndef DATACONVERT_H
#define DATACONVERT_H

#pragma once

/**
 * Get range values from AioSetAiRange/AioSetAoRange range code
 * @param RangeData Range code
 * @param Max Pointer to store maximum value
 * @param Min Pointer to store minimum value
 * @return 0 on success, -1 on error
 */
inline long GetRangeValue(short RangeData, float* Max, float* Min)
{
    if (Max == NULL || Min == NULL) {
        return -1;
    }
    switch (RangeData) {
    case 0:   *Max = 10.0f;     *Min = -10.0f;    break;
    case 1:   *Max = 5.0f;      *Min = -5.0f;     break;
    case 2:   *Max = 2.5f;      *Min = -2.5f;     break;
    case 3:   *Max = 1.25f;     *Min = -1.25f;    break;
    case 4:   *Max = 1.0f;      *Min = -1.0f;     break;
    case 5:   *Max = 0.625f;    *Min = -0.625f;   break;
    case 6:   *Max = 0.5f;      *Min = -0.5f;     break;
    case 7:   *Max = 0.3125f;   *Min = -0.3125f;  break;
    case 8:   *Max = 0.25f;     *Min = -0.25f;    break;
    case 9:   *Max = 0.125f;    *Min = -0.125f;   break;
    case 10:  *Max = 0.1f;      *Min = -0.1f;     break;
    case 11:  *Max = 0.05f;     *Min = -0.05f;    break;
    case 12:  *Max = 0.025f;    *Min = -0.025f;   break;
    case 13:  *Max = 0.0125f;   *Min = -0.0125f;  break;
    case 50:  *Max = 10.0f;     *Min = 0.0f;      break;
    case 51:  *Max = 5.0f;      *Min = 0.0f;      break;
    case 52:  *Max = 4.095f;    *Min = 0.0f;      break;
    case 53:  *Max = 2.5f;      *Min = 0.0f;      break;
    case 54:  *Max = 1.25f;     *Min = 0.0f;      break;
    case 55:  *Max = 1.0f;      *Min = 0.0f;      break;
    case 56:  *Max = 0.5f;      *Min = 0.0f;      break;
    case 57:  *Max = 0.25f;     *Min = 0.0f;      break;
    case 58:  *Max = 0.1f;      *Min = 0.0f;      break;
    case 59:  *Max = 0.05f;     *Min = 0.0f;      break;
    case 60:  *Max = 0.025f;    *Min = 0.0f;      break;
    case 61:  *Max = 0.0125f;   *Min = 0.0f;      break;
    case 100: *Max = 20.0f;     *Min = 0.0f;      break;
    case 101: *Max = 20.0f;     *Min = 4.0f;      break;
    case 150: *Max = 5.0f;      *Min = 1.0f;      break;
    default:  *Max = 0.0f;      *Min = 0.0f;      return -1;
    }
    return 0;
}

/**
 * Convert binary data to voltage/current
 * @param Max Range maximum
 * @param Min Range minimum
 * @param Bits Resolution (12 or 16)
 * @param Binary Binary data to convert
 * @return Converted voltage/current value
 */
inline float BinaryToVolt(float Max, float Min, short Bits, long Binary)
{
    long Resolution = (Bits == 16) ? 65535 : 4095;
    return Binary * (Max - Min) / Resolution + Min;
}

/**
 * Convert voltage/current to binary data
 * @param Max Range maximum
 * @param Min Range minimum
 * @param Bits Resolution (12 or 16)
 * @param Volt Voltage/current value to convert
 * @return Converted binary data
 */
inline long VoltToBinary(float Max, float Min, short Bits, float Volt)
{
    if (Max == Min) {
        return 0;
    }
    long Resolution = (Bits == 16) ? 65535 : 4095;
    return (long)(Resolution * (Volt - Min) / (Max - Min));
}

#endif // DATACONVERT_H

