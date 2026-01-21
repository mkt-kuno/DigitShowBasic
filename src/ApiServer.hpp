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
 * @file ApiServer.hpp
 * @brief REST API server for streaming sensor data via Server-Sent Events
 *
 * Provides HTTP endpoints for accessing real-time sensor data from the
 * DigitShowDST apparatus. Uses cpp-httplib for HTTP server implementation
 * and supports Server-Sent Events (SSE) for real-time data streaming.
 */

#include "control/measurement.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

// Use lightweight forward declarations and fwd headers to avoid heavy includes
#include <httplib.h>
#include <nlohmann/json_fwd.hpp>

// Forward declare httplib request/response types used in handler signatures
namespace httplib
{
struct Request;
struct Response;
} // namespace httplib

namespace api
{

/**
 * @brief Configuration for the API server
 */
struct ApiConfig
{
    bool enabled = false;
    std::string host = "127.0.0.1";
    int port = 8080;
    int update_interval_ms = 100;
    bool cors_enabled = true;
    int max_connections = 10;
};

/**
 * @brief HTTP REST API server for sensor data streaming
 *
 * Provides endpoints:
 * - GET /api/health - Health check
 * - GET /api/sensor-data - Current sensor data snapshot (JSON)
 * - GET /api/sensor-data/stream - SSE stream of sensor data
 */
class ApiServer
{
  public:
    ApiServer() noexcept;
    ~ApiServer() noexcept;

    // Disable copy and move
    ApiServer(const ApiServer &) = delete;
    ApiServer &operator=(const ApiServer &) = delete;
    ApiServer(ApiServer &&) = delete;
    ApiServer &operator=(ApiServer &&) = delete;

    /**
     * @brief Start the API server
     * @param config Server configuration
     * @return true if server started successfully, false otherwise
     */
    [[nodiscard]] bool start(const ApiConfig &config) noexcept;

    /**
     * @brief Stop the API server
     */
    void stop() noexcept;

    /**
     * @brief Check if the server is running
     * @return true if server is running, false otherwise
     */
    [[nodiscard]] bool is_running() const noexcept;

    /**
     * @brief Update sensor data to be served via API
     * @param input Current physical input measurements
     * @param output Current physical output values
     *
     * Thread-safe. Should be called from Timer 1 (50ms UI refresh).
     */
    void update_sensor_data(const control::PhysicalInput &input, const control::PhysicalOutput<> &output) noexcept;

    /**
     * @brief Load configuration from JSON file
     * @param config_path Path to the configuration file
     * @return ApiConfig object with loaded settings, or default config on error
     */
    [[nodiscard]] static ApiConfig load_config(const std::string &config_path) noexcept;

  private:
    // Server implementation
    std::unique_ptr<httplib::Server> server_;
    std::thread server_thread_;
    std::atomic<bool> running_{false};

    // Current sensor data (thread-safe)
    mutable std::mutex data_mutex_;
    control::PhysicalInput current_input_{};
    control::PhysicalOutput<> current_output_{};
    std::chrono::system_clock::time_point last_update_{};

    // Configuration
    ApiConfig config_;

    // Server endpoint handlers
    void setup_routes() noexcept;
    [[nodiscard]] std::string get_sensor_data_json() const noexcept;
    void handle_health(const httplib::Request &req, httplib::Response &res) const noexcept;
    void handle_sensor_data(const httplib::Request &req, httplib::Response &res) const noexcept;
    void handle_sensor_stream(const httplib::Request &req, httplib::Response &res) noexcept;
    void handle_openapi_yaml(const httplib::Request &req, httplib::Response &res) const noexcept;
    void handle_openapi_json(const httplib::Request &req, httplib::Response &res) const noexcept;

    // OpenAPI conversion helpers
    [[nodiscard]] static std::string yaml_to_json(const std::string_view yaml_content) noexcept;

    // JSON serialization helpers
    [[nodiscard]] static nlohmann::json to_json_object(const control::SpecimenSnapshot &specimen) noexcept;
    [[nodiscard]] static nlohmann::json to_json_object(const control::PhysicalInput &input) noexcept;
    [[nodiscard]] static nlohmann::json to_json_object(const control::PhysicalOutput<> &output) noexcept;
};

} // namespace api
