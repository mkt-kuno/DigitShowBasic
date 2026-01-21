# Control Mechanisms Overview

## Summary

DigitShowDST currently implements control through scripted CFNUM sequences and specimen state operations. **The YAML migration unifies all control operations into a single scripted workflow model**, where every operation—whether a complex loading sequence or specimen state capture—is represented as a step in a YAML control script. Manual voltage passthrough has been removed (2025-11-02).
# Control Mechanisms Overview

**Status:** Active (JSON bridge, 2025-11-02)

This sheet explains how Timer 1 and Timer 2 glue configuration files to the legacy control helpers. It replaces the YAML-first write-up from 2025-10; the YAML workflow will return once the strategy layer is extracted, but the behaviour described here matches the shipping build.

## Runtime Flow (Timer 2)
1. `DigitShowDSTView` fires Timer 2 every 500 ms.
2. `control::execute()` runs, reading `CFNUM[current_step_index]` and `CFPARA[current_step_index][0..17]`.
3. The switch statement dispatches to helpers in `Control_*.cpp` (monotonic loading, cyclic loading, consolidation, creep, etc.). Each helper:
   - Consumes `CFPARA` entries for set-points, durations, and overrides.
- Adjusts globals (`Vout`, `DAvout`, `current_step_index`, `step_elapsed`) and triggers D/A writes.
- Advances `current_step_index` when the step finishes; Timer 2 picks up the next entry on the following tick.

The JSON control script dialog (`src/Control_File.cpp`) is the authoritative bridge. It loads `steps[].use` entries, converts them to `CFNUM`/`CFPARA`, and writes them back to JSON on save. See `control_specifications.md` for the per-step field mapping.

## Mechanisms Covered by the JSON Bridge

| Category | Description | Implementation Notes |
|----------|-------------|----------------------|
| Scripted control (legacy Control ID 15) | CFNUM 0–19 loading/consolidation/creep routines. | All 20 codes have `use` aliases. Parameters map 1:1 via `JsonStepToCFNUM`. |
| Specimen state capture buttons | Dialog operations (`OnBUTTONBeConsol`, `OnBUTTONAfConsolidation`) updating `SpecimenData` and LVDT offsets. | Not yet scripted; future manifests may surface them, but today they are handled through the UI. |

Only the scripted CFNUM path is wired to the JSON file. Specimen state capture continues to be an ad-hoc UI operation; when the workspace manifest lands, that flow will be reconsidered. Manual voltage passthrough was removed in 2025-11-02.

## Key Translation Points
- **Direction flags:** JSON uses readable strings (`load`/`unload`, `compression`/`dilation`). `Control_File.cpp` converts them to `0` or `1` for the legacy helpers.
- **Durations:** `time_min` and `num_cycles` remain scalar doubles/integers; the helpers still track elapsed time using `step_elapsed` and Timer 2. There is no ISO-8601 or unit suffix parsing in the shipping bridge.
- **Sensitivity overrides:** JSON `overrides` map directly to `CFPARA[10..17]`. If a value is omitted, legacy global settings remain in effect for that step.
- **Validation:** `schemas/control_script.schema.json` enforces required fields but does not perform semantic checks (e.g., `tau_lower_kPa < tau_upper_kPa`). The dialog performs lightweight sanity checks; deeper validation remains a future task.

## Interaction with Other Timers
- **Timer 1 (50 ms):** Acquisition cycle calling `digitshow::read_analog_inputs()` → `Cal_Physical()` → `variables::physical::update()`. `variables::physical::update()` writes the derived values into `variables::physical::latest_physical_input` and `variables::physical::latest_physical_output`. Strategies should consume these snapshots (and `Phyout[]`) rather than the legacy `CalParam[]` array. Keep calibration docs (`calibration_factor.schema.json`, `tsv_unix_time_migration.md`) in sync.
- **Timer 3 (default 1000 ms):** Logging via `SaveToFile()`/`SaveToFile2()`, now augmented with optional SQLite writes (see `file_io_modernization.md`). Control code should avoid heavy work here to keep logging deterministic.

## Roadmap & Open Items
- **Strategy extraction:** `control_logic_extraction_targets.md` lists the helpers that are closest to pure computation after the inline-global split. These will become `IControlStrategy` implementations once Phase 2 starts.
- **YAML workflow reboot:** The previous YAML schema draft is on hold. When the strategy layer is in place, we will reintroduce YAML as the authoring format and auto-generate JSON/CFPARA as compatibility output. The JSON bridge documented here will then become an import/export path only.
- **Workspace manifest:** See `workspace_concept.md` for how calibration/specimen/control files will be bundled. Control scripts will be referenced from the manifest rather than through the existing dialog once the UI flow is updated.

## References
- `Control_File.cpp` — JSON ↔ CFNUM conversion.
- `src/control/control.hpp::execute()` — Timer 2 dispatcher.
- `Control_*.cpp` — Individual control helpers.
- `control_specifications.md` — Detailed field mapping per `use` string.
- `refactor_plan_2025-10-29.md` — Strategy extraction timeline.
- Next 10 values: Standard parameters (doubles)
