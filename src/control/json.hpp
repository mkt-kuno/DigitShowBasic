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

#pragma warning(push)
#pragma warning(disable : 4800 4866)

#include "../chrono_alias.hpp"
#include "control.hpp"
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
#include <vector>

// Default sensitivity parameters
static constexpr double kDefaultErrStressKpa = 0.5;
static constexpr double kDefaultTiltErrMm = 0.002;
static constexpr double kDefaultVerticalStressGainVPerKpam2 = 0.5;
static constexpr double kDefaultShearStressKpRpmPerKpa = 0.5;
static constexpr double kDefaultTiltGainVPerMm = 0.5;
static constexpr double kDefaultEpOutputLimitKpa = 1.2;
static constexpr double kDefaultNormalDispErrMm = 0.002;
static constexpr double kDefaultNormalDispGainVPerMm = 17.0;
static constexpr double kDefaultMotorOutputLimitRpm = 3000.0;

namespace control
{

// Parse error categories
enum class ParseErrorKind
{
    KeyMissing,           // Key does not exist (optional fields may ignore, required fields are critical)
    ValueAbsent,          // Key exists but value is absent (e.g., `key: null` or `key:`)
    TypeMismatch,         // Type conversion failed (e.g., "abc" to double)
    OutOfRange,           // Value is out of valid range (e.g., negative motor_rpm)
    InvalidPattern,       // Unknown pattern in `use` field
    MissingRequiredField, // Required field is missing
    SyntaxError,          // YAML/JSON syntax error
    IOError,              // File access error
};

// Detailed parse error information
struct ParseError
{
    ParseErrorKind kind;
    std::string field_path; // e.g., "steps[2].with.motor_rpm"
    std::string message;    // Human-readable description
    std::string source;     // Optional: file path or context

    ParseError(ParseErrorKind k, std::string_view path, std::string_view msg)
        : kind(k), field_path(path), message(msg), source()
    {
    }

    ParseError(ParseErrorKind k, std::string_view path, std::string_view msg, std::string_view src)
        : kind(k), field_path(path), message(msg), source(src)
    {
    }

    [[nodiscard]] std::string format() const
    {
        if (source.empty())
        {
            return std::format("{}: {}", field_path, message);
        }
        return std::format("{} ({}): {}", field_path, source, message);
    }
};

} // namespace control

// File format enumeration for configuration files
enum class FileFormat
{
    JSON,
    YAML
};

/**
 * @brief Detect file format from file extension
 * @param filepath Path to the file
 * @return FileFormat::YAML for .yml/.yaml extensions, FileFormat::JSON otherwise (default)
 */
inline FileFormat DetectFormat(const std::filesystem::path &filepath) noexcept
{
    const auto ext = filepath.extension().string();
    if (ext == ".yml" || ext == ".yaml")
    {
        return FileFormat::YAML;
    }
    return FileFormat::JSON; // Default to JSON for .json or unknown
}

/**
 * @brief Load configuration file (JSON or YAML) and return ryml tree
 * @param filepath Path to the configuration file
 * @return Expected containing ryml::Tree, or ParseError on failure
 *
 * This function automatically detects the file format based on extension and
 * parses the file accordingly. Distinguishes between IOError and SyntaxError.
 */
inline std::expected<ryml::Tree, control::ParseError> LoadConfigFile(const std::filesystem::path &filepath) noexcept
{
    try
    {
        std::ifstream ifs(filepath);
        if (!ifs.is_open())
        {
            return std::unexpected(control::ParseError{control::ParseErrorKind::IOError, filepath.string(),
                                                       std::format("Cannot open file: {}", filepath.string())});
        }

        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();

        const auto format = DetectFormat(filepath);
        ryml::Tree tree;

        try
        {
            if (format == FileFormat::YAML)
            {
                tree = ryml::parse_in_arena(ryml::to_csubstr(content));
            }
            else
            {
                tree = ryml::parse_json_in_arena(ryml::to_csubstr(content));
            }
        }
        catch (const std::exception &e)
        {
            return std::unexpected(control::ParseError{
                control::ParseErrorKind::SyntaxError, filepath.string(),
                std::format("{} syntax error: {}", (format == FileFormat::YAML ? "YAML" : "JSON"), e.what())});
        }

        return tree;
    }
    catch (const std::exception &e)
    {
        return std::unexpected(control::ParseError{control::ParseErrorKind::IOError, filepath.string(),
                                                   std::format("Error loading config file: {}", e.what())});
    }
}

/**
 * @brief Save configuration file in specified format
 * @param filepath Path where the file should be saved
 * @param tree RapidYAML tree containing the data to save
 * @param format Output format (JSON or YAML)
 * @return true on success, false on error
 *
 * This function writes the tree to the specified file in the requested format.
 * Errors are logged via spdlog.
 */
inline bool SaveConfigFile(const std::filesystem::path &filepath, const ryml::Tree &tree,
                           const FileFormat format) noexcept
{
    try
    {
        std::ofstream ofs(filepath);
        if (!ofs.is_open())
        {
            spdlog::error("Failed to open config file for write: {}", filepath.string());
            return false;
        }

        if (format == FileFormat::YAML)
        {
            ofs << tree;
        }
        else
        {
            ofs << ryml::as_json(tree);
        }

        ofs.close();
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error saving config file {}: {}", filepath.string(), e.what());
        return false;
    }
}

/**
 * @brief Helper template function to get value from YAML/JSON node with detailed error reporting
 * @tparam T The type of value to retrieve (std::string, double, int, size_t, etc.)
 * @param node The ryml node to read from
 * @param key The key name to look for
 * @param field_path The full path to this field (e.g., "steps[0].with.motor_rpm") for error reporting
 * @return Expected containing the value, or ParseError if parsing failed
 *
 * This function returns T{} (default value) if the key is missing (optional field behavior).
 * It returns ParseError for value absence (key: null or key:) or type mismatch.
 *
 * Usage examples:
 *   auto result = get<double>(node, "value", "config.value");
 *   if (!result.has_value()) {
 *       spdlog::error("Parse error: {}", result.error().format());
 *       return std::unexpected(result.error());
 *   }
 *   double value = result.value();
 */
template <typename T>
inline std::expected<T, control::ParseError> get(const ryml::ConstNodeRef &node, const char *key,
                                                 std::string_view field_path)
{
    if (!node.has_child(ryml::to_csubstr(key)))
    {
        return std::unexpected(control::ParseError{control::ParseErrorKind::KeyMissing, field_path,
                                                   std::format("Key '{}' is missing", key)});
    }

    auto child = node[ryml::to_csubstr(key)];
    if (!child.has_val())
    {
        return std::unexpected(control::ParseError{control::ParseErrorKind::ValueAbsent, field_path,
                                                   std::format("Value for '{}' is absent (key: null or key:)", key)});
    }

    T result{};
    try
    {
        child >> result;
    }
    catch (const std::exception &e)
    {
        return std::unexpected(control::ParseError{control::ParseErrorKind::TypeMismatch, field_path,
                                                   std::format("Failed to parse '{}': {}", key, e.what())});
    }
    return result;
}

// Convert JSON step with "use" field to ControlParams with error collection
// New format: { "id"?: string, "name"?: string, "use": string, "with"?: { ... pattern props } }
inline std::expected<control::ControlParams, std::vector<control::ParseError>> JsonStepToControlParams(
    const ryml::ConstNodeRef &step, size_t step_index)
{
    using namespace std::chrono;

    std::vector<control::ParseError> errors;
    const std::string step_path = std::format("steps[{}]", step_index);

    // Validate required 'use' field
    if (!step.has_child("use"))
    {
        return std::unexpected(std::vector{control::ParseError{control::ParseErrorKind::MissingRequiredField, step_path,
                                                               "Required field 'use' not found"}});
    }

    auto use_result = get<std::string>(step, "use", std::format("{}.use", step_path));
    if (!use_result)
    {
        return std::unexpected(std::vector{use_result.error()});
    }

    std::string use = *use_result;
    if (use.empty())
    {
        return std::unexpected(std::vector{control::ParseError{
            control::ParseErrorKind::ValueAbsent, std::format("{}.use", step_path), "Field 'use' cannot be empty"}});
    }

    control::ControlParams params{};

    auto it = use_map.find(use);
    if (it == use_map.end())
    {
        return std::unexpected(
            std::vector{control::ParseError{control::ParseErrorKind::InvalidPattern, std::format("{}.use", step_path),
                                            std::format("Unknown pattern: '{}'", use)}});
    }
    params.pattern = it->second;

    // Get the 'with' object for pattern-specific properties
    const bool has_with = step.has_child("with");
    ryml::ConstNodeRef with_node = has_with ? step["with"] : step;
    const std::string with_path = has_with ? std::format("{}.with", step_path) : step_path;

// Helper macro to get required field and collect errors (no default value)
#define GET_REQUIRED_FIELD(var, type, key)                                                                             \
    [[maybe_unused]] auto _ = get<type>(with_node, key, std::format("{}.{}", with_path, key))                          \
                                  .transform([&](const auto &val) {                                                    \
                                      var = val;                                                                       \
                                      return val;                                                                      \
                                  })                                                                                   \
                                  .transform_error([&](const auto &e) {                                                \
                                      errors.push_back(e);                                                             \
                                      return e;                                                                        \
                                  });

// Helper macro to get optional field with default value
#define GET_FIELD_OR_COLLECT(var, type, key, default_val)                                                              \
    var = get<type>(with_node, key, std::format("{}.{}", with_path, key))                                              \
              .transform_error([&](const auto &e) {                                                                    \
                  if (e.kind != control::ParseErrorKind::KeyMissing)                                                   \
                      errors.push_back(e);                                                                             \
                  return e;                                                                                            \
              })                                                                                                       \
              .value_or(default_val);

    // Helper macro to load optional sensitivity parameters with defaults

#define LOAD_SHEAR_STRESS_PARAMETERS()                                                                                 \
    GET_REQUIRED_FIELD(params.shear_stress_kpa.setpoint, double, "target_tau_kPa");                                    \
    GET_FIELD_OR_COLLECT(params.shear_stress_kpa.error, double, "shear_stress_error_kpa", kDefaultErrStressKpa);       \
    GET_FIELD_OR_COLLECT(params.shear_stress_kpa.kp_rpm_per_kpa, double, "shear_stress_kp_rpm_per_kpa",                \
                         kDefaultShearStressKpRpmPerKpa);                                                              \
    GET_FIELD_OR_COLLECT(params.shear_stress_kpa.cv_limit_rpm, double, "motor_output_limit_rpm",                       \
                         kDefaultMotorOutputLimitRpm);

#define LOAD_VERTICAL_STRESS_PARAMETERS()                                                                              \
    GET_REQUIRED_FIELD(params.vertical_stress_kpa.setpoint, double, "target_sigma_kPa");                               \
    GET_FIELD_OR_COLLECT(params.vertical_stress_kpa.error, double, "vertical_stress_error_kpa", kDefaultErrStressKpa); \
    GET_FIELD_OR_COLLECT(params.vertical_stress_kpa.ki, double, "vertical_stress_ki_kpa_per_kpa_m2",                   \
                         kDefaultVerticalStressGainVPerKpam2);                                                         \
    GET_FIELD_OR_COLLECT(params.vertical_stress_kpa.cv_limit_kpa, double, "ep_output_limit_kpa",                       \
                         kDefaultEpOutputLimitKpa);

#define LOAD_NORMAL_DISPLACEMENT_PARAMETERS()                                                                          \
    GET_FIELD_OR_COLLECT(params.normal_displacement_mm.error, double, "normal_displacement_error_mm",                  \
                         kDefaultNormalDispErrMm);                                                                     \
    GET_FIELD_OR_COLLECT(params.normal_displacement_mm.ki_kpa_per_mm, double, "normal_displacement_ki_kpa_per_mm",     \
                         kDefaultNormalDispGainVPerMm);                                                                \
    GET_FIELD_OR_COLLECT(params.normal_displacement_mm.cv_limit_kpa, double, "ep_output_limit_kpa",                    \
                         kDefaultEpOutputLimitKpa);

#define LOAD_TILT_PARAMETERS()                                                                                         \
    GET_FIELD_OR_COLLECT(params.tilt_mm.error, double, "tilt_error_mm", kDefaultTiltErrMm);                            \
    GET_FIELD_OR_COLLECT(params.tilt_mm.ki_kpa_per_mm, double, "tilt_ki_kpa_per_mm", kDefaultTiltGainVPerMm);          \
    GET_FIELD_OR_COLLECT(params.tilt_mm.cv_limit_kpa, double, "ep_output_limit_kpa", kDefaultEpOutputLimitKpa);

    // For patterns that use 'with', read from the 'with' object
    // Direction values: "load"/"unload" for most patterns, "compression"/"dilation" for consolidation patterns
    switch (params.pattern)
    {
    case ControlPattern::NoControl:
        break;

    case ControlPattern::MonotonicLoadingConstantPressure: {
        GET_REQUIRED_FIELD(params.monotonic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.monotonic_loading.target_tau_kpa, double, "target_tau_kPa");
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();

        break;
    }

    case ControlPattern::MonotonicLoadingConstantVolume: {
        GET_REQUIRED_FIELD(params.monotonic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.monotonic_loading.target_tau_kpa, double, "target_tau_kPa");
        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CyclicLoadingConstantPressure: {
        GET_REQUIRED_FIELD(params.cyclic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.cyclic_loading.tau_lower_kpa, double, "tau_lower_kPa");
        GET_REQUIRED_FIELD(params.cyclic_loading.tau_upper_kpa, double, "tau_upper_kPa");
        GET_REQUIRED_FIELD(params.cyclic_loading.cycles, size_t, "num_cycles");
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CyclicLoadingConstantVolume: {
        GET_REQUIRED_FIELD(params.cyclic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.cyclic_loading.tau_lower_kpa, double, "tau_lower_kPa");
        GET_REQUIRED_FIELD(params.cyclic_loading.tau_upper_kpa, double, "tau_upper_kPa");
        GET_REQUIRED_FIELD(params.cyclic_loading.cycles, size_t, "num_cycles");
        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CreepConstantPressure: {
        LOAD_SHEAR_STRESS_PARAMETERS();

        double time_min = 0.0;
        GET_REQUIRED_FIELD(time_min, double, "time_min");
        params.duration = minutes_d{time_min};

        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CreepConstantVolume: {
        LOAD_SHEAR_STRESS_PARAMETERS();

        double time_min = 0.0;
        GET_REQUIRED_FIELD(time_min, double, "time_min");
        params.duration = minutes_d{time_min};

        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::RelaxationConstantPressure: {
        double time_min = 0.0;
        GET_REQUIRED_FIELD(time_min, double, "time_min");
        params.duration = minutes_d{time_min};

        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::RelaxationConstantVolume: {
        double time_min = 0.0;
        GET_REQUIRED_FIELD(time_min, double, "time_min");
        params.duration = minutes_d{time_min};

        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::MonotonicLoadingDisplacementConstantPressure: {
        GET_REQUIRED_FIELD(params.monotonic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.monotonic_loading.target_displacement_mm, double, "target_displacement_mm");
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::MonotonicLoadingDisplacementConstantVolume: {
        GET_REQUIRED_FIELD(params.monotonic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.monotonic_loading.target_displacement_mm, double, "target_displacement_mm");
        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CyclicLoadingDisplacementConstantPressure: {
        GET_REQUIRED_FIELD(params.cyclic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.cyclic_loading.displacement_lower_mm, double, "displacement_lower_mm");
        GET_REQUIRED_FIELD(params.cyclic_loading.displacement_upper_mm, double, "displacement_upper_mm");
        GET_REQUIRED_FIELD(params.cyclic_loading.cycles, size_t, "num_cycles");
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CyclicLoadingDisplacementConstantVolume: {
        GET_REQUIRED_FIELD(params.cyclic_loading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.cyclic_loading.displacement_lower_mm, double, "displacement_lower_mm");
        GET_REQUIRED_FIELD(params.cyclic_loading.displacement_upper_mm, double, "displacement_upper_mm");
        GET_REQUIRED_FIELD(params.cyclic_loading.cycles, size_t, "num_cycles");
        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::AccelerationConstantPressure: {
        GET_REQUIRED_FIELD(params.motor_acceleration.start_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.motor_acceleration.acceleration, double, "acceleration_rate_rpm_per_min");
        GET_REQUIRED_FIELD(params.motor_acceleration.target_rpm, double, "target_rpm");
        GET_REQUIRED_FIELD(params.shear_stress_kpa.setpoint, double, "target_tau_kPa");
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::AccelerationConstantVolume: {
        GET_REQUIRED_FIELD(params.motor_acceleration.start_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.motor_acceleration.acceleration, double, "acceleration_rate_rpm_per_min");
        GET_REQUIRED_FIELD(params.motor_acceleration.target_rpm, double, "target_rpm");
        GET_REQUIRED_FIELD(params.shear_stress_kpa.setpoint, double, "target_tau_kPa");
        LOAD_NORMAL_DISPLACEMENT_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::ConstantTauConsolidation: {
        GET_REQUIRED_FIELD(params.consolidation_rate_kpa_per_min, double, "consolidation_rate_kPa_per_min");
        LOAD_SHEAR_STRESS_PARAMETERS();
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::KConsolidation: {
        GET_REQUIRED_FIELD(params.path_mloading.motor_rpm, double, "motor_rpm");
        GET_REQUIRED_FIELD(params.path_mloading.target_tau, double, "target_tau_kPa");
        GET_REQUIRED_FIELD(params.path_mloading.sigma_start, double, "sigma_start_kPa");
        GET_REQUIRED_FIELD(params.path_mloading.sigma_end, double, "sigma_end_kPa");
        GET_REQUIRED_FIELD(params.path_mloading.k_value, double, "k_value");
        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CreepConstantPressureFast: {
        LOAD_SHEAR_STRESS_PARAMETERS();

        double time_min = 0.0;
        GET_REQUIRED_FIELD(time_min, double, "time_min");
        params.duration = minutes_d{time_min};

        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::CreepConstantPressureFastRef: {
        LOAD_SHEAR_STRESS_PARAMETERS();

        double time_min = 0.0;
        GET_REQUIRED_FIELD(time_min, double, "time_min");
        params.duration = minutes_d{time_min};

        LOAD_VERTICAL_STRESS_PARAMETERS();
        LOAD_TILT_PARAMETERS();
        break;
    }

    case ControlPattern::PreConsolidation: {
        LOAD_SHEAR_STRESS_PARAMETERS();
        break;
    }

    case ControlPattern::BeforeConsolidation:
    case ControlPattern::AfterConsolidation: {
        // No parameters needed for rebase_reference patterns
        break;
    }

    default: {
        return std::unexpected(std::vector{
            control::ParseError{control::ParseErrorKind::InvalidPattern, step_path,
                                std::format("Unhandled pattern enum: {}", static_cast<int>(params.pattern))}});
    }
    }

#undef GET_REQUIRED_FIELD
#undef GET_FIELD_OR_COLLECT
#undef LOAD_SHEAR_STRESS_PARAMETERS
#undef LOAD_VERTICAL_STRESS_PARAMETERS
#undef LOAD_NORMAL_DISPLACEMENT_PARAMETERS
#undef LOAD_TILT_PARAMETERS

    // Load optional name field from top-level step
    // Backwards compatibility: older files used `description` (string) instead of `name`.
    // Prefer `name` if present, otherwise fall back to `description`.
    [[maybe_unused]] auto _ =
        get<std::string>(step, "name", std::format("{}.name", step_path))
            .or_else([&](const auto &) {
                return get<std::string>(step, "description", std::format("{}.description", step_path));
            })
            .transform([&](const auto &name) { params.name = name; });
    // Both are optional, so no error collection needed

    // If any errors were collected, return them
    if (!errors.empty())
    {
        return std::unexpected(errors);
    }

    return params;
}

// Convert ControlParams to ryml tree for config emission
// New format: { "name"?: string, "use": string, "with"?: { ... pattern props } }
inline ryml::Tree ControlParamsToJsonStep(const control::ControlParams &params)
{
    ryml::Tree tree;
    ryml::NodeRef step = tree.rootref();
    step |= ryml::MAP;

    // Helper macro to write optional field only if it differs from default
#define WRITE_OPTIONAL_FIELD(node, key, value, default_value)                                                          \
    if (value != default_value)                                                                                        \
    {                                                                                                                  \
        node[key] << value;                                                                                            \
    }

    // Helper macros for writing optional sensitivity parameters
#define WRITE_SHEAR_STRESS_OPTIONAL_FIELDS()                                                                           \
    WRITE_OPTIONAL_FIELD(with_node, "shear_stress_error_kpa", params.shear_stress_kpa.error, kDefaultErrStressKpa);    \
    WRITE_OPTIONAL_FIELD(with_node, "shear_stress_kp_rpm_per_kpa", params.shear_stress_kpa.kp_rpm_per_kpa,             \
                         kDefaultShearStressKpRpmPerKpa);                                                              \
    WRITE_OPTIONAL_FIELD(with_node, "motor_output_limit_rpm", params.shear_stress_kpa.cv_limit_rpm,                    \
                         kDefaultMotorOutputLimitRpm);

#define WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS()                                                                        \
    WRITE_OPTIONAL_FIELD(with_node, "vertical_stress_error_kpa", params.vertical_stress_kpa.error,                     \
                         kDefaultErrStressKpa);                                                                        \
    WRITE_OPTIONAL_FIELD(with_node, "vertical_stress_ki_kpa_per_kpa_m2", params.vertical_stress_kpa.ki,                \
                         kDefaultVerticalStressGainVPerKpam2);                                                         \
    WRITE_OPTIONAL_FIELD(with_node, "ep_output_limit_kpa", params.vertical_stress_kpa.cv_limit_kpa,                    \
                         kDefaultEpOutputLimitKpa);

#define WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS()                                                                    \
    WRITE_OPTIONAL_FIELD(with_node, "normal_displacement_error_mm", params.normal_displacement_mm.error,               \
                         kDefaultNormalDispErrMm);                                                                     \
    WRITE_OPTIONAL_FIELD(with_node, "normal_displacement_ki_kpa_per_mm", params.normal_displacement_mm.ki_kpa_per_mm,  \
                         kDefaultNormalDispGainVPerMm);                                                                \
    WRITE_OPTIONAL_FIELD(with_node, "ep_output_limit_kpa", params.normal_displacement_mm.cv_limit_kpa,                 \
                         kDefaultEpOutputLimitKpa);

#define WRITE_TILT_OPTIONAL_FIELDS()                                                                                   \
    WRITE_OPTIONAL_FIELD(with_node, "tilt_error_mm", params.tilt_mm.error, kDefaultTiltErrMm);                         \
    WRITE_OPTIONAL_FIELD(with_node, "tilt_ki_kpa_per_mm", params.tilt_mm.ki_kpa_per_mm, kDefaultTiltGainVPerMm);       \
    WRITE_OPTIONAL_FIELD(with_node, "ep_output_limit_kpa", params.tilt_mm.cv_limit_kpa, kDefaultEpOutputLimitKpa);

    // Write 'use' field first
    step["use"] << get_use_label(params.pattern).data();

    // Save name field at top level if not empty
    if (!params.name.empty())
    {
        step["name"] << params.name;
    }

    // Create 'with' object for pattern-specific properties
    ryml::NodeRef with_node = step["with"];
    with_node |= ryml::MAP;

    switch (params.pattern)
    {
    case ControlPattern::NoControl:
        // No 'with' properties for no_control
        break;

    case ControlPattern::MonotonicLoadingConstantPressure:
        with_node["motor_rpm"] << params.monotonic_loading.motor_rpm;
        with_node["target_tau_kPa"] << params.monotonic_loading.target_tau_kpa;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::MonotonicLoadingConstantVolume:
        with_node["motor_rpm"] << params.monotonic_loading.motor_rpm;
        with_node["target_tau_kPa"] << params.monotonic_loading.target_tau_kpa;
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CyclicLoadingConstantPressure:
        with_node["motor_rpm"] << params.cyclic_loading.motor_rpm;
        with_node["tau_lower_kPa"] << params.cyclic_loading.tau_lower_kpa;
        with_node["tau_upper_kPa"] << params.cyclic_loading.tau_upper_kpa;
        with_node["num_cycles"] << params.cyclic_loading.cycles;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CyclicLoadingConstantVolume:
        with_node["motor_rpm"] << params.cyclic_loading.motor_rpm;
        with_node["tau_lower_kPa"] << params.cyclic_loading.tau_lower_kpa;
        with_node["tau_upper_kPa"] << params.cyclic_loading.tau_upper_kpa;
        with_node["num_cycles"] << params.cyclic_loading.cycles;
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CreepConstantPressure:
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["time_min"] << params.duration.count();
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CreepConstantVolume:
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["time_min"] << params.duration.count();
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::RelaxationConstantPressure:
        with_node["time_min"] << params.duration.count();
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::RelaxationConstantVolume:
        with_node["time_min"] << params.duration.count();
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::MonotonicLoadingDisplacementConstantPressure:
        with_node["motor_rpm"] << params.monotonic_loading.motor_rpm;
        with_node["target_displacement_mm"] << params.monotonic_loading.target_displacement_mm;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::MonotonicLoadingDisplacementConstantVolume:
        with_node["motor_rpm"] << params.monotonic_loading.motor_rpm;
        with_node["target_displacement_mm"] << params.monotonic_loading.target_displacement_mm;
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CyclicLoadingDisplacementConstantPressure:
        with_node["motor_rpm"] << params.cyclic_loading.motor_rpm;
        with_node["displacement_lower_mm"] << params.cyclic_loading.displacement_lower_mm;
        with_node["displacement_upper_mm"] << params.cyclic_loading.displacement_upper_mm;
        with_node["num_cycles"] << params.cyclic_loading.cycles;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CyclicLoadingDisplacementConstantVolume:
        with_node["motor_rpm"] << params.cyclic_loading.motor_rpm;
        with_node["displacement_lower_mm"] << params.cyclic_loading.displacement_lower_mm;
        with_node["displacement_upper_mm"] << params.cyclic_loading.displacement_upper_mm;
        with_node["num_cycles"] << params.cyclic_loading.cycles;
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::AccelerationConstantPressure:
        with_node["motor_rpm"] << params.motor_acceleration.start_rpm;
        with_node["acceleration_rate_rpm_per_min"] << params.motor_acceleration.acceleration;
        with_node["target_rpm"] << params.motor_acceleration.target_rpm;
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::AccelerationConstantVolume:
        with_node["motor_rpm"] << params.motor_acceleration.start_rpm;
        with_node["acceleration_rate_rpm_per_min"] << params.motor_acceleration.acceleration;
        with_node["target_rpm"] << params.motor_acceleration.target_rpm;
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::ConstantTauConsolidation:
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["consolidation_rate_kPa_per_min"] << params.consolidation_rate_kpa_per_min;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::KConsolidation:
        with_node["motor_rpm"] << params.path_mloading.motor_rpm;
        with_node["target_tau_kPa"] << params.path_mloading.target_tau;
        with_node["sigma_start_kPa"] << params.path_mloading.sigma_start;
        with_node["sigma_end_kPa"] << params.path_mloading.sigma_end;
        with_node["k_value"] << params.path_mloading.k_value;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CreepConstantPressureFast:
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["time_min"] << params.duration.count();
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::CreepConstantPressureFastRef:
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["time_min"] << params.duration.count();
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        WRITE_TILT_OPTIONAL_FIELDS();
        break;

    case ControlPattern::PreConsolidation:
        with_node["target_tau_kPa"] << params.shear_stress_kpa.setpoint;
        with_node["target_sigma_kPa"] << params.vertical_stress_kpa.setpoint;
        WRITE_SHEAR_STRESS_OPTIONAL_FIELDS();
        WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS();
        break;

    case ControlPattern::BeforeConsolidation:
        // No 'with' properties for rebase_reference
        break;

    case ControlPattern::AfterConsolidation:
        // No 'with' properties for rebase_reference
        break;

    default:
        break;
    }

    // Remove empty 'with' node for patterns that don't need it
    if (params.pattern == ControlPattern::NoControl || params.pattern == ControlPattern::BeforeConsolidation ||
        params.pattern == ControlPattern::AfterConsolidation)
    {
        if (step.has_child("with"))
            step.remove_child("with");
    }

    // Note: 'overrides' property is no longer emitted in the new format

#undef WRITE_OPTIONAL_FIELD
#undef WRITE_SHEAR_STRESS_OPTIONAL_FIELDS
#undef WRITE_VERTICAL_STRESS_OPTIONAL_FIELDS
#undef WRITE_NORMAL_DISPLACEMENT_OPTIONAL_FIELDS
#undef WRITE_TILT_OPTIONAL_FIELDS

    return tree;
}
#pragma warning(pop)