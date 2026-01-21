# Knowledge Base Overview (2026-01-21)

## Current Foundation
- Control, calibration, and specimen data flow through JSON/YAML manifests validated by the schemas in `schemas/`.
- Runtime globals are split by concern (`src/Board.hpp`, `src/Control.hpp`, `src/Sensitivity.hpp`, `src/File.hpp`, `src/Variables.hpp`).
- Logging writes TSV by default; SQLite support exists but TSV remains the reference format described in `data_file_formats.md` and `tsv_unix_time_migration.md`.
- Third-party dependencies are managed by `vcpkg.json`; builds assume vcpkg manifest mode.

## Active References
- **Control Scripts**: `control_specifications.md` (JSON/YAML control format + `use` → CFNUM mapping); `control_mechanisms_overview.md` (Timer 2 dispatch flow); `control_parameter_inventory.md` (field semantics).
- **Logging & Timing**: `data_file_formats.md`, `tsv_unix_time_migration.md`, `timer_callback_vs_wmtimer.md`, `step_completion_safety_analysis_2026-01-01.md`.
- **Hardware**: `board_control_fifo_ownership.md`, `voltage_reset_on_exit.md`.
- **API**: `rest_api_quickstart.md` (SSE/REST usage).
- **Testing/Tooling**: `doctest_integration.md`.

## How To Use This Folder
- Prefer updating an existing document rather than adding a new memo; the goal is to keep guidance discoverable and current.
- If a document you need is missing after this cleanup, check git history instead of recreating it.
