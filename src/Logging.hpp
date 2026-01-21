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

/**
 * @file Logging.hpp
 * @brief spdlog initialization for DigitShowDST
 *
 * Provides initialization function for the global spdlog logger.
 * Use spdlog::* functions directly instead of macros.
 */

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace logging
{
inline constexpr auto LOGROTATE_MAX_SIZE = 2 * 1024 * 1024; // 2 MiB
inline constexpr auto LOGROTATE_MAX_FILES = 128;

/**
 * @brief Initialize spdlog with rotating file and console sinks
 *
 * Sets up logging with:
 * - Console output (stdout with colors)
 * - Rotating log file: logs/digitshowdst.log
 * - Max file size: 10 MiB per file
 * - Max files: 5 rotating files
 * - Default level: debug
 * - Pattern: [timestamp] [level] message
 */
inline void initialize() noexcept
{
    try
    {
        // Create sinks
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/digitshowdst.log", LOGROTATE_MAX_SIZE, LOGROTATE_MAX_FILES);

        // Combine sinks
        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("digitshowdst", sinks.begin(), sinks.end());

        // Set pattern and level
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        // Register as default logger
        spdlog::set_default_logger(logger);

        // #ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
        // #else
        // spdlog::set_level(spdlog::level::info);
        // #endif

        spdlog::flush_every(std::chrono::seconds{1});

        spdlog::info("Logging initialized");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        // If initialization fails, spdlog will use its default logger
        spdlog::error("Log initialization failed: {}", ex.what());
    }
}

} // namespace logging
