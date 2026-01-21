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
 * @file board_control.hpp
 * @brief High-level board control layer consolidating AIO operations
 *
 * This layer consolidates scattered aio calls from Doc and View into a unified API.
 * It uses existing aio_wrapper.hpp, aio_error_logger.hpp, and Board.hpp infrastructure.
 * Phase B: FIFO buffer ownership moved into board_control layer.
 */

#pragma once

#include "Board.hpp"
#include <Windows.h>
#include <chrono>
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace board_control
{

/**
 * @brief Board initialization result containing device IDs
 */
struct InitResult
{
    std::vector<short> adIds;
    std::vector<short> daIds;
};

/**
 * @brief Initialize all boards (A/D and D/A)
 * @param adDeviceNames A/D device names (e.g., {"AIO000", "AIO002"})
 * @param daDeviceNames D/A device names (e.g., {"AIO001"})
 * @return Expected with InitResult or error message
 *
 * This consolidates OpenBoard() logic from DigitShowDSTDoc.
 * Sets up board state in board:: namespace (AdId, DaId, AdChannels, etc.)
 */
template <size_t Extent1, size_t Extent2>
[[nodiscard]] std::expected<InitResult, std::string> InitializeBoards(
    std::span<const std::string_view, Extent1> adDeviceNames,
    std::span<const std::string_view, Extent2> daDeviceNames) noexcept;

/**
 * @brief Reset all D/A voltage outputs to 0V
 * @return Expected with void or error message
 *
 * This function sets all DAVout channels to 0.0f and writes them to hardware.
 * Should be called before closing boards to ensure devices stop safely.
 */
[[nodiscard]] std::expected<void, std::string> ResetAllVoltageOutputs() noexcept;

/**
 * @brief Close all initialized boards
 * @return Expected with void or error message
 *
 * This consolidates CloseBoard() logic from DigitShowDSTDoc.
 */
[[nodiscard]] std::expected<void, std::string> CloseBoards() noexcept;

/**
 * @brief Read analog input from all A/D boards and populate Vout array
 * @return Expected with void or error message
 *
 * This consolidates AD_INPUT() logic from DigitShowDSTDoc.
 * Reads from board::AdData and writes to variables::Vout.
 */
[[nodiscard]] std::expected<void, std::string> ReadAnalogInputs() noexcept;

/**
 * @brief Write analog output to all D/A boards
 * @return Expected with void or error message
 *
 * This consolidates DA_OUTPUT() logic from DigitShowDSTDoc.
 * Reads from variables::DAVout and writes to hardware.
 */
[[nodiscard]] std::expected<void, std::string> WriteAnalogOutputs() noexcept;

/**
 * @brief Configure FIFO sampling mode for A/D boards
 * @param hWnd Window handle for event notifications
 * @param samplingClockUs Sampling clock period in microseconds
 * @param samplingTimes Number of samples per event
 * @return Expected with void or error message
 *
 * This consolidates FIFO setup logic from DigitShowDSTView.
 */
[[nodiscard]] std::expected<void, std::string> ConfigureFifoSampling(HWND hWnd, const float samplingClockUs,
                                                                     const unsigned long samplingTimes) noexcept;

/**
 * @brief Get A/D sampling count from all boards
 * @return Expected with minimum sampling count across all boards, or error message
 *
 * This consolidates DefWindowProc AIOM_AIE_DATA_NUM logic.
 */
[[nodiscard]] std::expected<long, std::string> GetMinimumSamplingCount() noexcept;

/**
 * @brief Retrieve sampling data from A/D boards
 * @param samplingTimes Number of samples to retrieve
 * @return Expected with actual samples retrieved, or error message
 *
 * This consolidates AioGetAiSamplingData logic from DefWindowProc.
 * Writes data to board::AdData arrays.
 */
[[nodiscard]] std::expected<long, std::string> GetSamplingData(long samplingTimes) noexcept;

/**
 * @brief Reset A/D memory and restart sampling
 * @return Expected with void or error message
 *
 * This consolidates AIOM_AIE_OFERR recovery logic.
 */
[[nodiscard]] std::expected<void, std::string> ResetAndRestartSampling() noexcept;

/**
 * @brief Configure initial sampling after board initialization
 * @param hWnd Window handle for event notifications
 * @return Expected with void or error message
 *
 * This consolidates the initial sampling setup from OnInitialUpdate.
 */
[[nodiscard]] std::expected<void, std::string> ConfigureInitialSampling(HWND hWnd) noexcept;

} // namespace board_control
