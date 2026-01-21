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
 * @file control_patterns.hpp
 * @brief Pure function declarations for control patterns
 *
 * This file declares pure functions for all control patterns, replacing
 * the global-state-dependent functions in DigitShowDSTDoc.cpp.
 */

#pragma once

#include <chrono>
#include <unordered_map>

/**
 * @brief Control pattern identifiers
 *
 * This enum class represents all available control patterns in the system.
 * Each pattern corresponds to a specific testing mode (loading, creep, relaxation, etc.).
 * Values map 1:1 to legacy CFNUM codes for backward compatibility.
 */
enum class ControlPattern : size_t
{
    NoControl = 0,
    MonotonicLoadingConstantPressure = 1,
    MonotonicLoadingConstantVolume = 2,
    CyclicLoadingConstantPressure = 3,
    CyclicLoadingConstantVolume = 4,
    CreepConstantPressure = 5,
    CreepConstantVolume = 6,
    RelaxationConstantPressure = 7,
    RelaxationConstantVolume = 8,
    MonotonicLoadingDisplacementConstantPressure = 9,
    MonotonicLoadingDisplacementConstantVolume = 10,
    CyclicLoadingDisplacementConstantPressure = 11,
    CyclicLoadingDisplacementConstantVolume = 12,
    AccelerationConstantPressure = 13,
    AccelerationConstantVolume = 14,
    ConstantTauConsolidation = 15,
    KConsolidation = 16,
    CreepConstantPressureFast = 17,
    CreepConstantPressureFastRef = 18,
    PreConsolidation = 19,
    BeforeConsolidation = 20,
    AfterConsolidation = 21
};

// Map of known patterns to enum values
// Note: "rebase_reference" is the preferred name for the rebase operation (maps to CFNUM 20).
// "before_consolidation" (CFNUM 20) and "after_consolidation" (CFNUM 21) are kept as aliases
// for backward compatibility. Both execute identical code but maintain separate enum values
// for historical reasons. When outputting JSON, get_use_label() returns "rebase_reference" for both.
inline const std::unordered_map<std::string_view, ControlPattern> use_map = {
    {"rebase_reference", ControlPattern::BeforeConsolidation}, // Preferred name (CFNUM 20)
    {"no_control", ControlPattern::NoControl},
    {"monotonic_loading_constant_pressure", ControlPattern::MonotonicLoadingConstantPressure},
    {"monotonic_loading_constant_volume", ControlPattern::MonotonicLoadingConstantVolume},
    {"cyclic_loading_constant_pressure", ControlPattern::CyclicLoadingConstantPressure},
    {"cyclic_loading_constant_volume", ControlPattern::CyclicLoadingConstantVolume},
    {"creep_constant_pressure", ControlPattern::CreepConstantPressure},
    {"creep_constant_volume", ControlPattern::CreepConstantVolume},
    {"relaxation_constant_pressure", ControlPattern::RelaxationConstantPressure},
    {"relaxation_constant_volume", ControlPattern::RelaxationConstantVolume},
    {"monotonic_loading_displacement_constant_pressure", ControlPattern::MonotonicLoadingDisplacementConstantPressure},
    {"monotonic_loading_displacement_constant_volume", ControlPattern::MonotonicLoadingDisplacementConstantVolume},
    {"cyclic_loading_displacement_constant_pressure", ControlPattern::CyclicLoadingDisplacementConstantPressure},
    {"cyclic_loading_displacement_constant_volume", ControlPattern::CyclicLoadingDisplacementConstantVolume},
    {"acceleration_constant_pressure", ControlPattern::AccelerationConstantPressure},
    {"acceleration_constant_volume", ControlPattern::AccelerationConstantVolume},
    {"constant_tau_consolidation", ControlPattern::ConstantTauConsolidation},
    {"k_consolidation", ControlPattern::KConsolidation},
    {"creep_constant_pressure_fast", ControlPattern::CreepConstantPressureFast},
    {"creep_constant_pressure_fast_ref", ControlPattern::CreepConstantPressureFastRef},
    {"pre_consolidation", ControlPattern::PreConsolidation},
    {"before_consolidation", ControlPattern::BeforeConsolidation}, // Alias for backward compatibility
    {"after_consolidation",
     ControlPattern::AfterConsolidation}}; // Alias for backward compatibility (different enum but same behavior)

// Get use label from ControlPattern enum
inline std::string_view get_use_label(ControlPattern pattern) noexcept
{
    // Explicitly return preferred name for consolidated patterns
    if (pattern == ControlPattern::BeforeConsolidation || pattern == ControlPattern::AfterConsolidation)
    {
        return "rebase_reference";
    }

    for (const auto &[label, pat] : use_map)
    {
        if (pat == pattern)
            return label;
    }
    return "unknown";
}
