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
 * @file consolidation.hpp
 * @brief Stateful control strategies for consolidation patterns
 *
 * This file contains control strategies that require internal state
 * across multiple control ticks. These are implemented as function objects
 * (classes with operator()) to maintain state between calls.
 */

#pragma once

#include "../../Constants.h"
#include "../../Variables.hpp"
#include "../control.hpp"
#include "../measurement.hpp"
#include "../patterns.hpp"
#include "../strategy_types.hpp"
#include "../utils.hpp"
#include <spdlog/spdlog.h>

namespace control::strategies
{

/**
 * @brief Pre-consolidation strategy with state tracking
 *
 * This strategy applies initial consolidation pressure before the main test.
 * It tracks the initial sigma value across multiple control ticks.
 */
class PreConsolidationStrategy
{
  public:
    explicit PreConsolidationStrategy(const ControlContext &ctx)
        : initial_sigma_(ctx.physical_input.load().vertical_stress_kpa())
    {
        spdlog::debug("PreConsolidation initialized with sigma={} kPa", initial_sigma_);
    }

    ControlOutput operator()(const ControlContext &ctx) const
    {
        const auto &p = ctx.current_step.parameters;
        const auto &physical_input = ctx.physical_input.load();
        const double area = physical_input.specimen.area_mm2();

        auto output = ctx.control_output;

        // Apply incremental EP pressure adjustment
        output.front_ep_kpa += 0.2 * -initial_sigma_ * area / 1000.0;
        output.rear_ep_kpa += 0.2 * -initial_sigma_ * area / 1000.0;

        // Apply motor tau control
        return apply_motor_tau_control(p.shear_stress_kpa, ctx.physical_input, output);
    }

  private:
    double initial_sigma_;
};

/**
 * @brief Constant tau consolidation strategy with state tracking
 *
 * This strategy maintains constant deviator stress while ramping vertical
 * stress at a specified rate. It tracks the initial sigma value across
 * multiple control ticks.
 */
class ConstantTauConsolidationStrategy
{
  public:
    explicit ConstantTauConsolidationStrategy(const ControlContext &ctx)
        : initial_sigma_(ctx.physical_input.load().vertical_stress_kpa())
    {
        spdlog::debug("ConstantTauConsolidation initialized with sigma={} kPa", initial_sigma_);
    }

    ControlOutput operator()(const ControlContext &ctx) const
    {
        const auto &p = ctx.current_step.parameters;

        const auto sigma_rate = math_constexpr::copysign(p.consolidation_rate_kpa_per_min,
                                                         p.vertical_stress_kpa.setpoint - this->initial_sigma_);

        // 定圧制御を転用し、目標値を変動させることで応力速度制御を実現する

        auto vertical_stress_params_ = p.vertical_stress_kpa;
        // target sigma rateを加味したsigmaの目標値を計算する
        // step開始からparams.consolidation_rateで変化したと仮定した場合のsigmaを目標値とする
        vertical_stress_params_.setpoint =
            this->initial_sigma_ +
            sigma_rate * std::chrono::duration_cast<std::chrono::minutes_d>(ctx.elapsed_time).count();

        return apply_motor_tau_control(p.shear_stress_kpa, ctx.physical_input,
                                       apply_ep_constant_pressure_control(vertical_stress_params_, p.tilt_mm,
                                                                          ctx.physical_input, ctx.control_output));
    }

  private:
    double initial_sigma_;
};

} // namespace control::strategies
