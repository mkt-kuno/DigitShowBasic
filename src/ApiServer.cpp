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
 * @file ApiServer.cpp
 * @brief Implementation of REST API server for sensor data streaming
 */

#include "StdAfx.h"

#include "ApiServer.hpp"
#include "openapi_spec.hpp"
#include "version_info.hpp"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

namespace api
{

ApiServer::ApiServer() noexcept = default;

ApiServer::~ApiServer() noexcept
{
    stop();
}

bool ApiServer::start(const ApiConfig &config) noexcept
{
    if (running_.load())
    {
        spdlog::warn("API server is already running");
        return false;
    }

    if (!config.enabled)
    {
        spdlog::info("API server is disabled in configuration");
        return true; // Not an error - disabled is a valid state
    }

    config_ = config;

    try
    {
        server_ = std::make_unique<httplib::Server>();

        // Set up routes
        setup_routes();

        // Configure server options
        server_->set_read_timeout(5, 0);  // 5 seconds
        server_->set_write_timeout(5, 0); // 5 seconds
        server_->set_keep_alive_max_count(100);

        // Start server in background thread
        running_.store(true);
        server_thread_ = std::thread([this]() {
            spdlog::info("Starting API server on {}:{}", config_.host, config_.port);
            if (!server_->listen(config_.host, config_.port))
            {
                spdlog::error("Failed to start API server on {}:{}", config_.host, config_.port);
                running_.store(false);
            }
        });

        // Give server time to start - configurable wait period
        constexpr int startup_wait_ms = 50; // Short wait to verify startup
        std::this_thread::sleep_for(std::chrono::milliseconds(startup_wait_ms));

        if (running_.load())
        {
            spdlog::info("API server started successfully at http://{}:{}", config_.host, config_.port);
            return true;
        }
        else
        {
            spdlog::error("API server failed to start");
            if (server_thread_.joinable())
            {
                server_thread_.join();
            }
            return false;
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Exception while starting API server: {}", e.what());
        running_.store(false);
        return false;
    }
}

void ApiServer::stop() noexcept
{
    if (!running_.load())
    {
        return;
    }

    spdlog::info("Stopping API server...");
    running_.store(false);

    if (server_)
    {
        server_->stop();
    }

    if (server_thread_.joinable())
    {
        server_thread_.join();
    }

    server_.reset();
    spdlog::info("API server stopped");
}

bool ApiServer::is_running() const noexcept
{
    return running_.load();
}

void ApiServer::update_sensor_data(const control::PhysicalInput &input,
                                   const control::PhysicalOutput<> &output) noexcept
{
    std::lock_guard<std::mutex> lock(data_mutex_);
    current_input_ = input;
    current_output_ = output;
    last_update_ = std::chrono::system_clock::now();
}

void ApiServer::setup_routes() noexcept
{
    if (!server_)
    {
        return;
    }

    // Add CORS headers if enabled
    if (config_.cors_enabled)
    {
        server_->set_pre_routing_handler([](const httplib::Request &, httplib::Response &res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            return httplib::Server::HandlerResponse::Unhandled;
        });
    }

    // Health check endpoint
    server_->Get("/api/health",
                 [this](const httplib::Request &req, httplib::Response &res) { handle_health(req, res); });

    // Current sensor data snapshot
    server_->Get("/api/sensor-data",
                 [this](const httplib::Request &req, httplib::Response &res) { handle_sensor_data(req, res); });

    // SSE stream endpoint
    server_->Get("/api/sensor-data/stream",
                 [this](const httplib::Request &req, httplib::Response &res) { handle_sensor_stream(req, res); });

    // OpenAPI specification endpoints - JSON by default
    server_->Get("/api/openapi",
                 [this](const httplib::Request &req, httplib::Response &res) { handle_openapi_json(req, res); });
    server_->Get("/api/openapi.json",
                 [this](const httplib::Request &req, httplib::Response &res) { handle_openapi_json(req, res); });
    server_->Get("/api/openapi.yml",
                 [this](const httplib::Request &req, httplib::Response &res) { handle_openapi_yaml(req, res); });

    spdlog::debug("API routes configured");
}

void ApiServer::handle_health(const httplib::Request & /*req*/, httplib::Response &res) const noexcept
{
    const auto now = std::chrono::system_clock::now();
    const auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    const json response = {{"status", "ok"}, {"timestamp", timestamp_ms}};

    res.set_content(response.dump(), "application/json");
}

void ApiServer::handle_sensor_data(const httplib::Request & /*req*/, httplib::Response &res) const noexcept
{
    try
    {
        const auto data_json = get_sensor_data_json();
        res.set_content(data_json, "application/json");
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error in handle_sensor_data: {}", e.what());
        res.status = 500;
        res.set_content(R"({"error": "Internal server error"})", "application/json");
    }
}

void ApiServer::handle_sensor_stream(const httplib::Request & /*req*/, httplib::Response &res) noexcept
{
    spdlog::info("New SSE client connected");

    res.set_header("Content-Type", "text/event-stream");
    res.set_header("Cache-Control", "no-cache");
    res.set_header("Connection", "keep-alive");

    // Set up chunked transfer with SSE
    res.set_chunked_content_provider("text/event-stream", [this](const size_t /*offset*/, httplib::DataSink &sink) {
        // Check if we should continue streaming
        if (!running_.load())
        {
            return false;
        }

        try
        {
            // Get current sensor data
            const auto data_json = get_sensor_data_json();

            // Format as SSE event
            const auto sse_message = std::format("event: data\ndata: {}\n\n", data_json);

            // Send to client
            if (!sink.write(sse_message.c_str(), sse_message.size()))
            {
                spdlog::info("SSE client disconnected");
                return false;
            }

            // Wait for next update interval
            // Note: This sleep is acceptable as cpp-httplib runs each SSE connection
            // in its own thread via the chunked content provider. This does not block
            // other connections or the main server thread.
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.update_interval_ms));

            return true; // Continue streaming
        }
        catch (const std::exception &e)
        {
            spdlog::error("Error in SSE stream: {}", e.what());
            return false;
        }
    });
}

void ApiServer::handle_openapi_yaml(const httplib::Request & /*req*/, httplib::Response &res) const noexcept
{
    try
    {
        // Serve the embedded OpenAPI YAML specification
        res.set_content(std::string(kOpenApiYaml), "application/x-yaml; charset=utf-8");
        spdlog::debug("Served OpenAPI YAML specification");
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error serving OpenAPI YAML spec: {}", e.what());
        res.status = 500;
        res.set_content(R"({"error": "Internal server error"})", "application/json");
    }
}

void ApiServer::handle_openapi_json(const httplib::Request & /*req*/, httplib::Response &res) const noexcept
{
    try
    {
        // Convert YAML to JSON and serve
        const auto json_content = yaml_to_json(kOpenApiYaml);
        res.set_content(json_content, "application/json; charset=utf-8");
        spdlog::debug("Served OpenAPI JSON specification");
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error serving OpenAPI JSON spec: {}", e.what());
        res.status = 500;
        res.set_content(R"({"error": "Internal server error"})", "application/json");
    }
}

std::string ApiServer::yaml_to_json(const std::string_view yaml_content) noexcept
{
    try
    {
        // Parse YAML using ryml
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml_content.data()));

        // Convert ryml tree to nlohmann::json
        std::function<json(ryml::ConstNodeRef)> convert_node;
        convert_node = [&convert_node](ryml::ConstNodeRef node) -> json {
            if (node.is_map())
            {
                json obj = json::object();
                for (ryml::ConstNodeRef child : node.children())
                {
                    std::string key(child.key().data(), child.key().size());
                    obj[key] = convert_node(child);
                }
                return obj;
            }
            else if (node.is_seq())
            {
                json arr = json::array();
                for (ryml::ConstNodeRef child : node.children())
                {
                    arr.push_back(convert_node(child));
                }
                return arr;
            }
            else if (node.has_val())
            {
                std::string val(node.val().data(), node.val().size());

                // Try to parse as number
                if (!val.empty() && (std::isdigit(val[0]) || val[0] == '-'))
                {
                    try
                    {
                        if (val.find('.') != std::string::npos)
                        {
                            return std::stod(val);
                        }
                        else
                        {
                            return std::stoll(val);
                        }
                    }
                    catch (...)
                    {
                        // Not a valid number, treat as string
                    }
                }

                // Check for boolean
                if (val == "true")
                    return true;
                if (val == "false")
                    return false;
                if (val == "null")
                    return nullptr;

                // Return as string
                return val;
            }
            return nullptr;
        };

        const auto result = convert_node(tree.rootref());
        return result.dump(2); // Pretty print with 2-space indent
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error converting YAML to JSON: {}", e.what());
        return R"({"error": "Failed to convert OpenAPI specification to JSON"})";
    }
}

std::string ApiServer::get_sensor_data_json() const noexcept
{
    std::lock_guard<std::mutex> lock(data_mutex_);

    const auto timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(last_update_.time_since_epoch()).count();

    json response = {{"timestamp", timestamp_ms},
                     {"physical_input", to_json_object(current_input_)},
                     {"physical_output", to_json_object(current_output_)}};

    return response.dump();
}

nlohmann::json ApiServer::to_json_object(const control::SpecimenSnapshot &specimen) noexcept
{
    return json{{"height_mm", specimen.height_mm()},
                {"area_mm2", specimen.area_mm2()},
                {"volume_mm3", specimen.volume_mm3()},
                {"weight_g", specimen.weight_g()},
                {"box_weight_g", specimen.box_weight_g()}};
}

nlohmann::json ApiServer::to_json_object(const control::PhysicalInput &input) noexcept
{
    return json{{"specimen", to_json_object(input.specimen)},
                {"shear_force_N", input.shear_force_N},
                {"vertical_force_N", input.vertical_force_N},
                {"shear_displacement_mm", input.shear_displacement_mm},
                {"front_vertical_disp_mm", input.front_vertical_disp_mm},
                {"rear_vertical_disp_mm", input.rear_vertical_disp_mm},
                {"front_friction_force_N", input.front_friction_force_N},
                {"rear_friction_force_N", input.rear_friction_force_N},
                {"shear_stress_kpa", input.shear_stress_kpa()},
                {"vertical_stress_kpa", input.vertical_stress_kpa()},
                {"normal_displacement_mm", input.normal_displacement_mm()},
                {"tilt_mm", input.tilt_mm()}};
}

nlohmann::json ApiServer::to_json_object(const control::PhysicalOutput<> &output) noexcept
{
    return json{
        {"front_ep_kpa", output.front_ep_kpa}, {"rear_ep_kpa", output.rear_ep_kpa}, {"motor_rpm", output.motor_rpm}};
}

ApiConfig ApiServer::load_config(const std::string &config_path) noexcept
{
    ApiConfig config;

    try
    {
        const std::filesystem::path path(config_path);

        // Check if the file exists
        if (!std::filesystem::exists(path))
        {
            spdlog::warn("API config file not found: {}. Creating with default values.", config_path);

            // Create default config JSON
            json default_json = {{"$schema", "schemas/api_config.schema.json"},
                                 {"enabled", config.enabled},
                                 {"host", config.host},
                                 {"port", config.port},
                                 {"update_interval_ms", config.update_interval_ms},
                                 {"cors_enabled", config.cors_enabled},
                                 {"max_connections", config.max_connections}};

            // Add version information (git commit hash)
            const auto version = version_info::get_version_string();
            if (!version.empty())
            {
                default_json["version"] = version;
                spdlog::debug("Added version info to API config: {}", version);
            }

            // Write default config to file
            std::ofstream out_file(path);
            if (out_file.is_open())
            {
                out_file << default_json.dump(2) << "\n"; // Pretty print with 2-space indent
                out_file.close();
                spdlog::info("Created default API config file: {}", config_path);
            }
            else
            {
                spdlog::error("Failed to create API config file: {}", config_path);
            }

            return config; // Return defaults
        }

        // File exists, load it
        std::ifstream file(path);
        if (!file.is_open())
        {
            spdlog::error("Failed to open API config file: {}. Using defaults.", config_path);
            return config;
        }

        json j;
        file >> j;

        if (j.contains("enabled"))
            config.enabled = j["enabled"];
        if (j.contains("host"))
            config.host = j["host"];
        if (j.contains("port"))
            config.port = j["port"];
        if (j.contains("update_interval_ms"))
            config.update_interval_ms = j["update_interval_ms"];
        if (j.contains("cors_enabled"))
            config.cors_enabled = j["cors_enabled"];
        if (j.contains("max_connections"))
            config.max_connections = j["max_connections"];

        spdlog::info("Loaded API config from: {}", config_path);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error loading API config from {}: {}. Using defaults.", config_path, e.what());
    }

    return config;
}

} // namespace api
