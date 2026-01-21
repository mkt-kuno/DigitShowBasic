# Control Script Specifications

**Status:** Active (JSON bridge, 2025-11-02)

The control dialog now loads and saves JSON scripts that are validated against `schemas/control_script.schema.json`. Each step in `steps[]` is converted to legacy `CFNUM`/`CFPARA[0..17]` arrays by `src/Control_File.cpp` before being executed by `src/control/control.hpp::execute()` (Timer 2, 500 ms, extracted from DigitShowDSTDoc in PR #135). This document is the authoritative reference for the JSON format and its mapping back to legacy helpers.

## File Structure

```jsonc
{
	"$schema": "../schemas/control_script.schema.json",
	"steps": [
		{ "use": "pre_consolidation", "target_tau_kPa": 0.5, "motor_rpm": 1200 },
		{ "use": "constant_tau_consolidation", "direction": "compression", ... }
	]
}
```

- `steps` holds up to 128 entries.
- Each step is identified by the `use` string listed in the table below.
- Optional `description` strings carry through to the JSON file only (they do not affect execution).
- `overrides` supplies per-step sensitivity overrides (mapped to `CFPARA[10..17]`).

## `use` → CFNUM Summary

| `use` string | CFNUM | Legacy helper |
|--------------|-------|---------------|
| `no_control` | 0 | none (motor off, outputs hold) |
| `monotonic_loading_constant_pressure` | 1 | `MLoading_Constant_Pressure` |
| `monotonic_loading_constant_volume` | 2 | `MLoading_Constant_Volume` |
| `cyclic_loading_constant_pressure` | 3 | `CLoading_Constant_Pressure` |
| `cyclic_loading_constant_volume` | 4 | `CLoading_Constant_Volume` |
| `creep_constant_pressure` | 5 | `Creep_Constant_Pressure` |
| `creep_constant_volume` | 6 | `Creep_Constant_Volume` |
| `relaxation_constant_pressure` | 7 | `Relaxation_Constant_Pressure` |
| `relaxation_constant_volume` | 8 | `Relaxation_Constant_Volume` |
| `monotonic_loading_displacement_constant_pressure` | 9 | `MLoading_Disp_Constant_Pressure` |
| `monotonic_loading_displacement_constant_volume` | 10 | `MLoading_Disp_Constant_Volume` |
| `cyclic_loading_displacement_constant_pressure` | 11 | `CLoading_Disp_Constant_Pressure` |
| `cyclic_loading_displacement_constant_volume` | 12 | `CLoading_Disp_Constant_Volume` |
| `acceleration_constant_pressure` | 13 | `Acceleration_Constant_Pressure` |
| `acceleration_constant_volume` | 14 | `Acceleration_Constant_Volume` |
| `constant_tau_consolidation` | 15 | `Constant_Tau_Consolidation` |
| `k_consolidation` | 16 | `Path_MLoading_Constant_Pressure` |
| `creep_constant_pressure_fast` | 17 | `Creep_Constant_Pressure_Fast` |
| `creep_constant_pressure_fast_ref` | 18 | `Creep_Constant_Pressure_Fast_Ref` |
| `pre_consolidation` | 19 | `Pre_consolidation` |
| `rebase_reference` | 20/21 | `Before_consolidation` / `After_consolidation` (both execute same code) |

**Note: `wait` (CFNUM 22) has been removed from the codebase. Legacy files containing `"use": "wait"` will automatically fallback to `no_control`.**

## Step Completion Conditions

Each control step advances to the next when its specific termination condition is met. Completion evaluation is centralized in `src/control/step_completion.hpp::evaluate_step_completion()`, called by the dispatcher (`src/control/control.hpp::execute()`).

### Completion Types

| Completion Type | Check | Applied To |
|-----------------|-------|-----------|
| **Stroke Limit** | Displacement out of range [-5mm, 15mm] | All motor-based patterns |
| **Target Stress** | τ (shear stress) reaches `target_tau_kPa` in specified direction | Monotonic loading (CFNUM 1,2) |
| **Target Displacement** | Axial displacement reaches `target_displacement_mm` | Monotonic displacement (CFNUM 9,10) |
| **Cycle Count** | `num_cyclic` > `num_cycles` | Cyclic patterns (CFNUM 3,4,11,12) |
| **Timer** | Elapsed time ≥ `time_min` | Creep/relaxation (CFNUM 5–8,17,18) |
| **Target Sigma** | \|σ' - target_sigma\| < `err_stress_kPa` | Constant tau consolidation (CFNUM 15) |
| **Immediate** | Always true | No control, rebase (CFNUM 0,20,21) |

### Pattern → Completion Matrix

| Pattern (CFNUM) | Stroke | Stress | Displacement | Cycles | Timer | Sigma | Immediate |
|-----------------|:------:|:------:|:------------:|:------:|:-----:|:-----:|:---------:|
| 0 – no_control | | | | | | | ✓ |
| 1 – monotonic_loading_constant_pressure | ✓ | ✓ | | | | | |
| 2 – monotonic_loading_constant_volume | ✓ | ✓ | | | | | |
| 3 – cyclic_loading_constant_pressure | ✓ | | | ✓ | | | |
| 4 – cyclic_loading_constant_volume | ✓ | | | ✓ | | | |
| 5 – creep_constant_pressure | ✓ | | | | ✓ | | |
| 6 – creep_constant_volume | ✓ | | | | ✓ | | |
| 7 – relaxation_constant_pressure | ✓ | | | | ✓ | | |
| 8 – relaxation_constant_volume | ✓ | | | | ✓ | | |
| 9 – monotonic_loading_displacement_constant_pressure | ✓ | | ✓ | | | | |
| 10 – monotonic_loading_displacement_constant_volume | ✓ | | ✓ | | | | |
| 11 – cyclic_loading_displacement_constant_pressure | ✓ | | | ✓ | | | |
| 12 – cyclic_loading_displacement_constant_volume | ✓ | | | ✓ | | | |
| 13 – acceleration_constant_pressure | ✓ | | | | | | |
| 14 – acceleration_constant_volume | ✓ | | | | | | |
| 15 – constant_tau_consolidation | ✓ | | | | | ✓ | |
| 16 – k_consolidation | ✓ | ✓ | | | | | |
| 17 – creep_constant_pressure_fast | ✓ | | | | ✓ | | |
| 18 – creep_constant_pressure_fast_ref | ✓ | | | | ✓ | | |
| 19 – pre_consolidation | ✓ | | | | | | |
| 20/21 – rebase_reference | | | | | | | ✓ |

**Notes:**
- Stroke limit (displacement ≤ -5mm or ≥ 15mm) triggers immediate completion for all motor-based patterns to protect hardware.
- Multiple conditions may apply; the step completes when *any* condition becomes true.
- Cyclic patterns internally manage direction reversals via `flag_cyclic`; completion occurs only when `num_cyclic` exceeds the target.
- Acceleration patterns (CFNUM 13,14) currently complete only on stroke limit or manual intervention (target RPM completion pending implementation).

## Shared Fields


| JSON key | Type | Notes | CF Index when present |
|----------|------|-------|-----------------------|
| `motor_rpm` | number | Motor speed in RPM with sign indicating direction: **positive = loading/compression**, **negative = unloading/dilation**. | `[0]` for all patterns |
| `tau_kPa`, `target_tau_kPa`, `tau_start_kPa`, `tau_end_kPa` | number | Deviator stress targets. | `[2]` or `[3]` depending on strategy |
| `sigma_kPa`, `sigma_start_kPa`, `target_sigma_kPa` | number | Radial effective stress set-points (kPa). | `[3]`–`[5]` depending on strategy |
| `time_min` | number | Duration in minutes for creep/relaxation. | `[2]` or `[0]` depending on strategy |
| `num_cycles` | integer | Cycle count for cyclic modes. | `[4]` or `[5]` |
| `target_displacement_mm`, `displacement_lower_mm`, `displacement_upper_mm` | number | Axial displacement targets (mm). | `[2]`–`[3]` |
| `acceleration_rate_rpm_per_min` | number | Motor acceleration rate (RPM/min). | `[2]` |
| `k_value` | number | Stress-path slope (dimensionless). | `[5]` |
| `overrides` | object | Optional sensitivity overrides (see below). | `[10]`–`[17]` |

### Sensitivity Overrides

`overrides` is an object with any of the following keys. Values are written into `CFPARA[9..16]` if provided; missing entries keep the document defaults.

| Override key | CF index | Units | Description |
|--------------|----------|-------|-------------|
| `err_stress_kPa` | `[9]` | kPa | Overrides `ERR_StressP` (EP stress controller tolerance). |
| `err_disp_mm` | `[10]` | mm | Overrides `ERR_Disp` (front/rear displacement balance). |
| `amp_V_per_kPa_m2` | `[11]` | V/(kPa·m²) | Overrides `amp` (stress loop gain). |
| `amp2f_V_per_mm` | `[12]` | V/mm | Overrides `amp2F` (front displacement gain). |
| `amp2r_V_per_mm` | `[13]` | V/mm | Overrides `amp2R` (rear displacement gain). |
| `dmax_V` | `[14]` | V | Overrides `Dmax` (EP slew limit). |
| `err_disp_cv_mm` | `[15]` | mm | Overrides `ERR_DispCV` (constant-volume displacement tolerance). |
| `amp_cv_V_per_mm` | `[16]` | V/mm | Overrides `ampCV` (constant-volume gain). |

## Step Details

Each section lists the required JSON keys, optional extras, and the resulting `CFPARA` mapping. Units follow the schema descriptions.

### 0 – `no_control`
- Keys: `use = "no_control"`; optional `description`.
- Effect: motor clutch remains on, no movement; outputs hold previous values.
- Mapping: all `CFPARA` values zero.

### 1 – `monotonic_loading_constant_pressure`
- Required keys: `motor_rpm` (sign indicates direction), `tau_kPa`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_kPa`, `[2]=sigma_kPa`.

### 2 – `monotonic_loading_constant_volume`
- Required keys: `motor_rpm` (sign indicates direction), `tau_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_kPa`.

### 3 – `cyclic_loading_constant_pressure`
- Required keys: `motor_rpm` (initial phase from sign), `tau_lower_kPa`, `tau_upper_kPa`, `num_cycles`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_lower`, `[2]=tau_upper`, `[3]=num_cycles`, `[4]=sigma_kPa`.

### 4 – `cyclic_loading_constant_volume`
- Required keys: `motor_rpm` (initial phase from sign), `tau_lower_kPa`, `tau_upper_kPa`, `num_cycles`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_lower`, `[2]=tau_upper`, `[3]=num_cycles`.

### 5 – `creep_constant_pressure`
- Required keys: `motor_rpm`, `tau_kPa`, `time_min`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_kPa`, `[2]=time_min`, `[3]=sigma_kPa`.

### 6 – `creep_constant_volume`
- Required keys: `motor_rpm`, `tau_kPa`, `time_min`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_kPa`, `[2]=time_min`.

### 7 – `relaxation_constant_pressure`
- Required keys: `time_min`, `sigma_kPa`.
- Mapping: `[0]=time_min`, `[1]=sigma_kPa`.

### 8 – `relaxation_constant_volume`
- Required keys: `time_min`.
- Mapping: `[0]=time_min`.

### 9 – `monotonic_loading_displacement_constant_pressure`
- Required keys: `motor_rpm` (sign indicates direction), `target_displacement_mm`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=target_displacement`, `[2]=sigma_kPa`.

### 10 – `monotonic_loading_displacement_constant_volume`
- Required keys: `motor_rpm` (sign indicates direction), `target_displacement_mm`.
- Mapping: `[0]=motor_rpm`, `[1]=target_displacement`.

### 11 – `cyclic_loading_displacement_constant_pressure`
- Required keys: `motor_rpm` (initial phase from sign), `displacement_lower_mm`, `displacement_upper_mm`, `num_cycles`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=lower`, `[2]=upper`, `[3]=num_cycles`, `[4]=sigma_kPa`.

### 12 – `cyclic_loading_displacement_constant_volume`
- Required keys: `motor_rpm` (initial phase from sign), `displacement_lower_mm`, `displacement_upper_mm`, `num_cycles`.
- Mapping: `[0]=motor_rpm`, `[1]=lower`, `[2]=upper`, `[3]=num_cycles`.

### 13 – `acceleration_constant_pressure`
- Required keys: `motor_rpm` (initial, sign indicates direction), `acceleration_rate_rpm_per_min`, `target_tau_kPa`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=acceleration_rate`, `[2]=target_tau`, `[3]=sigma_kPa`.

### 14 – `acceleration_constant_volume`
- Required keys: `motor_rpm` (initial, sign indicates direction), `acceleration_rate_rpm_per_min`, `target_tau_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=acceleration_rate`, `[2]=target_tau`.

### 15 – `constant_tau_consolidation`
- Required keys: `motor_rpm` (sign indicates compression/dilation), `tau_kPa`, `consolidation_rate_kPa_per_min`, `target_sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_kPa`, `[2]=consolidation_rate`, `[3]=target_sigma`.

### 16 – `k_consolidation`
- Required keys: `motor_rpm` (sign indicates compression/dilation), `tau_start_kPa`, `tau_end_kPa`, `sigma_start_kPa`, `k_value`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_start`, `[2]=tau_end`, `[3]=sigma_start`, `[4]=k_value`.

### 17 – `creep_constant_pressure_fast`
- Required keys: `motor_rpm` (sign indicates direction), `tau_kPa`, `time_min`, `sigma_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=tau_kPa`, `[2]=time_min`, `[3]=sigma_kPa`.

### 18 – `creep_constant_pressure_fast_ref`
- Required keys: `motor_rpm` (sign indicates direction), `tau_kPa`, `time_min`, `sigma_kPa`.
- Mapping identical to CFNUM 17: `[0]=motor_rpm`, `[1]=tau_kPa`, `[2]=time_min`, `[3]=sigma_kPa`.

### 19 – `pre_consolidation`
- Required keys: `motor_rpm`, `target_tau_kPa`.
- Mapping: `[0]=motor_rpm`, `[1]=target_tau_kPa`.

### 20/21 – `rebase_reference`
- Required keys: `use = "rebase_reference"`; optional `description`.
- **Aliases for backward compatibility**: `before_consolidation` (CFNUM=20), `after_consolidation` (CFNUM=21)
- Effect: Rebases the measurement reference point (zero-reference) for physical inputs. This is a placeholder step used to mark the start or end of a consolidation phase. No parameters are required.
- Mapping: all `CFPARA` values zero.
- Note: Both `before_consolidation` (CFNUM=20) and `after_consolidation` (CFNUM=21) execute identical code (see DigitShowDSTDoc.cpp) and are now unified under the `rebase_reference` name. Legacy control scripts using the old names will continue to work but new scripts should use `rebase_reference`. The enum values remain separate but may be consolidated in a future PR.

## Implementation Notes
- Conversion is performed by `src/Control_File.cpp` (`JsonStepToCFNUM` / `CFNUMToJsonStep`). Keep the mapping tables above in sync with those helpers when adding new strategies or parameters.
- Motor direction encoding:
	- **New format (v1.1+)**: Motor direction is encoded in the sign of `motor_rpm`. Positive = loading/compression, negative = unloading/dilation.
	- For cyclic patterns, the sign of `motor_rpm` determines the initial phase (positive = start loading, negative = start unloading).
	- Existing `.ctl.json` files using the legacy `direction` field must be migrated using `scripts/migrate_control_files.py` before use.
- Optional fields missing from the JSON fall back to zero, which in most helpers means "disabled" (e.g., `sigma_kPa = 0` disables EP control).

## Related Material
- `schemas/control_script.schema.json` (schema specification and validation constraints)
- `control_mechanisms_overview.md` (execution flow and Timer 2 context)
- `control_logic_extraction_targets.md` (strategy extraction backlog)
