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
 * @file board_control.cpp
 * @brief Implementation of high-level board control layer
 */

#include "StdAfx.h"

#include "Caio.h"
#include "Constants.h"
#include "DataConvert.h"
#include "TsvWriter.hpp"
#include "Variables.hpp"
#include "aio_error_logger.hpp"
#include "aio_wrapper.hpp"
#include "board_control.hpp"
#include "timer.hpp"
#include <algorithm>
#include <format>
#include <memory>
#include <mutex>
#include <ranges>
#include <spdlog/spdlog.h>

// Static state for range conversions
static std::array<float, 2> adRangeMax = {0.0f};
static std::array<float, 2> adRangeMin = {0.0f};
static std::array<float, 1> daRangeMax = {0.0f};
static std::array<float, 1> daRangeMin = {0.0f};

static std::array<short, 2> AdId = {0}; // ID
static std::array<short, 1> DaId = {0}; // ID

static std::array<std::array<long, 262144>, 2> AdData = {}; // Array of data for A/D (2 boards, max 262144 samples each)

namespace board_control
{

template <size_t Extent1, size_t Extent2>
std::expected<InitResult, std::string> InitializeBoards(
    std::span<const std::string_view, Extent1> adDeviceNames,
    std::span<const std::string_view, Extent2> daDeviceNames) noexcept
{
    using namespace board;

    spdlog::info("InitializeBoards called");

    if (Flag_SetBoard)
    {
        spdlog::warn("Board already initialized");
        return std::unexpected("Board already initialized");
    }

    InitResult result;

    // Initialize A/D boards
    const size_t numAdBoards = std::min(adDeviceNames.size(), static_cast<size_t>(dsb::NUMAD));
    for (size_t i = 0; i < numAdBoards; ++i)
    {
        const auto devName = const_cast<char *>(adDeviceNames[i].data());
        const auto initResult = aio::init(devName);
        if (!initResult)
        {
            aio::log_error(std::format("AioInit({})", adDeviceNames[i]), initResult.error());
            return std::unexpected(std::format("AioInit({}) failed: {}", adDeviceNames[i], initResult.error()));
        }
        AdId[i] = *initResult;
        result.adIds.push_back(*initResult);

        const auto resetResult = aio::resetDevice(AdId[i]);
        if (!resetResult)
        {
            aio::log_error(std::format("AioResetDevice({})", adDeviceNames[i]), resetResult.error());
            return std::unexpected(std::format("AioResetDevice({}) failed: {}", adDeviceNames[i], resetResult.error()));
        }
        spdlog::info("A/D board {} initialized successfully", i);
    }

    // Initialize D/A boards
    const size_t numDaBoards = std::min(daDeviceNames.size(), static_cast<size_t>(dsb::NUMDA));
    for (size_t i = 0; i < numDaBoards; ++i)
    {
        const auto devName = const_cast<char *>(daDeviceNames[i].data());
        const auto initResult = aio::init(devName);
        if (!initResult)
        {
            aio::log_error(std::format("AioInit({})", daDeviceNames[i]), initResult.error());
            return std::unexpected(std::format("AioInit({}) failed: {}", daDeviceNames[i], initResult.error()));
        }
        DaId[i] = *initResult;
        result.daIds.push_back(*initResult);

        const auto resetResult = aio::resetDevice(DaId[i]);
        if (!resetResult)
        {
            aio::log_error(std::format("AioResetDevice({})", daDeviceNames[i]), resetResult.error());
            return std::unexpected(std::format("AioResetDevice({}) failed: {}", daDeviceNames[i], resetResult.error()));
        }
        spdlog::info("D/A board {} initialized successfully", i);
    }

    // Configure A/D sampling conditions
    spdlog::debug("Configuring sampling conditions");
    board::AdMaxCH = 0;
    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        auto configResult = aio::getAiInputMethod(AdId[i])
                                .and_then([&](short method) {
                                    AdInputMethod[i] = method;
                                    return aio::getAiResolution(AdId[i]);
                                })
                                .and_then([&](short resolution) {
                                    AdResolution[i] = resolution;
                                    return aio::getAiMaxChannels(AdId[i]);
                                })
                                .and_then([&](short channels) {
                                    AdChannels[i] = channels;
                                    AdMaxCH += channels / 2;
                                    return aio::setAiChannels(AdId[i], channels);
                                })
                                .and_then([&]() { return aio::setAiRangeAll(AdId[i], 0); })
                                .and_then([&]() { return aio::getAiRange(AdId[i], 0); })
                                .transform([&](short range) {
                                    AdRange[i] = range;
                                    return GetRangeValue(range, &adRangeMax[i], &adRangeMin[i]);
                                });

        if (!configResult)
        {
            aio::log_error(std::format("A/D board {} configuration", i), configResult.error());
            return std::unexpected(std::format("A/D board {} configuration failed: {}", i, configResult.error()));
        }
        spdlog::debug("A/D board {} config: channels={}, resolution={}, range=±10V", i, AdChannels[i], AdResolution[i]);
    }

    // Set default sampling parameters
    using namespace std::literals::chrono_literals;
    AvSmplNum = 20;
    spdlog::info("Sampling configuration: total_channels={}", AdMaxCH);

    // Configure D/A boards
    for (size_t i = 0; i < numDaBoards; i++)
    {
        auto configResult = aio::getAoResolution(DaId[i])
                                .and_then([&](short resolution) {
                                    DaResolution[i] = resolution;
                                    return aio::getAoMaxChannels(DaId[i]);
                                })
                                .and_then([&](short channels) {
                                    DaChannels[i] = channels;
                                    return aio::setAoRangeAll(DaId[i], PM10);
                                })
                                .and_then([&]() { return aio::getAoRange(DaId[i], 0); })
                                .transform([&](short range) {
                                    DaRange[i] = range;
                                    GetRangeValue(range, &daRangeMax[i], &daRangeMin[i]);
                                    return 0;
                                });

        if (!configResult)
        {
            aio::log_error(std::format("D/A board {} configuration", i), configResult.error());
            return std::unexpected(std::format("D/A board {} configuration failed: {}", i, configResult.error()));
        }
        spdlog::debug("D/A board {} config: channels={}, resolution={}, range=±10V", i, DaChannels[i], DaResolution[i]);
    }

    Flag_SetBoard = true;
    spdlog::info("Board initialization completed successfully");
    return result;
}

std::expected<void, std::string> ResetAllVoltageOutputs() noexcept
{
    using namespace board;
    using namespace variables;

    spdlog::info("Resetting all D/A voltage outputs to 0V");

    if (!Flag_SetBoard)
    {
        spdlog::debug("Board not initialized (Flag_SetBoard is false), skipping voltage reset");
        return {};
    }

    // Reset all DAVout channels to 0.0f
    for (size_t i = 0; i < MAX_DA_CHANNELS; ++i)
    {
        DAVout[i] = 0.0f;
    }

    // Write 0V to all D/A boards
    const auto result = WriteAnalogOutputs();
    if (!result)
    {
        spdlog::error("Failed to reset voltage outputs: {}", result.error());
        return result;
    }

    spdlog::info("All D/A voltage outputs reset to 0V successfully");
    return {};
}

std::expected<void, std::string> CloseBoards() noexcept
{
    using namespace board;

    spdlog::info("CloseBoards called");

    if (!Flag_SetBoard)
    {
        spdlog::debug("No boards to close (Flag_SetBoard is false)");
        return {};
    }

    // Close A/D boards
    spdlog::debug("Closing {} A/D boards", dsb::NUMAD);
    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        if (auto result = aio::exit(AdId[i]); !result)
        {
            aio::log_warning(std::format("Closing A/D board {}", i), result.error());
        }
        spdlog::debug("A/D board {} closed", i);
    }

    // Close D/A boards
    spdlog::debug("Closing {} D/A boards", dsb::NUMDA);
    for (size_t i = 0; i < dsb::NUMDA; i++)
    {
        if (auto result = aio::exit(DaId[i]); !result)
        {
            aio::log_warning(std::format("Closing D/A board {}", i), result.error());
        }
        spdlog::debug("D/A board {} closed", i);
    }

    Flag_SetBoard = false;
    spdlog::info("All boards closed successfully");
    return {};
}

std::expected<void, std::string> ReadAnalogInputs() noexcept
{
    using namespace board;
    using namespace variables;

    size_t k = 0;
    for (size_t b = 0; b < static_cast<size_t>(dsb::NUMAD); ++b)
    {
        const auto ch_half = static_cast<size_t>(AdChannels[b]) / 2;
        for (size_t i = 0; i < ch_half; ++i)
        {
            Vout[k] = 0.0f;
            for (size_t j = 0; j < static_cast<size_t>(AvSmplNum); ++j)
            {
                Vout[k] +=
                    BinaryToVolt(adRangeMax[b], adRangeMin[b], AdResolution[b], AdData[b][AdChannels[b] * j + 2 * i]) /
                    static_cast<float>(AvSmplNum);
            }
            ++k;
        }
    }
    return {};
}

std::expected<void, std::string> WriteAnalogOutputs() noexcept
{
    using namespace board;
    using namespace variables;

    size_t k = 0;
    const size_t numDaBoards = static_cast<size_t>(std::min(dsb::NUMDA, 1));
    for (size_t i = 0; i < numDaBoards; i++)
    {
        std::array<long, 8> daData = {0};
        for (size_t j = 0; std::cmp_less(j, DaChannels[i]); j++)
        {
            // Clamp output voltage
            if (DAVout[k] < 0.0f)
                DAVout[k] = 0.0f;
            if (DAVout[k] > 9.9999f)
                DAVout[k] = 9.9999f;
            daData[j] = VoltToBinary(daRangeMax[i], daRangeMin[i], DaResolution[i], DAVout[k]);
            k++;
        }

        if (auto result = aio::multiAo(DaId[i], DaChannels[i], &daData[0]); !result)
        {
            aio::log_error(std::format("WriteAnalogOutputs board {}", i), result.error());
            return std::unexpected(result.error());
        }
    }
    return {};
}

std::expected<void, std::string> ConfigureFifoSampling(HWND hWnd, const float samplingClockUs,
                                                       const unsigned long samplingTimes) noexcept
{
    using namespace board;

    spdlog::info("ConfigureFifoSampling: clock={}us, times={}", samplingClockUs, samplingTimes);

    // Set up event flags
    const auto AdEvent = AIE_DATA_NUM | AIE_OFERR | AIE_SCERR | AIE_ADERR;

    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        auto result = aio::setAiSamplingClock(AdId[i], samplingClockUs)
                          .and_then([&]() { return aio::setAiEventSamplingTimes(AdId[i], samplingTimes); })
                          .and_then([&]() { return aio::getAiEventSamplingTimes(AdId[i]); })
                          .and_then([&](auto) { return aio::setAiStopTrigger(AdId[i], 4); })
                          .and_then([&]() { return aio::resetAiMemory(AdId[i]); })
                          .and_then([&]() { return aio::setAiEvent(AdId[i], hWnd, AdEvent); });

        if (!result)
        {
            aio::log_error(std::format("ConfigureFifoSampling A/D board {}", i), result.error());
            return std::unexpected(std::format("FIFO configuration failed for board {}: {}", i, result.error()));
        }
    }

    spdlog::info("FIFO sampling configured successfully");
    return {};
}

std::expected<long, std::string> GetMinimumSamplingCount() noexcept
{
    return std::ranges::min(std::views::iota(0, dsb::NUMAD) | std::views::transform([&](size_t i) {
                                return aio::getAiSamplingCount(AdId[i]).value_or(0);
                            }));
}

std::expected<long, std::string> GetSamplingData(const long samplingTimes) noexcept
{
    using namespace board;

    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        const auto result = aio::getAiSamplingData(AdId[i], samplingTimes, &AdData[i][0]);
        if (!result)
        {
            aio::log_error(std::format("Getting A/D sampling data board {}", i), result.error());
            return std::unexpected(std::format("GetSamplingData failed for board {}: {}", i, result.error()));
        }
    }

    return samplingTimes;
}

std::expected<void, std::string> ResetAndRestartSampling() noexcept
{
    using namespace board;

    spdlog::info("ResetAndRestartSampling called");

    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        auto result = aio::resetAiMemory(AdId[i]).and_then([&]() { return aio::startAi(AdId[i]); });
        if (!result)
        {
            aio::log_error(std::format("Resetting/Starting A/D board {}", i), result.error());
            return std::unexpected(std::format("Reset/restart failed for board {}: {}", i, result.error()));
        }
    }

    spdlog::info("All boards reset and restarted");
    return {};
}

std::expected<void, std::string> ConfigureInitialSampling(HWND hWnd) noexcept
{
    using namespace board;

    spdlog::info("ConfigureInitialSampling called");

    // Set up event flags
    const auto AdEvent = AIE_DATA_NUM | AIE_OFERR | AIE_SCERR | AIE_ADERR;

    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        auto result = aio::setAiSamplingClock(AdId[i], 1000.0f)
                          .and_then([&]() { return aio::getAiSamplingClock(AdId[i]); })
                          .and_then([&](float clock) {
                              const auto eventSamplingTimes = board::CalculateSamplingTimes(
                                  std::chrono::microseconds{timer::TimeInterval_1}, clock);
                              return aio::setAiEventSamplingTimes(AdId[i], eventSamplingTimes);
                          })
                          .and_then([&]() { return aio::setAiStopTrigger(AdId[i], 4); })
                          .and_then([&]() { return aio::resetAiMemory(AdId[i]); })
                          .and_then([&]() { return aio::setAiEvent(AdId[i], hWnd, AdEvent); });

        if (!result)
        {
            aio::log_error(std::format("ConfigureInitialSampling A/D board {}", i), result.error());
            return std::unexpected(std::format("Initial sampling config failed for board {}: {}", i, result.error()));
        }
    }

    // Start all boards
    for (size_t i = 0; i < dsb::NUMAD; i++)
    {
        if (auto result = aio::startAi(AdId[i]); !result)
        {
            aio::log_error(std::format("Starting A/D board {}", i), result.error());
            return std::unexpected(std::format("Failed to start A/D board {}: {}", i, result.error()));
        }
    }

    spdlog::info("Initial sampling configured and started successfully");
    return {};
}

// Explicit template instantiation for the specific types used in the application
template std::expected<InitResult, std::string> InitializeBoards<2, 1>(
    std::span<const std::string_view, 2> adDeviceNames, std::span<const std::string_view, 1> daDeviceNames) noexcept;

} // namespace board_control
