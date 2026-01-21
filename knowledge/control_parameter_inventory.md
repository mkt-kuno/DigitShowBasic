# Control Parameter Inventory

**Status:** Active — aligned with the JSON control bridge (2025-11-02)

This note keeps a readable index of the control capabilities that DigitShowDST exposes through the production JSON workflow files (`src/Control_File.cpp` loader) and the legacy CFNUM helpers behind `src/DigitShowDSTDoc::Control_DA()`. Use it to answer "which `uses` string do I need?" or "where does this number land in the document?" and pair it with `control_specifications.md`, which remains the authoritative mapping of field names, units, and defaults.

## How to read the inventory
- Everything the runtime executes becomes a **workflow step**. Today we persist those steps as JSON (`schemas/control_workflow.schema.json`); the same model will be serialised to YAML once the schema reboot lands.
- Legacy CFNUM routines already live behind the JSON bridge. When you see “workflow strategy” below, think `uses: ...` in JSON and the matching CFNUM branch in `Control_DA()`.
- Specimen dialogs are surfaced as strategies too. They still call into legacy UI code, but the JSON bridge can schedule them alongside scripted loading.
	- Note: Manual voltage passthrough (Control ID 1–14) was removed on 2025-11-02; use scripted steps instead.

## Strategy families at a glance

| Legacy entry point | Workflow `uses` value | Notes |
|--------------------|-----------------------|-------|
| CFNUM 0            | `no_control`                               | Motor off, outputs hold; step completes immediately. |
| CFNUM 1 / 2        | `monotonic_loading_constant_pressure` / `_constant_volume` | Stress-controlled monotonic loading. |
| CFNUM 3 / 4        | `cyclic_stress_loading_constant_pressure` / `_constant_volume` | Stress/strain cyclic variants; constant-volume counterparts reuse the same helpers with volume lock flags. |
| CFNUM 5 / 6 / 7 / 8| `creep_*`, `relaxation_*`                  | Long holds with adaptive motor limits; duration handling still tied to legacy timers. |
| CFNUM 9 / 10       | `displacement_loading_constant_pressure` / `_constant_volume` | Displacement targets with optional sigma hold. |
| CFNUM 11 / 12      | `cyclic_displacement_loading_constant_pressure` / `_constant_volume` | Pair of cyclic displacement routines with cycle counters. |
| CFNUM 13 / 14      | `acceleration_constant_pressure` / `_constant_volume` | Motor acceleration ramps. |
| CFNUM 15           | `constant_tau_consolidation`               | Holds tau while ramping sigma; only ERR_Disp override is honoured. |
| CFNUM 16           | `stress_path_loading_constant_pressure`    | Linear stress path defined by start/end tau and sigma start plus slope. |
| CFNUM 17 / 18      | `creep_constant_pressure_fast` / `_fast_ref` | Fast-response creep modes with reference tracking. |
| CFNUM 19           | `pre_consolidation`                        | Isotropic pre-consolidation controller, distinct from specimen state capture. |
| Specimen buttons   | `specimen_state_capture`                   | Wraps `OnBUTTONBeConsol` / `OnBUTTONAfConsol`; resets LVDTs and updates specimen arrays. |
| Utility            | `wait`                                     | Synthetic strategy for delays/manual approvals, implemented in the bridge. |

For the definitive parameter dictionary (field names, units, ranges, optional sensitivity overrides), see `control_specifications.md`. That document mirrors the JSON schema and lists every `with` field that the loader accepts today.

## Implementation breadcrumbs
- **Loader:** `src/Control_File.cpp` parses the JSON file, validates against `schemas/control_workflow.schema.json`, and populates the legacy `CFNUM`/`CFPARA` arrays. Specimen capture steps may be injected as synthetic rows where needed.
- **Runtime:** `src/digitshow_operations.cpp::execute_control_step()` (Timer 2, extracted from DigitShowDSTDoc in PR #135) still performs the switch on `ControlDataNum`/`CFNUM`, invoking helpers in `src/control/` modules and writing to hardware. The JSON bridge shortens that switch by treating strategies uniformly but has not yet extracted the logic into standalone services.
- **Sensitivity overrides:** Optional indices 10–17 remain exposed for strategies that honour them. The JSON bridge only pushes values that differ from the global defaults held in `src/Sensitivity.hpp`.
- **Cycle counters:** The meaning of `cycle_count_param1/2` for cyclic strategies is unchanged from the legacy implementation. They feed the internal counters in `src/digitshow_operations.cpp::execute_control_step()` (extracted in PR #135); documentation updates will follow once we untangle that logic during strategy extraction.

## Guard rails and open items
- The inventory reflects **current behaviour** of the JSON bridge. When the strategy extraction (Phase 2) lands, expect the same names but new service boundaries (`ControlScheduler`, `ActuatorWriter`). Revisit this file once those types exist.
- Specimen state capture remains a thin wrapper around UI code. Until it is reimplemented as a pure service, expect side effects like dialog message boxes when running headless.
<!-- Manual voltage passthrough removed in 2025-11-02; guidance no longer applicable. -->
- Duration semantics for creep/relaxation/fast modes remain tied to `step_elapsed` and timer-driven checks. Verify with the helper implementation before assuming the JSON field enforces a hard stop.

## Where to deepen
- `control_mechanisms_overview.md` – narrative of Timer 2 flow and how JSON scripts interact with helpers.
- `control_logic_extraction_targets.md` – current shortlist for pulling strategies out of the document class.
- `ctl_to_yaml_migration.md` – legacy CTL conversion notes; still useful when comparing archived scripts during audits.

Keep this page synced with `control_specifications.md`: whenever a new `uses` string or parameter ships, update both the schema and these descriptions in the same change.
