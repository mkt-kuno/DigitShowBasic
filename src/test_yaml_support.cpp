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
 * @file test_yaml_support.cpp
 * @brief Tests for YAML read/write support for calibration and control scripts
 *
 * This file contains unit tests using doctest framework to verify:
 * - File format detection
 * - JSON/YAML loading
 * - JSON/YAML saving
 * - Round-trip conversion
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "StdAfx.h"
#include <doctest/doctest.h>

#include "control/json.hpp"
#include <filesystem>
#include <fstream>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>

namespace fs = std::filesystem;

TEST_SUITE("FileFormat Detection")
{
    TEST_CASE("DetectFormat detects JSON from .json extension")
    {
        CHECK(DetectFormat(fs::path("test.json")) == FileFormat::JSON);
    }

    TEST_CASE("DetectFormat detects YAML from .yml extension")
    {
        CHECK(DetectFormat(fs::path("test.yml")) == FileFormat::YAML);
    }

    TEST_CASE("DetectFormat detects YAML from .yaml extension")
    {
        CHECK(DetectFormat(fs::path("test.yaml")) == FileFormat::YAML);
    }

    TEST_CASE("DetectFormat defaults to JSON for unknown extensions")
    {
        CHECK(DetectFormat(fs::path("test.txt")) == FileFormat::JSON);
        CHECK(DetectFormat(fs::path("test")) == FileFormat::JSON);
    }
}

TEST_SUITE("LoadConfigFile")
{
    TEST_CASE("LoadConfigFile parses valid JSON")
    {
        // Create a temporary JSON file
        const auto temp_path = fs::temp_directory_path() / "test_config.json";
        {
            std::ofstream ofs(temp_path);
            ofs << R"({
                "calibration_data": [
                    {"channel": 0, "cal_a": 0.0, "cal_b": 1.0, "cal_c": 0.0}
                ],
                "initial_specimen": {
                    "height_mm": 120.0,
                    "area_mm2": 14400.0,
                    "weight_g": 0.0,
                    "box_weight_g": 10000.0
                }
            })";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        CHECK(root.has_child("calibration_data"));
        CHECK(root.has_child("initial_specimen"));

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile parses valid YAML")
    {
        // Create a temporary YAML file
        const auto temp_path = fs::temp_directory_path() / "test_config.yaml";
        {
            std::ofstream ofs(temp_path);
            ofs << R"(
calibration_data:
  - channel: 0
    cal_a: 0.0
    cal_b: 1.0
    cal_c: 0.0
initial_specimen:
  height_mm: 120.0
  area_mm2: 14400.0
  weight_g: 0.0
  box_weight_g: 10000.0
)";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        CHECK(root.has_child("calibration_data"));
        CHECK(root.has_child("initial_specimen"));

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile returns ParseError for non-existent file")
    {
        const auto result = LoadConfigFile(fs::path("nonexistent_file.json"));
        REQUIRE_FALSE(result.has_value());
        CHECK(result.error().kind == control::ParseErrorKind::IOError);
    }

    TEST_CASE("LoadConfigFile returns SyntaxError for invalid JSON")
    {
        const auto temp_path = fs::temp_directory_path() / "invalid.json";
        {
            std::ofstream ofs(temp_path);
            ofs << R"({ "key": invalid_value })"; // Invalid JSON
        }

        const auto result = LoadConfigFile(temp_path);
        REQUIRE_FALSE(result.has_value());
        CHECK(result.error().kind == control::ParseErrorKind::SyntaxError);

        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile returns SyntaxError for invalid YAML")
    {
        const auto temp_path = fs::temp_directory_path() / "invalid.yaml";
        {
            std::ofstream ofs(temp_path);
            ofs << "key: [unclosed"; // Invalid YAML
        }

        const auto result = LoadConfigFile(temp_path);
        REQUIRE_FALSE(result.has_value());
        CHECK(result.error().kind == control::ParseErrorKind::SyntaxError);

        fs::remove(temp_path);
    }
}

TEST_SUITE("SaveConfigFile")
{
    TEST_CASE("SaveConfigFile writes valid JSON")
    {
        const auto temp_path = fs::temp_directory_path() / "test_save.json";

        // Create a simple tree
        ryml::Tree tree;
        auto root = tree.rootref();
        root |= ryml::MAP;
        root["test_key"] << "test_value";
        root["test_number"] << 42;

        const auto success = SaveConfigFile(temp_path, tree, FileFormat::JSON);
        REQUIRE(success);
        CHECK(fs::exists(temp_path));

        // Verify content
        std::ifstream ifs(temp_path);
        std::string content(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
        CHECK(content.find("test_key") != std::string::npos);
        CHECK(content.find("test_value") != std::string::npos);

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("SaveConfigFile writes valid YAML")
    {
        const auto temp_path = fs::temp_directory_path() / "test_save.yaml";

        // Create a simple tree
        ryml::Tree tree;
        auto root = tree.rootref();
        root |= ryml::MAP;
        root["test_key"] << "test_value";
        root["test_number"] << 42;

        const auto success = SaveConfigFile(temp_path, tree, FileFormat::YAML);
        REQUIRE(success);
        CHECK(fs::exists(temp_path));

        // Verify content
        std::ifstream ifs(temp_path);
        std::string content(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
        CHECK(content.find("test_key") != std::string::npos);
        CHECK(content.find("test_value") != std::string::npos);

        // Clean up
        fs::remove(temp_path);
    }
}

TEST_SUITE("Round-trip Conversion")
{
    TEST_CASE("JSON to YAML round-trip preserves data")
    {
        const auto json_path = fs::temp_directory_path() / "roundtrip.json";
        const auto yaml_path = fs::temp_directory_path() / "roundtrip.yaml";

        // Create original JSON
        {
            std::ofstream ofs(json_path);
            ofs << R"({
                "calibration_data": [
                    {"channel": 0, "cal_a": 1.5, "cal_b": 2.0, "cal_c": 3.5}
                ],
                "initial_specimen": {
                    "height_mm": 120.0,
                    "area_mm2": 14400.0,
                    "weight_g": 500.0,
                    "box_weight_g": 10000.0
                }
            })";
        }

        // Load JSON
        auto tree_opt = LoadConfigFile(json_path);
        REQUIRE(tree_opt.has_value());
        auto tree = tree_opt.value();

        // Save as YAML
        const auto save_success = SaveConfigFile(yaml_path, tree, FileFormat::YAML);
        REQUIRE(save_success);

        // Load YAML back
        auto yaml_tree_opt = LoadConfigFile(yaml_path);
        REQUIRE(yaml_tree_opt.has_value());
        const auto yaml_tree = yaml_tree_opt.value();

        // Verify data preservation
        const auto root = yaml_tree.rootref();
        CHECK(root.has_child("calibration_data"));
        CHECK(root.has_child("initial_specimen"));

        const auto channels = root["calibration_data"];
        CHECK(channels.is_seq());
        CHECK(channels.num_children() == 1);

        const auto ch0 = channels[0];
        int channel = -1;
        double cal_a = 0.0, cal_b = 0.0, cal_c = 0.0;
        ch0["channel"] >> channel;
        ch0["cal_a"] >> cal_a;
        ch0["cal_b"] >> cal_b;
        ch0["cal_c"] >> cal_c;

        CHECK(channel == 0);
        CHECK(cal_a == doctest::Approx(1.5));
        CHECK(cal_b == doctest::Approx(2.0));
        CHECK(cal_c == doctest::Approx(3.5));

        // Clean up
        fs::remove(json_path);
        fs::remove(yaml_path);
    }

    TEST_CASE("YAML to JSON round-trip preserves data")
    {
        const auto yaml_path = fs::temp_directory_path() / "roundtrip2.yaml";
        const auto json_path = fs::temp_directory_path() / "roundtrip2.json";

        // Create original YAML
        {
            std::ofstream ofs(yaml_path);
            ofs << R"(
calibration_data:
  - channel: 1
    cal_a: 2.5
    cal_b: 3.0
    cal_c: 4.5
initial_specimen:
  height_mm: 130.0
  area_mm2: 15000.0
  weight_g: 600.0
  box_weight_g: 11000.0
)";
        }

        // Load YAML
        auto tree_opt = LoadConfigFile(yaml_path);
        REQUIRE(tree_opt.has_value());
        auto tree = tree_opt.value();

        // Save as JSON
        const auto save_success = SaveConfigFile(json_path, tree, FileFormat::JSON);
        REQUIRE(save_success);

        // Load JSON back
        auto json_tree_opt = LoadConfigFile(json_path);
        REQUIRE(json_tree_opt.has_value());
        const auto json_tree = json_tree_opt.value();

        // Verify data preservation
        const auto root = json_tree.rootref();
        CHECK(root.has_child("calibration_data"));
        CHECK(root.has_child("initial_specimen"));

        const auto channels = root["calibration_data"];
        CHECK(channels.is_seq());
        CHECK(channels.num_children() == 1);

        const auto ch0 = channels[0];
        int channel = -1;
        double cal_a = 0.0, cal_b = 0.0, cal_c = 0.0;
        ch0["channel"] >> channel;
        ch0["cal_a"] >> cal_a;
        ch0["cal_b"] >> cal_b;
        ch0["cal_c"] >> cal_c;

        CHECK(channel == 1);
        CHECK(cal_a == doctest::Approx(2.5));
        CHECK(cal_b == doctest::Approx(3.0));
        CHECK(cal_c == doctest::Approx(4.5));

        // Clean up
        fs::remove(yaml_path);
        fs::remove(json_path);
    }
}

TEST_SUITE("Control Script Format")
{
    TEST_CASE("LoadConfigFile parses control script JSON")
    {
        const auto temp_path = fs::temp_directory_path() / "test_control.json";
        {
            std::ofstream ofs(temp_path);
            ofs << R"({
                "steps": [
                    {
                        "use": "monotonic_loading_constant_pressure",
                        "with": {
                            "direction": "load",
                            "motor_rpm": 50.0,
                            "tau_kPa": 100.0,
                            "sigma_kPa": 60.0
                        }
                    }
                ]
            })";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        CHECK(root.has_child("steps"));
        CHECK(root["steps"].is_seq());
        CHECK(root["steps"].num_children() == 1);

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile parses control script YAML")
    {
        const auto temp_path = fs::temp_directory_path() / "test_control.yaml";
        {
            std::ofstream ofs(temp_path);
            ofs << R"(
steps:
  - use: monotonic_loading_constant_pressure
    with:
      direction: load
      motor_rpm: 50.0
      tau_kPa: 100.0
      sigma_kPa: 60.0
)";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        CHECK(root.has_child("steps"));
        CHECK(root["steps"].is_seq());
        CHECK(root["steps"].num_children() == 1);

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile parses control script with version field")
    {
        // Create a control script JSON file with version field
        const auto temp_path = fs::temp_directory_path() / "test_control_with_version.json";
        {
            std::ofstream ofs(temp_path);
            ofs << R"({
                "$schema": "../schemas/control_script.schema.json",
                "version": "1234567890abcdef",
                "steps": [
                    {
                        "name": "Test step",
                        "use": "no_control"
                    }
                ]
            })";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        // Check that version field is present and readable
        CHECK(root.has_child("version"));
        if (root.has_child("version"))
        {
            std::string version;
            root["version"] >> version;
            CHECK(version == "1234567890abcdef");
        }

        // Check that steps are still properly loaded
        CHECK(root.has_child("steps"));
        CHECK(root["steps"].is_seq());
        CHECK(root["steps"].num_children() == 1);

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile parses calibration file with version field")
    {
        // Create a calibration JSON file with version field
        const auto temp_path = fs::temp_directory_path() / "test_calibration_with_version.json";
        {
            std::ofstream ofs(temp_path);
            ofs << R"({
                "version": "1234567890abcdef",
                "calibration_data": [
                    {"channel": 0, "cal_a": 0.0, "cal_b": 1.0, "cal_c": 0.0}
                ],
                "initial_specimen": {
                    "height_mm": 120.0,
                    "area_mm2": 14400.0,
                    "weight_g": 0.0,
                    "box_weight_g": 10000.0
                }
            })";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        // Check that version field is present and readable
        CHECK(root.has_child("version"));
        if (root.has_child("version"))
        {
            std::string version;
            root["version"] >> version;
            CHECK(version == "1234567890abcdef");
        }

        // Check that calibration data is still properly loaded
        CHECK(root.has_child("calibration_data"));
        CHECK(root.has_child("initial_specimen"));

        // Clean up
        fs::remove(temp_path);
    }

    TEST_CASE("LoadConfigFile handles files without version field (backward compatibility)")
    {
        // Create a control script JSON file WITHOUT version field (old format)
        const auto temp_path = fs::temp_directory_path() / "test_control_no_version.json";
        {
            std::ofstream ofs(temp_path);
            ofs << R"({
                "$schema": "../schemas/control_script.schema.json",
                "steps": [
                    {
                        "name": "Test step",
                        "use": "no_control"
                    }
                ]
            })";
        }

        const auto tree_opt = LoadConfigFile(temp_path);
        REQUIRE(tree_opt.has_value());

        const auto &tree = tree_opt.value();
        const auto root = tree.rootref();

        // Version field should not be present in old files
        CHECK(!root.has_child("version"));

        // Steps should still load correctly
        CHECK(root.has_child("steps"));
        CHECK(root["steps"].is_seq());
        CHECK(root["steps"].num_children() == 1);

        // Clean up
        fs::remove(temp_path);
    }
}

TEST_SUITE("JsonStepToControlParams error handling")
{
    TEST_CASE("JsonStepToControlParams returns error for missing 'use' field")
    {
        const std::string yaml = R"(
name: "Test step"
with:
  motor_rpm: 1.0
)";
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml));
        const auto result = JsonStepToControlParams(tree.rootref(), 0);

        REQUIRE_FALSE(result.has_value());
        const auto &errors = result.error();
        CHECK(errors.size() == 1);
        CHECK(errors[0].kind == control::ParseErrorKind::MissingRequiredField);
    }

    TEST_CASE("JsonStepToControlParams returns error for invalid pattern")
    {
        const std::string yaml = R"(
use: "invalid_pattern"
)";
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml));
        const auto result = JsonStepToControlParams(tree.rootref(), 0);

        REQUIRE_FALSE(result.has_value());
        const auto &errors = result.error();
        CHECK(errors.size() == 1);
        CHECK(errors[0].kind == control::ParseErrorKind::InvalidPattern);
    }

    TEST_CASE("JsonStepToControlParams collects type mismatch errors")
    {
        const std::string yaml = R"(
use: "monotonic_loading_constant_pressure"
with:
  motor_rpm: "not_a_number"
  tau_kPa: 100
  sigma_kPa: 200
)";
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml));
        const auto result = JsonStepToControlParams(tree.rootref(), 0);

        REQUIRE_FALSE(result.has_value());
        const auto &errors = result.error();
        CHECK(errors.size() >= 1);
        // Check that at least one error is TypeMismatch
        bool has_type_mismatch = false;
        for (const auto &err : errors)
        {
            if (err.kind == control::ParseErrorKind::TypeMismatch)
            {
                has_type_mismatch = true;
                break;
            }
        }
        CHECK(has_type_mismatch);
    }

    TEST_CASE("JsonStepToControlParams collects value absent errors")
    {
        const std::string yaml = R"(
use: "monotonic_loading_constant_pressure"
with:
  motor_rpm:
  tau_kPa: 100
  sigma_kPa: 200
)";
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml));
        const auto result = JsonStepToControlParams(tree.rootref(), 0);

        REQUIRE_FALSE(result.has_value());
        const auto &errors = result.error();
        CHECK(errors.size() >= 1);
        // Check that at least one error is ValueAbsent
        bool has_value_absent = false;
        for (const auto &err : errors)
        {
            if (err.kind == control::ParseErrorKind::ValueAbsent)
            {
                has_value_absent = true;
                break;
            }
        }
        CHECK(has_value_absent);
    }

    TEST_CASE("JsonStepToControlParams succeeds with valid input")
    {
        const std::string yaml = R"(
use: "monotonic_loading_constant_pressure"
name: "Test loading step"
with:
  motor_rpm: 1.5
  tau_kPa: 100
  sigma_kPa: 200
  direction: "load"
)";
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml));
        const auto result = JsonStepToControlParams(tree.rootref(), 0);

        REQUIRE(result.has_value());
        const auto &params = result.value();
        CHECK(params.pattern == ControlPattern::MonotonicLoadingConstantPressure);
        CHECK(params.motor_rpm == doctest::Approx(1.5));
        CHECK(params.target_tau_kpa == doctest::Approx(100.0));
        CHECK(params.target_sigma_kpa == doctest::Approx(200.0));
        CHECK(params.loading_dir == true);
        CHECK(params.name == "Test loading step");
    }
}
