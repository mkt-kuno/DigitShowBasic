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
 * @file ControlConfig.cpp
 * @brief Implementation of YAML-based control configuration parser
 *
 * Phase 1: Configuration loading with rigorous validation.
 * Provides type-safe parsing of YAML control workflow files.
 */
#include "StdAfx.h"
#pragma warning(push)
#pragma warning(disable : 4800 4866)

#include "ControlConfig.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <regex>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

// YAML parser integration notes:
// - Prefer RapidYAML (ryml) single-header if available (ENABLE_RYML_SINGLE_HDR)
// - yaml-cpp path kept as fallback plan but currently not enabled

namespace ControlConfig
{

// Format error message for display
static std::string ReadAllText(const std::string &path)
{
    std::ifstream ifs{path, std::ios::binary};
    if (!ifs)
    {
        return {};
    }
    std::string s((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return s;
}

std::expected<ControlDocument, ConfigErrorList> ConfigLoader::LoadFromFile(const std::string &path)
{
    ConfigErrorList errors;
    std::string content = ReadAllText(path);
    if (content.empty())
    {
        errors.emplace_back(0, 0, "", "Failed to read YAML file: " + path);
        return std::unexpected(std::move(errors));
    }
    return LoadFromString(content);
}

std::expected<ControlDocument, ConfigErrorList> ConfigLoader::LoadFromString(const std::string &yaml_content)
{
    ConfigErrorList errors;
    try
    {
        // Parse into arena to keep stable views
        ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml_content));
        ryml::ConstNodeRef root = tree.rootref();
        if (!root.is_map())
        {
            errors.emplace_back(1, 1, "/", "Top-level YAML must be a mapping (object)");
            return std::unexpected(std::move(errors));
        }

        // Helper lambdas
        auto missing = [&](const char *path, const char *name) {
            errors.emplace_back(1, 1, std::string(path) + "/" + name, std::string("Missing required field: ") + name);
        };

        auto get_map = [&](ryml::ConstNodeRef parent, const char *key, const char *path,
                           ryml::ConstNodeRef *out) -> bool {
            if (!parent.has_child(key))
            {
                missing(path, key);
                return false;
            }
            auto n = parent[key];
            if (!n.is_map())
            {
                errors.emplace_back(1, 1, std::string(path) + "/" + key, "Expected mapping (object)");
                return false;
            }
            *out = n;
            return true;
        };

        auto get_seq = [&](ryml::ConstNodeRef parent, const char *key, const char *path,
                           ryml::ConstNodeRef *out) -> bool {
            if (!parent.has_child(key))
            {
                missing(path, key);
                return false;
            }
            auto n = parent[key];
            if (!n.is_seq())
            {
                errors.emplace_back(1, 1, std::string(path) + "/" + key, "Expected sequence (array)");
                return false;
            }
            *out = n;
            return true;
        };

        auto get_str = [&](ryml::ConstNodeRef node, const char *key, const char *path, std::string *out) -> bool {
            if (!node.has_child(key))
            {
                missing(path, key);
                return false;
            }
            auto n = node[key];
            if (!n.has_val())
            {
                errors.emplace_back(1, 1, std::string(path) + "/" + key, "Expected scalar value");
                return false;
            }
            n >> *out;
            return true;
        };

        auto get_bool = [&](ryml::ConstNodeRef node, const char *key, const char *path, bool *out) -> bool {
            if (!node.has_child(key))
            {
                missing(path, key);
                return false;
            }
            auto n = node[key];
            if (!n.has_val())
            {
                errors.emplace_back(1, 1, std::string(path) + "/" + key, "Expected boolean value");
                return false;
            }
            n >> *out;
            return true;
        };

        auto get_opt_int = [&](ryml::ConstNodeRef node, const char *key, int *out) -> bool {
            if (!node.has_child(key))
                return false;
            auto n = node[key];
            if (!n.has_val())
                return false;
            n >> *out;
            return true;
        };

        auto get_opt_str = [&](ryml::ConstNodeRef node, const char *key, std::string *out) -> bool {
            if (!node.has_child(key))
                return false;
            auto n = node[key];
            if (!n.has_val())
                return false;
            n >> *out;
            return true;
        };

        ControlDocument doc{};
        // version
        if (root.has_child("version"))
        {
            int v = 0;
            root["version"] >> v;
            doc.version = v;
            if (v != 1)
            {
                errors.emplace_back(1, 1, "/version", "Unsupported version: " + std::to_string(v));
            }
        }
        else
        {
            errors.emplace_back(1, 1, "/version", "Missing required field: version");
        }

        // metadata
        ryml::ConstNodeRef md;
        if (get_map(root, "metadata", "/", &md))
        {
            std::string name, spec_profile, calib_profile;
            get_str(md, "name", "/metadata", &name);
            get_str(md, "specimen_profile", "/metadata", &spec_profile);
            get_str(md, "calibration_profile", "/metadata", &calib_profile);
            doc.metadata.name = std::move(name);
            doc.metadata.specimen_profile = std::move(spec_profile);
            doc.metadata.calibration_profile = std::move(calib_profile);
            std::string author, created, description;
            if (get_opt_str(md, "author", &author))
                doc.metadata.author = author;
            if (get_opt_str(md, "created", &created))
                doc.metadata.created = created;
            if (get_opt_str(md, "description", &description))
                doc.metadata.description = description;
        }

        // execution
        ryml::ConstNodeRef exec;
        if (get_map(root, "execution", "/", &exec))
        {
            bool sim = false;
            get_bool(exec, "simulation", "/execution", &sim);
            doc.execution.simulation = sim;

            ryml::ConstNodeRef log;
            if (get_map(exec, "logging", "/execution", &log))
            {
                std::string basename;
                get_str(log, "basename", "/execution/logging", &basename);
                doc.execution.logging.basename = std::move(basename);
                int interval_ms = 1000; // NOLINT(cppcoreguidelines-avoid-magic-numbers)
                if (get_opt_int(log, "interval_ms", &interval_ms))
                {
                    if (interval_ms <= 0)
                    {
                        errors.emplace_back(1, 1, "/execution/logging/interval_ms", "interval_ms must be positive");
                    }
                    else
                    {
                        doc.execution.logging.interval_ms = std::chrono::milliseconds(interval_ms);
                    }
                }
            }
        }

        // guards (optional)
        if (root.has_child("guards"))
        {
            auto guards_node = root["guards"];
            if (guards_node.is_seq())
            {
                for (auto const &g : guards_node.children())
                {
                    if (!g.is_map())
                    {
                        errors.emplace_back(1, 1, "/guards[]", "Each guard must be a mapping");
                        continue;
                    }
                    GuardRule guard{};
                    std::string when_expr, action_str;
                    if (g.has_child("when") && g["when"].has_val())
                    {
                        g["when"] >> when_expr;
                        guard.when = when_expr;
                    }
                    else
                    {
                        errors.emplace_back(1, 1, "/guards[]", "Missing required field: when");
                    }
                    if (g.has_child("action") && g["action"].has_val())
                    {
                        g["action"] >> action_str;
                        guard.action = action_str;
                        // Validate action is one of: emergency_stop, pid_stop_all, pause, abort
                        if (action_str != "emergency_stop" && action_str != "pid_stop_all" && action_str != "pause" &&
                            action_str != "abort")
                        {
                            errors.emplace_back(1, 1, "/guards[]/action",
                                                "Invalid action: " + action_str +
                                                    " (expected emergency_stop, pid_stop_all, pause, or abort)");
                        }
                    }
                    else
                    {
                        errors.emplace_back(1, 1, "/guards[]", "Missing required field: action");
                    }
                    if (g.has_child("message") && g["message"].has_val())
                    {
                        std::string msg;
                        g["message"] >> msg;
                        guard.message = msg;
                    }
                    doc.guards.push_back(std::move(guard));
                }
            }
            else
            {
                errors.emplace_back(1, 1, "/guards", "Expected sequence (array)");
            }
        }

        // steps
        ryml::ConstNodeRef steps;
        if (get_seq(root, "steps", "/", &steps))
        {
            std::unordered_set<std::string> ids;
            for (auto const &stepNode : steps.children())
            {
                if (!stepNode.is_map())
                {
                    errors.emplace_back(1, 1, "/steps[]", "Each step must be a mapping");
                    continue;
                }
                ControlStep step{};
                std::string id;
                get_str(stepNode, "id", "/steps[]", &id);
                if (!id.empty())
                {
                    if (!ids.insert(id).second)
                    {
                        errors.emplace_back(1, 1, "/steps/" + id, "Duplicate step id: " + id);
                    }
                }
                step.id = id;

                std::string uses_name;
                get_str(stepNode, "uses", "/steps[]", &uses_name);
                auto st = StrategyRegistry::LookupStrategy(uses_name);
                if (!st.has_value())
                {
                    errors.emplace_back(1, 1, "/steps/" + id + "/uses", st.error());
                    step.uses = StrategyType::NoControl;
                }
                else
                {
                    step.uses = st.value();
                }

                // parameters (renamed to 'with' in YAML)
                StrategyParameters params = NoControlParams{};
                if (stepNode.has_child("with") && stepNode["with"].is_map())
                {
                    auto p = stepNode["with"];
                    switch (step.uses)
                    {
                    case StrategyType::PreConsolidation: {
                        PreConsolidationParams s{};
                        std::string v;
                        if (p.has_child("target_cell_pressure"))
                        {
                            p["target_cell_pressure"] >> v;
                            auto pr = ParsePressure(v);
                            if (pr)
                                s.target_cell_pressure = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_cell_pressure", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_cell_pressure");
                        if (p.has_child("motor_speed_rpm"))
                        {
                            p["motor_speed_rpm"] >> s.motor_speed_rpm;
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "motor_speed_rpm");
                        if (p.has_child("axial_error_threshold"))
                        {
                            p["axial_error_threshold"] >> v;
                            auto pr = ParsePressure(v);
                            if (pr)
                                s.axial_error_threshold = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/axial_error_threshold", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "axial_error_threshold");
                        if (p.has_child("hold_duration_minutes"))
                        {
                            p["hold_duration_minutes"] >> v;
                            auto d = ParseDuration(v);
                            if (d)
                                s.hold_duration_minutes = d.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/hold_duration_minutes", d.error());
                        }
                        if (p.has_child("sensitivity_overrides") && p["sensitivity_overrides"].is_map())
                        {
                            ryml::ConstNodeRef so = p["sensitivity_overrides"];
                            SensitivityOverrides sox{};
                            bool used = false;
                            std::string sv;
                            if (so.has_child("err_disp_override"))
                            {
                                so["err_disp_override"] >> sv;
                                auto dv = ParseDistance(sv);
                                if (dv)
                                {
                                    sox.err_disp_override = dv.value();
                                    used = true;
                                }
                                else
                                    errors.emplace_back(
                                        1, 1, "/steps/" + id + "/with/sensitivity_overrides/err_disp_override",
                                        dv.error());
                            }
                            if (used)
                                s.sensitivity_overrides = sox;
                        }
                        params = s;
                    }
                    break;
                    case StrategyType::ConstantTauConsolidation: {
                        ConstantTauConsolidationParams s{};
                        std::string sv;
                        if (p.has_child("direction"))
                        {
                            p["direction"] >> sv;
                            auto d = ParseDirection(sv);
                            if (d)
                                s.direction = d.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/direction", d.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "direction");
                        if (p.has_child("motor_speed_rpm"))
                        {
                            p["motor_speed_rpm"] >> s.motor_speed_rpm;
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "motor_speed_rpm");
                        if (p.has_child("target_deviator_stress"))
                        {
                            p["target_deviator_stress"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.target_deviator_stress = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_deviator_stress", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_deviator_stress");
                        if (p.has_child("consolidation_rate"))
                        {
                            p["consolidation_rate"] >> sv;
                            auto rr = ParsePressureRate(sv);
                            if (rr)
                                s.consolidation_rate = rr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/consolidation_rate", rr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "consolidation_rate");
                        if (p.has_child("target_cell_pressure_final"))
                        {
                            p["target_cell_pressure_final"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.target_cell_pressure_final = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_cell_pressure_final",
                                                    pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_cell_pressure_final");
                        if (p.has_child("sensitivity_overrides") && p["sensitivity_overrides"].is_map())
                        {
                            ryml::ConstNodeRef so = p["sensitivity_overrides"];
                            SensitivityOverrides sox{};
                            bool used = false;
                            std::string dv;
                            if (so.has_child("err_disp_override"))
                            {
                                so["err_disp_override"] >> dv;
                                auto dd = ParseDistance(dv);
                                if (dd)
                                {
                                    sox.err_disp_override = dd.value();
                                    used = true;
                                }
                                else
                                    errors.emplace_back(
                                        1, 1, "/steps/" + id + "/with/sensitivity_overrides/err_disp_override",
                                        dd.error());
                            }
                            if (used)
                                s.sensitivity_overrides = sox;
                        }
                        params = s;
                    }
                    break;
                    case StrategyType::CreepConstantPressure: {
                        CreepConstantPressureParams s{};
                        std::string sv;
                        if (p.has_child("target_deviator_stress"))
                        {
                            p["target_deviator_stress"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.target_deviator_stress = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_deviator_stress", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_deviator_stress");
                        if (p.has_child("target_cell_pressure"))
                        {
                            p["target_cell_pressure"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.target_cell_pressure = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_cell_pressure", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_cell_pressure");
                        if (p.has_child("motor_speed_rpm_max"))
                        {
                            p["motor_speed_rpm_max"] >> s.motor_speed_rpm_max;
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "motor_speed_rpm_max");
                        if (p.has_child("stress_error_threshold"))
                        {
                            p["stress_error_threshold"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.stress_error_threshold = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/stress_error_threshold", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "stress_error_threshold");
                        if (p.has_child("hold_duration_minutes"))
                        {
                            p["hold_duration_minutes"] >> sv;
                            auto d = ParseDuration(sv);
                            if (d)
                                s.hold_duration_minutes = d.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/hold_duration_minutes", d.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "hold_duration_minutes");
                        params = s;
                    }
                    break;
                    case StrategyType::CyclicStressLoadingConstantPressure: {
                        CyclicStressLoadingConstantPressureParams s{};
                        std::string sv;
                        if (p.has_child("direction"))
                        {
                            p["direction"] >> sv;
                            auto d = ParseDirection(sv);
                            if (d)
                                s.direction = d.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/direction", d.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "direction");
                        if (p.has_child("motor_speed_rpm"))
                        {
                            p["motor_speed_rpm"] >> s.motor_speed_rpm;
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "motor_speed_rpm");
                        if (p.has_child("deviator_stress_min"))
                        {
                            p["deviator_stress_min"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.deviator_stress_min = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/deviator_stress_min", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "deviator_stress_min");
                        if (p.has_child("deviator_stress_max"))
                        {
                            p["deviator_stress_max"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.deviator_stress_max = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/deviator_stress_max", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "deviator_stress_max");
                        if (p.has_child("target_cell_pressure"))
                        {
                            p["target_cell_pressure"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.target_cell_pressure = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_cell_pressure", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_cell_pressure");
                        if (p.has_child("cycle_count_param1"))
                        {
                            double d1 = 0;
                            p["cycle_count_param1"] >> d1;
                            s.cycle_count_param1 = d1;
                        }
                        params = s;
                    }
                    break;
                    case StrategyType::MonotonicLoadingConstantVolume: {
                        MonotonicLoadingConstantVolumeParams s{};
                        std::string sv;
                        if (p.has_child("direction"))
                        {
                            p["direction"] >> sv;
                            auto d = ParseDirection(sv);
                            if (d)
                                s.direction = d.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/direction", d.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "direction");
                        if (p.has_child("motor_speed_rpm"))
                        {
                            p["motor_speed_rpm"] >> s.motor_speed_rpm;
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "motor_speed_rpm");
                        if (p.has_child("target_deviator_stress"))
                        {
                            p["target_deviator_stress"] >> sv;
                            auto pr = ParsePressure(sv);
                            if (pr)
                                s.target_deviator_stress = pr.value();
                            else
                                errors.emplace_back(1, 1, "/steps/" + id + "/with/target_deviator_stress", pr.error());
                        }
                        else
                            missing(("/steps/" + id + "/parameters").c_str(), "target_deviator_stress");
                        params = s;
                    }
                    break;
#pragma warning(push)
#pragma warning(disable : 4061)
                    default:
                        // For strategies not used in fixtures, keep default NoControlParams to avoid overreach
                        break;
                    }
#pragma warning(pop)
                }
                step.with = std::move(params);
                doc.steps.push_back(std::move(step));
            }

            if (doc.steps.empty())
            {
                errors.emplace_back(1, 1, "/steps", "At least one step is required");
            }
        }

        // postprocessing (optional)
        if (root.has_child("postprocessing"))
        {
            auto pp = root["postprocessing"];
            if (!pp.is_map())
            {
                errors.emplace_back(1, 1, "/postprocessing", "Expected mapping (object)");
            }
            else
            {
                if (pp.has_child("export_fifo"))
                {
                    bool b = false;
                    pp["export_fifo"] >> b;
                    doc.postprocessing.export_fifo = b;
                }
                if (pp.has_child("notes"))
                {
                    std::string s;
                    pp["notes"] >> s;
                    doc.postprocessing.notes = s;
                }
            }
        }

        // If any blocking errors, return them; otherwise success
        bool has_error = false;
        for (const auto &e : errors)
            if (e.severity == ErrorSeverity::Error)
            {
                has_error = true;
                break;
            }
        if (has_error)
            return std::unexpected(std::move(errors));
        return doc;
    }
    catch (const std::exception &ex)
    {
        errors.emplace_back(0, 0, "", std::string("YAML parse error: ") + ex.what());
        return std::unexpected(std::move(errors));
    }
}
} // namespace ControlConfig

#pragma warning(pop)
