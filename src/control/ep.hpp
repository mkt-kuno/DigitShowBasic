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

#include "../math_constexpr.hpp"
#include "measurement.hpp"
#include "params.hpp"
#include "utils.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

/**
 * @brief Apply EP constant pressure control (P-control for confining pressure)
 *
 * This implements the three-state control logic:
 * 1. If sigma > target + ERR: Unload (reduce EP pressure)
 * 2. If sigma < target - ERR: Load (increase EP pressure)
 * 3. Otherwise: Neutral (balance front/rear displacement difference)
 *
 * @param params Control parameters
 * @param input Physical input state
 * @param output Physical output state
 * @return Updated physical output state
 */
template <control::PhysicalOutputLike Output>
inline constexpr Output apply_ep_constant_pressure_control(
    const control::ControlParams::VerticalStress &vertical_stress_params,
    const control::ControlParams::Tilt &tilt_params, const control::PhysicalInput &input, const Output &output) noexcept
{
    // sigmaをPVとしてEPの平均圧力を速度型I制御する
    const auto ki_sigma = vertical_stress_params.ki * input.specimen.area_mm2() / 1000.0;
    const auto sigma_error =
        apply_tolerance(vertical_stress_params.setpoint - input.vertical_stress_kpa(), vertical_stress_params.error);

    // 垂直変位差をPVとしてEPの圧力差を速度型I制御する
    const auto tilt_error = apply_tolerance(tilt_params.setpoint - input.tilt_mm(), tilt_params.error);

    const auto cv_limit_kpa = math_constexpr::abs(vertical_stress_params.cv_limit_kpa);
    auto new_output = output;
    new_output.front_ep_kpa +=
        std::clamp(ki_sigma * sigma_error + tilt_params.ki_kpa_per_mm * tilt_error, -cv_limit_kpa, cv_limit_kpa);
    new_output.rear_ep_kpa +=
        std::clamp(ki_sigma * sigma_error - tilt_params.ki_kpa_per_mm * tilt_error, -cv_limit_kpa, cv_limit_kpa);

    return new_output;
}

/**
 * @brief Apply EP constant volume control (maintain zero volumetric strain)
 *
 * Controls front and rear EP cells to keep displacements near zero,
 * implementing constant volume condition.
 *
 * @param params Control parameters
 * @param input Physical input state
 * @param output Physical output state
 * @return Updated physical output state
 */
template <control::PhysicalOutputLike Output>
inline constexpr Output apply_ep_constant_volume_control(const control::ControlParams::NormalDisplacement &disp_params,
                                                         const control::ControlParams::Tilt &tilt_params,
                                                         const control::PhysicalInput &input,
                                                         const Output &output) noexcept
{
    // 平均垂直変位をPVとしてEPの平均圧力を速度型I制御する
    const auto disp_ave_error =
        apply_tolerance(disp_params.setpoint - input.normal_displacement_mm(), disp_params.error);

    // 垂直変位差をPVとしてEPの圧力差を速度型I制御する
    const auto tilt_error = apply_tolerance(tilt_params.setpoint - input.tilt_mm(), tilt_params.error);

    const auto cv_limit_kpa = math_constexpr::abs(disp_params.cv_limit_kpa);
    auto new_output = output;
    new_output.front_ep_kpa +=
        std::clamp(disp_params.ki_kpa_per_mm * disp_ave_error + tilt_params.ki_kpa_per_mm * tilt_error, -cv_limit_kpa,
                   cv_limit_kpa);
    new_output.rear_ep_kpa +=
        std::clamp(disp_params.ki_kpa_per_mm * disp_ave_error - tilt_params.ki_kpa_per_mm * tilt_error, -cv_limit_kpa,
                   cv_limit_kpa);

    return new_output;
}
