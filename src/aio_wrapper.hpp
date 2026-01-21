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
 * @file aio_wrapper.hpp
 * @brief Type-safe wrapper for CONTEC AIO API with std::expected error handling
 *
 * This wrapper provides modern C++23 error handling for CONTEC analog I/O operations.
 * Error messages from AioGetErrorString (Shift_JIS/CP932) are converted to UTF-8.
 */

#pragma once

#include "Caio.h"
#include <Windows.h>
#include <expected>
#include <memory>
#include <string>

namespace aio
{

/**
 * @brief Convert Shift_JIS (CP932) string to UTF-8
 * @param shiftjis_str Input string in Shift_JIS encoding
 * @return UTF-8 encoded string
 */
[[nodiscard]] inline std::string shiftjis_to_utf8(const char *const shiftjis_str) noexcept
{
    if (shiftjis_str == nullptr || shiftjis_str[0] == '\0')
    {
        return "";
    }

    // Convert Shift_JIS to wide char (UTF-16)
    const int wide_len = MultiByteToWideChar(CP_ACP, 0, shiftjis_str, -1, nullptr, 0);
    if (wide_len <= 0)
    {
        return "";
    }

    auto wide_str = std::make_unique<wchar_t[]>(static_cast<size_t>(wide_len));
    MultiByteToWideChar(CP_ACP, 0, shiftjis_str, -1, wide_str.get(), wide_len);

    // Convert wide char (UTF-16) to UTF-8
    const int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_str.get(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8_len <= 0)
    {
        return "";
    }

    auto utf8_str = std::make_unique<char[]>(static_cast<size_t>(utf8_len));
    WideCharToMultiByte(CP_UTF8, 0, wide_str.get(), -1, utf8_str.get(), utf8_len, nullptr, nullptr);

    return std::string(utf8_str.get());
}

/**
 * @brief Get error message from AIO error code (converted to UTF-8)
 * @param error_code Error code from AIO API
 * @return UTF-8 encoded error message
 */
[[nodiscard]] inline std::string get_error_message(const long error_code) noexcept
{
    std::array<char, 256> buffer = {};
    AioGetErrorString(error_code, buffer.data());
    return shiftjis_to_utf8(buffer.data());
}

/**
 * @brief Initialize AIO device
 * @param deviceName Device name (e.g., "AIO000", "AIO001")
 * @return Expected with device ID or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> init(char *const deviceName) noexcept
{
    short id = 0;
    const auto ret = AioInit(deviceName, &id);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return id;
}

/**
 * @brief Exit/close AIO device
 * @param id Device ID
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> exit(const short id) noexcept
{
    const auto ret = AioExit(id);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Reset AIO device
 * @param id Device ID
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> resetDevice(const short id) noexcept
{
    const auto ret = AioResetDevice(id);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Get analog input method
 * @param id Device ID
 * @return Expected with input method or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAiInputMethod(const short id) noexcept
{
    short method = 0;
    const auto ret = AioGetAiInputMethod(id, &method);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return method;
}

/**
 * @brief Get analog input resolution
 * @param id Device ID
 * @return Expected with resolution or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAiResolution(const short id) noexcept
{
    short resolution = 0;
    const auto ret = AioGetAiResolution(id, &resolution);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return resolution;
}

/**
 * @brief Get maximum number of analog input channels
 * @param id Device ID
 * @return Expected with max channels or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAiMaxChannels(const short id) noexcept
{
    short maxChannels = 0;
    const auto ret = AioGetAiMaxChannels(id, &maxChannels);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return maxChannels;
}

/**
 * @brief Set number of analog input channels
 * @param id Device ID
 * @param channels Number of channels
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAiChannels(const short id, const short channels) noexcept
{
    const auto ret = AioSetAiChannels(id, channels);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Set analog input range for all channels
 * @param id Device ID
 * @param range Range constant (e.g., PM10 for ±10V)
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAiRangeAll(const short id, const short range) noexcept
{
    const auto ret = AioSetAiRangeAll(id, range);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Get analog input range for a channel
 * @param id Device ID
 * @param ch Channel number
 * @return Expected with range or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAiRange(const short id, const short ch) noexcept
{
    short range = 0;
    const auto ret = AioGetAiRange(id, ch, &range);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return range;
}

/**
 * @brief Get analog input memory type
 * @param id Device ID
 * @return Expected with memory type or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAiMemoryType(const short id) noexcept
{
    short memoryType = 0;
    const auto ret = AioGetAiMemoryType(id, &memoryType);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return memoryType;
}

/**
 * @brief Set analog input sampling clock
 * @param id Device ID
 * @param samplingClock Sampling clock period in microseconds
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAiSamplingClock(const short id,
                                                                         const float samplingClock) noexcept
{
    const auto ret = AioSetAiSamplingClock(id, samplingClock);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Get analog input sampling clock
 * @param id Device ID
 * @return Expected with sampling clock or UTF-8 error message
 */
[[nodiscard]] inline std::expected<float, std::string> getAiSamplingClock(const short id) noexcept
{
    float samplingClock = 0.0f;
    const auto ret = AioGetAiSamplingClock(id, &samplingClock);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return samplingClock;
}

/**
 * @brief Set analog input event sampling times
 * @param id Device ID
 * @param samplingTimes Number of sampling times
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAiEventSamplingTimes(
    const short id, const unsigned long samplingTimes) noexcept
{
    const auto ret = AioSetAiEventSamplingTimes(id, static_cast<long>(samplingTimes));
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Get analog input event sampling times
 * @param id Device ID
 * @return Expected with sampling times or UTF-8 error message
 */
[[nodiscard]] inline std::expected<long, std::string> getAiEventSamplingTimes(const short id) noexcept
{
    long samplingTimes = 0;
    const auto ret = AioGetAiEventSamplingTimes(id, &samplingTimes);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return samplingTimes;
}

/**
 * @brief Set analog input stop trigger
 * @param id Device ID
 * @param stopTrigger Stop trigger mode
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAiStopTrigger(const short id, const short stopTrigger) noexcept
{
    const auto ret = AioSetAiStopTrigger(id, stopTrigger);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Reset analog input memory
 * @param id Device ID
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> resetAiMemory(const short id) noexcept
{
    const auto ret = AioResetAiMemory(id);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Set analog input event
 * @param id Device ID
 * @param hWnd Window handle for event notification
 * @param event Event flags
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAiEvent(const short id, HWND hWnd, const long event) noexcept
{
    const auto ret = AioSetAiEvent(id, hWnd, event);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Start analog input operation
 * @param id Device ID
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> startAi(const short id) noexcept
{
    const auto ret = AioStartAi(id);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Stop analog input operation
 * @param id Device ID
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> stopAi(const short id) noexcept
{
    const auto ret = AioStopAi(id);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Get analog input sampling count
 * @param id Device ID
 * @return Expected with sampling count or UTF-8 error message
 */
[[nodiscard]] inline std::expected<long, std::string> getAiSamplingCount(const short id) noexcept
{
    long samplingCount = 0;
    const auto ret = AioGetAiSamplingCount(id, &samplingCount);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return samplingCount;
}

/**
 * @brief Get analog input sampling data
 * Note: Buffer is kept as pointer parameter due to potentially large data size
 * @param id Device ID
 * @param samplingTimes Number of sampling times (input/output)
 * @param data Output buffer for sampling data
 * @return Expected with actual sampling times or UTF-8 error message
 */
[[nodiscard]] inline std::expected<long, std::string> getAiSamplingData(const short id, long samplingTimes,
                                                                        long *const data) noexcept
{
    const auto ret = AioGetAiSamplingData(id, &samplingTimes, data);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return samplingTimes;
}

/**
 * @brief Get analog output resolution
 * @param id Device ID
 * @return Expected with resolution or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAoResolution(const short id) noexcept
{
    short resolution = 0;
    const auto ret = AioGetAoResolution(id, &resolution);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return resolution;
}

/**
 * @brief Get maximum number of analog output channels
 * @param id Device ID
 * @return Expected with max channels or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAoMaxChannels(const short id) noexcept
{
    short maxChannels = 0;
    const auto ret = AioGetAoMaxChannels(id, &maxChannels);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return maxChannels;
}

/**
 * @brief Set analog output range for all channels
 * @param id Device ID
 * @param range Range constant (e.g., P10 for 0-10V)
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> setAoRangeAll(const short id, const short range) noexcept
{
    const auto ret = AioSetAoRangeAll(id, range);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

/**
 * @brief Get analog output range for a channel
 * @param id Device ID
 * @param ch Channel number
 * @return Expected with range or UTF-8 error message
 */
[[nodiscard]] inline std::expected<short, std::string> getAoRange(const short id, const short ch) noexcept
{
    short range = 0;
    const auto ret = AioGetAoRange(id, ch, &range);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return range;
}

/**
 * @brief Perform multi-channel analog output
 * Note: Data buffer is kept as pointer parameter due to potentially large data size
 * @param id Device ID
 * @param channels Number of channels
 * @param data Output data array
 * @return Expected with void or UTF-8 error message
 */
[[nodiscard]] inline std::expected<void, std::string> multiAo(const short id, const short channels,
                                                              long *const data) noexcept
{
    const auto ret = AioMultiAo(id, channels, data);
    if (ret != 0)
    {
        return std::unexpected(get_error_message(ret));
    }
    return {};
}

} // namespace aio
