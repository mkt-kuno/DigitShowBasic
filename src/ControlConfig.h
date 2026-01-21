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
 * @file ControlConfig.h
 * @brief YAML-based control configuration structures and parsing
 *
 * Phase 1: Modern C++ configuration loader for soil testing workflows.
 * Provides type-safe configuration loading from YAML files with unit-aware types.
 */

#pragma once

#include <cctype>
#include <chrono>
#include <expected>
#include <optional>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ControlConfig
{

enum class Direction
{
    Compression,
    Extension
};

enum class StrategyType
{
    NoControl,
    PreConsolidation,
    MonotonicLoadingConstantPressure,
    MonotonicLoadingConstantVolume,
    CyclicStressLoadingConstantPressure,
    CyclicStressLoadingConstantVolume,
    CyclicStrainLoadingConstantPressure,
    CyclicStrainLoadingConstantVolume,
    CreepConstantPressure,
    CreepConstantVolume,
    ConstantTauConsolidation,
    StressPathLoadingConstantPressure
};

// Unit-aware value types
struct Pressure
{
    double value_kPa;
    constexpr Pressure() noexcept : value_kPa(0.0)
    {
    }
    constexpr explicit Pressure(double kPa) noexcept : value_kPa(kPa)
    {
    }
    constexpr double kPa() const noexcept
    {
        return value_kPa;
    }
    constexpr double Pa() const noexcept
    {
        return value_kPa * 1000.0;
    }
    constexpr double MPa() const noexcept
    {
        return value_kPa / 1000.0;
    }
};

struct Duration
{
    std::chrono::milliseconds value;
    constexpr Duration() noexcept : value(0)
    {
    }
    constexpr explicit Duration(std::chrono::milliseconds ms) noexcept : value(ms)
    {
    }
    constexpr std::chrono::milliseconds ms() const noexcept
    {
        return value;
    }
    constexpr std::chrono::seconds s() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::seconds>(value);
    }
    constexpr std::chrono::minutes minutes() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::minutes>(value);
    }
};

struct Percentage
{
    double value;
    constexpr Percentage() noexcept : value(0.0)
    {
    }
    constexpr explicit Percentage(double percent) noexcept : value(percent)
    {
    }
    constexpr double percent() const noexcept
    {
        return value;
    }
    constexpr double fraction() const noexcept
    {
        return value / 100.0;
    }
};

struct Distance
{
    double value_mm;
    constexpr Distance() noexcept : value_mm(0.0)
    {
    }
    constexpr explicit Distance(double mm) noexcept : value_mm(mm)
    {
    }
    constexpr double mm() const noexcept
    {
        return value_mm;
    }
    constexpr double cm() const noexcept
    {
        return value_mm / 10.0;
    }
    constexpr double m() const noexcept
    {
        return value_mm / 1000.0;
    }
};

struct PressureRate
{
    double value_kPa_per_min;
    constexpr PressureRate() noexcept : value_kPa_per_min(0.0)
    {
    }
    constexpr explicit PressureRate(double kPa_per_min) noexcept : value_kPa_per_min(kPa_per_min)
    {
    }
    constexpr double kPa_per_min() const noexcept
    {
        return value_kPa_per_min;
    }
    constexpr double kPa_per_s() const noexcept
    {
        return value_kPa_per_min / 60.0;
    }
};

// Sensitivity override parameters (optional per-step calibration overrides)
struct SensitivityOverrides
{
    std::optional<Pressure> err_stress_p_override;
    std::optional<Distance> err_disp_override;
    std::optional<double> amp_override;   // V/(kPa·m²)
    std::optional<double> amp2f_override; // V/mm
    std::optional<double> amp2r_override; // V/mm
    std::optional<double> dmax_override;  // V
    std::optional<Distance> err_disp_cv_override;
    std::optional<double> amp_cv_override; // V/mm
};

// Strategy-specific parameter structures
struct NoControlParams
{
    // No parameters
};

struct PreConsolidationParams
{
    Pressure target_cell_pressure;
    int motor_speed_rpm;
    Pressure axial_error_threshold;
    std::optional<Duration> hold_duration_minutes;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct MonotonicLoadingConstantPressureParams
{
    Direction direction;
    int motor_speed_rpm;
    Pressure target_deviator_stress;
    Pressure target_cell_pressure;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct MonotonicLoadingConstantVolumeParams
{
    Direction direction;
    int motor_speed_rpm;
    Pressure target_deviator_stress;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct CyclicStressLoadingConstantPressureParams
{
    Direction direction;
    int motor_speed_rpm;
    Pressure deviator_stress_min;
    Pressure deviator_stress_max;
    Pressure target_cell_pressure;
    std::optional<double> cycle_count_param1;
    std::optional<double> cycle_count_param2;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct CyclicStressLoadingConstantVolumeParams
{
    Direction direction;
    int motor_speed_rpm;
    Pressure deviator_stress_min;
    Pressure deviator_stress_max;
    std::optional<double> cycle_count_param1;
    std::optional<double> cycle_count_param2;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct CyclicStrainLoadingConstantPressureParams
{
    Direction direction;
    int motor_speed_rpm;
    Percentage axial_strain_min;
    Percentage axial_strain_max;
    Pressure target_cell_pressure;
    std::optional<double> cycle_count_param1;
    std::optional<double> cycle_count_param2;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct CyclicStrainLoadingConstantVolumeParams
{
    Direction direction;
    int motor_speed_rpm;
    Percentage axial_strain_min;
    Percentage axial_strain_max;
    std::optional<double> cycle_count_param1;
    std::optional<double> cycle_count_param2;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct CreepConstantPressureParams
{
    Pressure target_deviator_stress;
    Pressure target_cell_pressure;
    int motor_speed_rpm_max;
    Pressure stress_error_threshold;
    Duration hold_duration_minutes;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct CreepConstantVolumeParams
{
    Pressure target_deviator_stress;
    int motor_speed_rpm_max;
    Pressure stress_error_threshold;
    Duration hold_duration_minutes;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct ConstantTauConsolidationParams
{
    Direction direction;
    int motor_speed_rpm;
    Pressure target_deviator_stress;
    PressureRate consolidation_rate;
    Pressure target_cell_pressure_final;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

struct StressPathLoadingConstantPressureParams
{
    Direction direction;
    int motor_speed_rpm;
    Pressure deviator_stress_start;
    Pressure deviator_stress_end;
    Pressure cell_pressure_start;
    double stress_path_slope_k;
    std::optional<SensitivityOverrides> sensitivity_overrides;
};

// Variant type for strategy parameters
using StrategyParameters =
    std::variant<NoControlParams, PreConsolidationParams, MonotonicLoadingConstantPressureParams,
                 MonotonicLoadingConstantVolumeParams, CyclicStressLoadingConstantPressureParams,
                 CyclicStressLoadingConstantVolumeParams, CyclicStrainLoadingConstantPressureParams,
                 CyclicStrainLoadingConstantVolumeParams, CreepConstantPressureParams, CreepConstantVolumeParams,
                 ConstantTauConsolidationParams, StressPathLoadingConstantPressureParams>;

// Control step instruction
struct ControlStep
{
    std::string id;
    StrategyType uses;       // Renamed from 'strategy' to match YAML syntax
    StrategyParameters with; // Renamed from 'parameters' to match YAML syntax
};

// Guard rule for cross-cutting safety
struct GuardRule
{
    std::string when;                   // Boolean expression (e.g., "abs(q_kpa) > 500")
    std::string action;                 // emergency_stop, pid_stop_all, pause, abort
    std::optional<std::string> message; // Human-readable description
};

// Logging configuration
struct LoggingConfig
{
    std::string basename;
    std::chrono::milliseconds interval_ms{1000}; // Default 1000 ms
};

// Execution settings
struct ExecutionConfig
{
    bool simulation;
    LoggingConfig logging;
};

// Metadata
struct Metadata
{
    std::string name;
    std::string specimen_profile;
    std::string calibration_profile;
    std::optional<std::string> author;
    std::optional<std::string> created;
    std::optional<std::string> description;
};

// Postprocessing settings
struct PostprocessingConfig
{
    bool export_fifo = false;
    std::optional<std::string> notes;
};

// Top-level configuration document
struct ControlDocument
{
    int version;
    Metadata metadata;
    ExecutionConfig execution;
    std::vector<GuardRule> guards; // Cross-cutting safety rules
    std::vector<ControlStep> steps;
    PostprocessingConfig postprocessing;
};

// Error severity levels
enum class ErrorSeverity
{
    Error,  // Blocking error
    Warning // Informational warning
};

// Configuration error with location and diagnostic information
struct ConfigError
{
    int line;
    int column;
    std::string field_path;
    std::string message;
    ErrorSeverity severity;

    constexpr ConfigError(int l, int c, std::string path, std::string msg,
                          ErrorSeverity sev = ErrorSeverity::Error) noexcept
        : line(l), column(c), field_path(std::move(path)), message(std::move(msg)), severity(sev)
    {
    }

    std::string format() const
    {

        std::ostringstream oss;
        oss << (severity == ErrorSeverity::Error ? "Error" : "Warning") << " at line " << line << ", column " << column;
        if (!field_path.empty())
        {
            oss << " (" << field_path << ")";
        }
        oss << ":\n  " << message;
        return oss.str();
    }
};

using ConfigErrorList = std::vector<ConfigError>;

// Strategy metadata for validation and documentation
struct StrategyMetadata
{
    std::string name;
    std::string description;
    std::vector<std::string> required_params;
    std::vector<std::string> optional_params;
};

// Strategy registry (populated at initialization)
class StrategyRegistry
{
  public:
    static const StrategyMetadata &GetMetadata(StrategyType strategy)
    {
        return registry_.at(strategy);
    }
    static bool IsValidStrategy(const std::string &name) noexcept
    {
        return name_to_type_.find(name) != name_to_type_.end();
    }

    static const std::expected<StrategyType, std::string> LookupStrategy(const std::string &name) noexcept
    {
        auto it = name_to_type_.find(name);
        if (it == name_to_type_.end())
        {
            return std::unexpected("Unknown strategy type: " + name);
        }
        return it->second;
    }

  private:
    static inline const std::unordered_map<StrategyType, StrategyMetadata> registry_ = {
        {StrategyType::NoControl, {"no_control", "Motor clutch ON, no rotation; outputs hold previous values", {}, {}}},
        {StrategyType::PreConsolidation,
         {"pre_consolidation",
          "Maintain near-isotropic stress state; proportional motor control",
          {"target_cell_pressure", "motor_speed_rpm", "axial_error_threshold"},
          {"hold_duration_minutes", "sensitivity_overrides"}}},
        {StrategyType::MonotonicLoadingConstantPressure,
         {"monotonic_loading_constant_pressure",
          "Single-direction compression/extension under constant cell pressure",
          {"direction", "motor_speed_rpm", "target_deviator_stress", "target_cell_pressure"},
          {"sensitivity_overrides"}}},
        {StrategyType::MonotonicLoadingConstantVolume,
         {"monotonic_loading_constant_volume",
          "Undrained single-direction compression/extension",
          {"direction", "motor_speed_rpm", "target_deviator_stress"},
          {"sensitivity_overrides"}}},
        {StrategyType::CyclicStressLoadingConstantPressure,
         {"cyclic_stress_loading_constant_pressure",
          "Cyclic loading between stress limits under constant cell pressure",
          {"direction", "motor_speed_rpm", "deviator_stress_min", "deviator_stress_max", "target_cell_pressure"},
          {"cycle_count_param1", "cycle_count_param2", "sensitivity_overrides"}}},
        {StrategyType::CyclicStressLoadingConstantVolume,
         {"cyclic_stress_loading_constant_volume",
          "Undrained cyclic loading between stress limits",
          {"direction", "motor_speed_rpm", "deviator_stress_min", "deviator_stress_max"},
          {"cycle_count_param1", "cycle_count_param2", "sensitivity_overrides"}}},
        {StrategyType::CyclicStrainLoadingConstantPressure,
         {"cyclic_strain_loading_constant_pressure",
          "Cyclic loading between strain limits under constant cell pressure",
          {"direction", "motor_speed_rpm", "axial_strain_min", "axial_strain_max", "target_cell_pressure"},
          {"cycle_count_param1", "cycle_count_param2", "sensitivity_overrides"}}},
        {StrategyType::CyclicStrainLoadingConstantVolume,
         {"cyclic_strain_loading_constant_volume",
          "Undrained cyclic loading between strain limits",
          {"direction", "motor_speed_rpm", "axial_strain_min", "axial_strain_max"},
          {"cycle_count_param1", "cycle_count_param2", "sensitivity_overrides"}}},
        {StrategyType::CreepConstantPressure,
         {"creep_constant_pressure",
          "Maintain constant deviator stress under constant cell pressure; P-control",
          {"target_deviator_stress", "target_cell_pressure", "motor_speed_rpm_max", "stress_error_threshold",
           "hold_duration_minutes"},
          {"sensitivity_overrides"}}},
        {StrategyType::CreepConstantVolume,
         {"creep_constant_volume",
          "Undrained creep test",
          {"target_deviator_stress", "motor_speed_rpm_max", "stress_error_threshold", "hold_duration_minutes"},
          {"sensitivity_overrides"}}},
        {StrategyType::ConstantTauConsolidation,
         {"constant_tau_consolidation",
          "Consolidation along constant-�� path",
          {"direction", "motor_speed_rpm", "target_deviator_stress", "consolidation_rate",
           "target_cell_pressure_final"},
          {"sensitivity_overrides"}}},
        {StrategyType::StressPathLoadingConstantPressure,
         {"stress_path_loading_constant_pressure",
          "Loading along linear stress path in p'-q space",
          {"direction", "motor_speed_rpm", "deviator_stress_start", "deviator_stress_end", "cell_pressure_start",
           "stress_path_slope_k"},
          {"sensitivity_overrides"}}}};

    static inline const std::unordered_map<std::string, StrategyType> name_to_type_ = {
        {"no_control", StrategyType::NoControl},
        {"pre_consolidation", StrategyType::PreConsolidation},
        {"monotonic_loading_constant_pressure", StrategyType::MonotonicLoadingConstantPressure},
        {"monotonic_loading_constant_volume", StrategyType::MonotonicLoadingConstantVolume},
        {"cyclic_stress_loading_constant_pressure", StrategyType::CyclicStressLoadingConstantPressure},
        {"cyclic_stress_loading_constant_volume", StrategyType::CyclicStressLoadingConstantVolume},
        {"cyclic_strain_loading_constant_pressure", StrategyType::CyclicStrainLoadingConstantPressure},
        {"cyclic_strain_loading_constant_volume", StrategyType::CyclicStrainLoadingConstantVolume},
        {"creep_constant_pressure", StrategyType::CreepConstantPressure},
        {"creep_constant_volume", StrategyType::CreepConstantVolume},
        {"constant_tau_consolidation", StrategyType::ConstantTauConsolidation},
        {"stress_path_loading_constant_pressure", StrategyType::StressPathLoadingConstantPressure}};
};

// Configuration loader
class ConfigLoader
{
  public:
    // Load and parse YAML control document from file
    static std::expected<ControlDocument, ConfigErrorList> LoadFromFile(const std::string &path);

    // Parse YAML control document from string (for testing)
    static std::expected<ControlDocument, ConfigErrorList> LoadFromString(const std::string &yaml_content);

  private:
    static const std::expected<Pressure, std::string> ParsePressure(const std::string &value_str)
    {
        std::regex pattern(R"(^\s*([0-9.]+)\s*_?\s*(kPa|kpa|KPA|MPa|mpa|MPA|Pa|pa|PA)?\s*$)", std::regex::icase);
        std::smatch match;

        if (!std::regex_match(value_str, match, pattern))
        {
            return std::unexpected("Invalid pressure format: " + value_str);
        }

        double value = std::stod(match[1].str());
        auto unit = match[2].str() |
                    std::views::transform([](char i) { return static_cast<char>(std::tolower(static_cast<int>(i))); }) |
                    std::ranges::to<std::string>();

        if (unit.empty() || unit == "kpa")
        {
            return Pressure(value);
        }
        else if (unit == "mpa")
        {
            return Pressure(value * 1000.0);
        }
        else if (unit == "pa")
        {
            return Pressure(value / 1000.0);
        }

        return std::unexpected("Unsupported pressure unit: " + unit);
    }
    static std::expected<Duration, std::string> ParseDuration(const std::string &value_str)
    {
        std::regex pattern(R"(^\s*([0-9.]+)\s*_?\s*(ms|msec|s|sec|min|minute|h|hr|hour)?\s*$)", std::regex::icase);
        std::smatch match;

        if (!std::regex_match(value_str, match, pattern))
        {
            return std::unexpected("Invalid duration format: " + value_str);
        }

        double value = std::stod(match[1].str());
        auto unit = match[2].str() |
                    std::views::transform([](char i) { return static_cast<char>(std::tolower(static_cast<int>(i))); }) |
                    std::ranges::to<std::string>();

        if (unit.empty() || unit == "s" || unit == "sec")
        {
            return Duration(std::chrono::milliseconds(static_cast<long long>(value * 1000)));
        }
        else if (unit == "ms" || unit == "msec")
        {
            return Duration(std::chrono::milliseconds(static_cast<long long>(value)));
        }
        else if (unit == "min" || unit == "minute")
        {
            return Duration(std::chrono::milliseconds(static_cast<long long>(value * 60000)));
        }
        else if (unit == "h" || unit == "hr" || unit == "hour")
        {
            return Duration(std::chrono::milliseconds(static_cast<long long>(value * 3600000)));
        }

        return std::unexpected("Unsupported duration unit: " + unit);
    }
    static std::expected<Percentage, std::string> ParsePercentage(const std::string &value_str)
    {
        std::regex pattern(R"(^\s*([0-9.]+)\s*%?\s*$)");
        std::smatch match;

        if (!std::regex_match(value_str, match, pattern))
        {
            return std::unexpected("Invalid percentage format: " + value_str);
        }

        double value = std::stod(match[1].str());
        return Percentage(value);
    }
    static std::expected<Distance, std::string> ParseDistance(const std::string &value_str)
    {
        std::regex pattern(R"(^\s*([0-9.]+)\s*_?\s*(mm|cm|m)?\s*$)", std::regex::icase);
        std::smatch match;

        if (!std::regex_match(value_str, match, pattern))
        {
            return std::unexpected("Invalid distance format: " + value_str);
        }

        double value = std::stod(match[1].str());
        auto unit = match[2].str() |
                    std::views::transform([](char i) { return static_cast<char>(std::tolower(static_cast<int>(i))); }) |
                    std::ranges::to<std::string>();

        if (unit.empty() || unit == "mm")
        {
            return Distance(value);
        }
        else if (unit == "cm")
        {
            return Distance(value * 10.0);
        }
        else if (unit == "m")
        {
            return Distance(value * 1000.0);
        }

        return std::unexpected("Unsupported distance unit: " + unit);
    }
    static std::expected<PressureRate, std::string> ParsePressureRate(const std::string &value_str)
    {
        std::regex pattern(R"(^\s*([0-9.]+)\s*_?\s*(kPa|MPa)?\s*/\s*min\s*$)", std::regex::icase);
        std::smatch match;

        if (!std::regex_match(value_str, match, pattern))
        {
            return std::unexpected("Invalid pressure rate format: " + value_str);
        }

        double value = std::stod(match[1].str());
        auto unit = match[2].str() |
                    std::views::transform([](char i) { return static_cast<char>(std::tolower(static_cast<int>(i))); }) |
                    std::ranges::to<std::string>();

        if (unit.empty() || unit == "kpa")
        {
            return PressureRate(value);
        }
        else if (unit == "mpa")
        {
            return PressureRate(value * 1000.0);
        }

        return std::unexpected("Unsupported pressure rate unit: " + unit);
    }
    static std::expected<Direction, std::string> ParseDirection(const std::string &value_str)
    {
        auto lower =
            value_str |
            std::views::transform([](char i) { return static_cast<char>(std::tolower(static_cast<int>(i))); }) |
            std::ranges::to<std::string>();

        if (lower == "compression")
        {
            return Direction::Compression;
        }
        else if (lower == "extension")
        {
            return Direction::Extension;
        }

        return std::unexpected("Invalid direction: " + value_str + " (expected 'compression' or 'extension')");
    }
    static std::expected<StrategyType, std::string> ParseStrategyType(const std::string &value_str) noexcept
    {
        return StrategyRegistry::LookupStrategy(value_str);
    }
};

} // namespace ControlConfig
