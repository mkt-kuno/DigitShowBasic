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
 * @file strategy_factory.cpp
 * @brief Implementation of control strategy factory
 *
 * This file implements the factory function that creates appropriate control
 * strategies based on the control pattern.
 */
#include "../StdAfx.h"

#include "ep.hpp"
#include "patterns.hpp"
#include "strategies/consolidation.hpp"
#include "strategies/stateless.hpp"
#include "strategy_factory.hpp"
#include <spdlog/spdlog.h>

namespace control
{

ControlStrategy create_strategy(const ControlPattern pattern, const ControlContext &ctx)
{
    using namespace strategies;
    using enum ControlPattern;

    switch (pattern)
    {
    // No control - motor off
    case NoControl:
        return no_control_strategy;

    // Rebase operations (before/after consolidation)
    case BeforeConsolidation:
    case AfterConsolidation:
        return rebase_reference_strategy;

    // Consolidation patterns (stateful - require initialization with context)
    case PreConsolidation:
        return PreConsolidationStrategy{ctx};
    case ConstantTauConsolidation:
        return ConstantTauConsolidationStrategy{ctx};
    case KConsolidation:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto input = ctx.physical_input.load();
            const auto tau_progress = input.shear_stress_kpa() / ctx.current_step.parameters.path_mloading.target_tau;

            auto vertical_stress_params_ = ctx.current_step.parameters.vertical_stress_kpa;
            vertical_stress_params_.setpoint = ctx.current_step.parameters.path_mloading.sigma_start +
                                               (ctx.current_step.parameters.path_mloading.sigma_end -
                                                ctx.current_step.parameters.path_mloading.sigma_start) *
                                                   tau_progress;

            auto output = apply_ep_constant_pressure_control(
                vertical_stress_params_, ctx.current_step.parameters.tilt_mm, input, ctx.control_output);
            output.motor_rpm = ctx.current_step.parameters.path_mloading.motor_rpm;
            return output;
        };

    // Monotonic loading with constant pressure
    case MonotonicLoadingConstantPressure:
    case MonotonicLoadingDisplacementConstantPressure:
        return [](const ControlContext &ctx) -> ControlOutput {
            auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                             ctx.current_step.parameters.tilt_mm, ctx.physical_input,
                                                             ctx.control_output);
            output.motor_rpm = ctx.current_step.parameters.monotonic_loading.motor_rpm;
            return output;
        };

    // Monotonic loading with constant volume
    case MonotonicLoadingConstantVolume:
    case MonotonicLoadingDisplacementConstantVolume:
        return [](const ControlContext &ctx) -> ControlOutput {
            auto output = apply_ep_constant_volume_control(ctx.current_step.parameters.normal_displacement_mm,
                                                           ctx.current_step.parameters.tilt_mm, ctx.physical_input,
                                                           ctx.control_output);
            output.motor_rpm = ctx.current_step.parameters.monotonic_loading.motor_rpm;
            return output;
        };

    // Cyclic loading with constant pressure
    case CyclicLoadingConstantPressure:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                                   ctx.current_step.parameters.tilt_mm,
                                                                   ctx.physical_input, ctx.control_output);
            return apply_cyclic_motor_control(ctx.current_step.parameters.cyclic_loading, ctx.physical_input, output);
        };

    // Cyclic loading with constant volume
    case CyclicLoadingConstantVolume:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_volume_control(ctx.current_step.parameters.normal_displacement_mm,
                                                                 ctx.current_step.parameters.tilt_mm,
                                                                 ctx.physical_input, ctx.control_output);
            return apply_cyclic_motor_control(ctx.current_step.parameters.cyclic_loading, ctx.physical_input, output);
        };

    // Cyclic loading displacement with constant pressure
    case CyclicLoadingDisplacementConstantPressure:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                                   ctx.current_step.parameters.tilt_mm,
                                                                   ctx.physical_input, ctx.control_output);
            return apply_cyclic_motor_control_disp(ctx.current_step.parameters.cyclic_loading, ctx.physical_input,
                                                   output);
        };

    // Cyclic loading displacement with constant volume
    case CyclicLoadingDisplacementConstantVolume:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_volume_control(ctx.current_step.parameters.normal_displacement_mm,
                                                                 ctx.current_step.parameters.tilt_mm,
                                                                 ctx.physical_input, ctx.control_output);
            return apply_cyclic_motor_control_disp(ctx.current_step.parameters.cyclic_loading, ctx.physical_input,
                                                   output);
        };

    // Acceleration with constant pressure
    case AccelerationConstantPressure:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                                   ctx.current_step.parameters.tilt_mm,
                                                                   ctx.physical_input, ctx.control_output);
            return apply_motor_acceleration(ctx.current_step.parameters.motor_acceleration, ctx.physical_input, output);
        };

    // Acceleration with constant volume
    case AccelerationConstantVolume:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_volume_control(ctx.current_step.parameters.normal_displacement_mm,
                                                                 ctx.current_step.parameters.tilt_mm,
                                                                 ctx.physical_input, ctx.control_output);
            return apply_motor_acceleration(ctx.current_step.parameters.motor_acceleration, ctx.physical_input, output);
        };

    // Creep with constant pressure
    case CreepConstantPressure:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                                   ctx.current_step.parameters.tilt_mm,
                                                                   ctx.physical_input, ctx.control_output);

            return apply_motor_tau_control(ctx.current_step.parameters.shear_stress_kpa, ctx.physical_input, output);
        };

    // Creep with constant volume
    case CreepConstantVolume:
        return [](const ControlContext &ctx) -> ControlOutput {
            const auto output = apply_ep_constant_volume_control(ctx.current_step.parameters.normal_displacement_mm,
                                                                 ctx.current_step.parameters.tilt_mm,
                                                                 ctx.physical_input, ctx.control_output);
            return apply_motor_tau_control(ctx.current_step.parameters.shear_stress_kpa, ctx.physical_input, output);
        };

    // Creep with constant pressure (fast variant)
    case CreepConstantPressureFast:
        return [](const ControlContext &ctx) -> ControlOutput {
            auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                             ctx.current_step.parameters.tilt_mm, ctx.physical_input,
                                                             ctx.control_output);
            // TODO: アルゴリズムを間違えている、要修正
            output.motor_rpm = ctx.current_step.parameters.shear_stress_kpa.cv_limit_rpm;
            return apply_motor_tau_control(ctx.current_step.parameters.shear_stress_kpa, ctx.physical_input, output);
        };

    // Creep with constant pressure (fast variant with reference)
    case CreepConstantPressureFastRef:
        return [](const ControlContext &ctx) -> ControlOutput {
            auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                             ctx.current_step.parameters.tilt_mm, ctx.physical_input,
                                                             ctx.control_output);
            // TODO: Need to use reference tau from parameters
            output.motor_rpm = ctx.current_step.parameters.shear_stress_kpa.cv_limit_rpm;
            return apply_motor_tau_control(ctx.current_step.parameters.shear_stress_kpa, ctx.physical_input, output);
        };

    // Relaxation with constant pressure
    case RelaxationConstantPressure:
        return [](const ControlContext &ctx) -> ControlOutput {
            auto output = apply_ep_constant_pressure_control(ctx.current_step.parameters.vertical_stress_kpa,
                                                             ctx.current_step.parameters.tilt_mm, ctx.physical_input,
                                                             ctx.control_output);
            output.motor_rpm = 0;
            return output;
        };

    // Relaxation with constant volume
    case RelaxationConstantVolume:
        return [](const ControlContext &ctx) -> ControlOutput {
            auto output = apply_ep_constant_volume_control(ctx.current_step.parameters.normal_displacement_mm,
                                                           ctx.current_step.parameters.tilt_mm, ctx.physical_input,
                                                           ctx.control_output);
            output.motor_rpm = 0;
            return output;
        };

    default:
        spdlog::warn("Unknown control pattern: {}", static_cast<size_t>(pattern));
        return [](const ControlContext &ctx) -> ControlOutput { return ctx.control_output; };
    }
}
} // namespace control