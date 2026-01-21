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
 * @file control/measurement.hpp
 * @brief Lightweight snapshots for specimen geometry and physical IO values.
 */

#pragma once

#include "../math_constexpr.hpp"
#include <array>
#include <concepts>
#include <utility>

namespace control
{

class SpecimenSnapshot
{
  public:
    constexpr SpecimenSnapshot() noexcept = default;

    constexpr SpecimenSnapshot(const double height_mm, const double area_mm2, const double weight_g,
                               const double box_weight_g = 0.0) noexcept
        : height_mm_(height_mm), area_mm2_(area_mm2), weight_g_(weight_g), box_weight_g_(box_weight_g)
    {
    }

    [[nodiscard]] constexpr double height_mm() const noexcept
    {
        return height_mm_;
    }
    [[nodiscard]] constexpr double area_mm2() const noexcept
    {
        return area_mm2_;
    }
    [[nodiscard]] constexpr double volume_mm3() const noexcept
    {
        return height_mm_ * area_mm2_;
    }
    [[nodiscard]] constexpr double weight_g() const noexcept
    {
        return weight_g_;
    }
    [[nodiscard]] constexpr double box_weight_g() const noexcept
    {
        return box_weight_g_;
    }

  private:
    double height_mm_ = 0.0;
    double area_mm2_ = 0.0;
    double weight_g_ = 0.0;
    double box_weight_g_ = 0.0;
};

struct PhysicalInput
{
    SpecimenSnapshot specimen{};
    double shear_force_N = 0.0;
    double vertical_force_N = 0.0;
    double shear_displacement_mm = 0.0;
    double front_vertical_disp_mm = 0.0;
    double rear_vertical_disp_mm = 0.0;
    double front_friction_force_N = 0.0;
    double rear_friction_force_N = 0.0;

    [[nodiscard]] constexpr double vertical_stress_kpa() const noexcept
    {
        return to_kpa(this->vertical_force_N + normal_force_component());
    }
    [[nodiscard]] constexpr double shear_stress_kpa() const noexcept
    {
        // friction LCとshear LCで得られる荷重は符号が逆という想定なため、
        // そのままshear LCに足すことでfriction LCで測定したレールとの摩擦力を取り除ける
        return to_kpa(this->shear_force_N + this->front_friction_force_N + this->rear_friction_force_N);
    }
    [[nodiscard]] constexpr double normal_displacement_mm() const noexcept
    {
        return 0.5 * (front_vertical_disp_mm + rear_vertical_disp_mm);
    }
    [[nodiscard]] constexpr double front_vertical_displacement_mm() const noexcept
    {
        return front_vertical_disp_mm;
    }
    [[nodiscard]] constexpr double rear_vertical_displacement_mm() const noexcept
    {
        return rear_vertical_disp_mm;
    }
    [[nodiscard]] constexpr double tilt_mm() const noexcept
    {
        return 0.5 * (front_vertical_disp_mm - rear_vertical_disp_mm);
    }

  private:
    [[nodiscard]] constexpr double to_kpa(const double force_component) const noexcept
    {
        return specimen.area_mm2() > 0.0 ? force_component / specimen.area_mm2() * 1000.0 : 0.0;
    }
    [[nodiscard]] constexpr double box_weight_force_component() const noexcept
    {
        return specimen.box_weight_g() * 0.001 * 9.81;
    }
    [[nodiscard]] constexpr double specimen_weight_force_component() const noexcept
    {
        return specimen.weight_g() * 0.001 * 9.81;
    }
    [[nodiscard]] constexpr double normal_force_component() const noexcept
    {
        return box_weight_force_component() + 0.5 * specimen_weight_force_component();
    }
};

constexpr auto toVoltage(const double physical_value, const double cal_a, const double cal_b) noexcept
{
    return cal_a * physical_value + cal_b;
}

template <typename Value = double>
constexpr auto fromVoltage(const Value voltage, const double cal_a, const double cal_b) noexcept
{
    return (voltage - cal_b) / cal_a;
}

constexpr std::array<float, 3> toIISMotorVoltage(const double rpm, const double cal_a, const double cal_b) noexcept
{
    return {{
        // ON: 5.0V, OFF: 0.0V
        rpm != 0.0 ? 5.0f : 0.0f,
        // UP: 0.0V, DOWN: 5.0V
        rpm > 0.0 ? 0.0f : 5.0f,
        // SPEED
        static_cast<float>(toVoltage(math_constexpr::abs(rpm), cal_a, cal_b)),
    }};
}

constexpr auto fromIISMotorVoltage(const float on_voltage, const float clutch_voltage, const float speed_voltage,
                                   const double cal_a, const double cal_b) noexcept
{
    return on_voltage <= 0.f || speed_voltage <= 0.f
               ? 0.0
               : math_constexpr::copysign(fromVoltage(static_cast<double>(speed_voltage), cal_a, cal_b),
                                          clutch_voltage > 0.f ? -1.0 : 1.0);
}

template <typename Value = double> struct PhysicalOutput
{
    Value front_ep_kpa;
    Value rear_ep_kpa;
    Value motor_rpm;
};

// requires at minimum the same member variables as PhysicalOutput<Value>
template <typename T, typename Value = double>
concept PhysicalOutputLike = requires(T output) {
    { output.front_ep_kpa } -> std::convertible_to<Value>;
    { output.rear_ep_kpa } -> std::convertible_to<Value>;
    { output.motor_rpm } -> std::convertible_to<Value>;
};

[[nodiscard]] constexpr SpecimenSnapshot present_specimen(const PhysicalInput &input) noexcept
{
    return SpecimenSnapshot{
        input.specimen.height_mm() - input.normal_displacement_mm(),
        input.specimen.area_mm2(),
        input.specimen.weight_g(),
        input.specimen.box_weight_g(),
    };
}

[[nodiscard]] constexpr PhysicalInput rebase(const PhysicalInput &input, const SpecimenSnapshot &reference) noexcept
{
    const double absolute_height_mm = input.specimen.height_mm() - input.normal_displacement_mm();
    const double rebased_normal_mm = reference.height_mm() - absolute_height_mm;
    // 変位差はSpecimenで表現できないのでセンサーデータに引き継ぐ
    const double new_front_disp = rebased_normal_mm + input.tilt_mm();
    const double new_rear_disp = rebased_normal_mm - input.tilt_mm();

    return PhysicalInput{
        reference,      input.shear_force_N, input.vertical_force_N,       input.shear_displacement_mm,
        new_front_disp, new_rear_disp,       input.front_friction_force_N, input.rear_friction_force_N,
    };
}

} // namespace control
