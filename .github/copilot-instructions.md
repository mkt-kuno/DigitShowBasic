# Copilot Instructions for DigitShowDST

## System Overview
DigitShowDST is a **legacy MFC single-document/single-view C++ application** for controlling a direct-shear test machine. Architecture is split by concern (PR #16):
- **`DigitShowDSTDoc`** (in [src/DigitShowDSTDoc.h](../src/DigitShowDSTDoc.h)): Manages hardware initialization, TSV logging, control script execution.
- **`DigitShowDSTView`** (in [src/DigitShowDSTView.h](../src/DigitShowDSTView.h)): Owns UI timers, refreshes display arrays, routes button clicks to Dialog handlers.
- **Three timer loops** drive the system:
  - **Timer 1 (50 ms)**: UI refresh cycle. Calls `digitshow::read_analog_inputs()` → `Cal_Physical()` → `variables::physical::update()`. Results stored in `variables::physical::latest_physical_input` snapshots.
  - **Timer 2 (500 ms)**: Closed-loop control. Calls `control::execute()` to dispatch `CFNUM`-indexed control helpers and issue D/A writes via `variables::set_output_voltages()`.
  - **Timer 3 (default 1000 ms)**: Periodic logging via `SaveToFile()` (TSV) and optional SQLite writes; FIFO exports use `SaveToFile2()`.

## Hardware & Calibration
- **CONTEC board access** lives in [src/Caio.h](../src/Caio.h). Device IDs: `AIO000/1/2`; `AioSetAiRangeAll(..., 0)` for ±10 V; `AioMultiAo` for batch analog output.
- **Analog input wiring** (every other channel): Index using `AdChannels[i] / 2` and stride `2*j` (see [src/digitshow_operations.hpp](../src/digitshow_operations.hpp) and [src/physical_variables.cpp](../src/physical_variables.cpp)).
- **Calibration** is **quadratic on A/D** (`Cal_a`, `Cal_b`, `Cal_c` in [src/Variables.hpp](../src/Variables.hpp)), **linear on D/A** (`DA_Cal_a`, `DA_Cal_b`). Dialogs in `src/CalibrationAmp.cpp`, `src/CalibrationFactor.cpp` update persistence; arrays in `variables::` namespace hold runtime state.
- **Offline runs** must guard `Aio*` calls. Reuse existing `Flag_SetBoard` checks (set via Board Settings dialog) before touching hardware—agents may run without boards.

## Control Scripts & Logic
- **JSON bridge format** (active): Control dialog loads `.ctl.json` files validated against [schemas/control_script.schema.json](../schemas/control_script.schema.json). Each step's `use` field maps to a `CFNUM` (see [knowledge/control_specifications.md](../knowledge/control_specifications.md)).
- **Conversion flow**: [src/Control_File.cpp](../src/Control_File.cpp) converts JSON steps → `CFNUM`/`CFPARA[0..17]` arrays. **Timer 2** calls `control::execute()` (in [src/control/control.hpp](../src/control/control.hpp)) which dispatches based on CFNUM.
  - Each helper (e.g., `MLoading_Constant_Pressure` in [src/control/](../src/control/)) consumes `CFPARA` entries for set-points and durations.
  - When a step completes, `current_step_index` advances; Timer 2 picks up the next entry on the next tick.
- **Step completion** logic lives in [src/control/step_completion.hpp](../src/control/step_completion.hpp)—handles stroke limits, time/cycle limits, and convergence checks.
- **YAML roadmap**: Strategy extraction will enable YAML workflows with guards and richer composition; JSON remains production until that lands.
- **When editing control logic**: Update [knowledge/control_specifications.md](../knowledge/control_specifications.md) first—it documents the `use` → CFNUM mapping, parameter units, and CF index assignments.
## Configuration & Schemas
- Calibration, specimen, and control data flow through JSON manifests in `schemas/` (validated via JSON Schema Draft 7).
- Runtime state split by concern: `src/Board.hpp`, `src/Control.hpp`, `src/Sensitivity.hpp`, `src/File.hpp`, `src/Variables.hpp` (PR #16).
- VS Code integrates schema validation—add `"$schema": "../schemas/control_script.schema.json"` to JSON files for autocomplete.

## Data Logging
- **TSV (stable)**: Since 2025-10-28 the app writes `{basename}.tsv`, `{basename}_vlt.tsv`, `{basename}_out.tsv` with `UnixTime(ms)` column; older `.dat/.vlt/.out` stay readable.
- **SQLite (prototype)**: PR #18 added dual-write to `{basename}.sqlite` (tables: `samples`, `voltages`). Schema is evolving; TSV remains the primary analysis source until Python tooling catches up.
- `SaveToFile()` captures `Phyout` + the parameter columns derived from `latest_physical_input` / `latest_physical_output` every Timer 3 tick; FIFO exports (`OnBUTTONWriteData` → `SaveToFile2()`) reconstruct timestamps from `FifoStartTimeMs`.
- `_out.tsv` headers are historical—consult `knowledge/data_file_formats.md` to map indices to actual quantities before renaming or reordering columns.

## Dependencies & Toolchain
- **vcpkg manifest mode**: `vcpkg.json` declares dependencies (nlohmann-json, doctest, ryml); MSBuild integration auto-installs to `vcpkg_installed/`.
- New dependencies: add to `vcpkg.json`, then rebuild. vcpkg handles headers/libs automatically.
- Submodule at `vcpkg/` pins the toolchain version; update via `git submodule update --remote vcpkg`.

## Build & Runtime
- Build through VS Code tasks (`MSBuild: Build Release`, `MSBuild: Build Debug`), which bootstrap `Load-DevEnv.ps1` automatically.
- Manual builds: `msbuild .\DigitShowDST.vcxproj /t:Build /p:Configuration=Release` (requires VS Dev Shell initialized via `Load-DevEnv.ps1`).
- No automated test harness yet; validate by exercising the GUI. Add hardware fallbacks (check `Flag_SetBoard`) whenever touching real I/O so agents can run without boards.

## Python Tooling
- Data analysis scripts live in `scripts/`; managed via `uv` and `pyproject.toml` (e.g., `uv run .\scripts\analyze_data.py <log_dir>`).
- `scripts/load_data.py` → `read_all_files()` auto-detects TSV vs legacy headers and returns pandas DataFrames (DatetimeIndex for new logs, `Time(s)` index for old).
- Log files are tab-delimited UTF-8 with BOM—match that encoding when exporting or diffing in Python utilities. SQLite loader support is roadmapped.

## C++ Conventions (See `.github/instructions/cpp.instructions.md` for full details)
- **Qualifiers by default**: Mark functions `constexpr noexcept` where possible; parameters as `const`.
- **Error handling**: Normal errors use `std::optional<T>` or `std::expected<T, Error>` (C++23); unexpected bugs throw exceptions only.
- **Pure functions**: Separate side effects (I/O, hardware calls) from calculation logic. Example: calibration math is pure; `digitshow::write_analog_outputs()` handles hardware.
- **Modern C++**: C++17+ preferred; leverage `std::array`, `std::expected`, `std::format` over legacy patterns.
- **Scope hierarchy**: Local > `static` > `inline` > avoid `extern`. Wrap `inline` variables in namespaces.

## Message Mapping & Event Loop (MFC-Specific)
- UI events route through `BEGIN_MESSAGE_MAP()`/`END_MESSAGE_MAP()` macros in [src/DigitShowDSTView.h](../src/DigitShowDSTView.h).
- `OnBUTTON*` handlers are wired to button clicks via `ON_BN_CLICKED()` macros; `OnTimer()` dispatches based on timer ID (1, 2, or 3).
- **Timer callbacks are not preempted**: Each timer runs to completion within its slot. Avoid blocking I/O in timer handlers.
- Dialog resource IDs map to UI elements via `DDX_Text()` loops in `DoDataExchange()`—see `IDS_VOUT[]`, `IDS_PHYOUT[]`, `IDS_PARA[]` arrays for element mappings.

## Data Flow Snapshot Model
- **Timer 1** populates snapshots: `variables::physical::latest_physical_input` and `latest_physical_output` hold read-only copies.
- **UI and Timer 2** both read these snapshots—safe because Timer 1 is the only writer and runs frequently.
- **Avoid direct `Vout[]`/`Phyout[]` access** in new code; prefer typed accessors in `variables::physical::*` or consult [knowledge/control_parameter_inventory.md](../knowledge/control_parameter_inventory.md) for field semantics.

## REST API Server (Optional Integration)
- `ApiServer` (in [src/ApiServer.hpp](../src/ApiServer.hpp)) exposes endpoints for real-time sensor streaming via Server-Sent Events (SSE).
- Controlled by `api_config.json`; configure port and feature flags there.
- Thread-safe API update call in Timer 1: `api_server_->update_data(...)` receives the latest snapshots.
- Future: command endpoints will allow external systems to trigger control steps.

## Knowledge Base (`knowledge/`)
- **Start here** when touching control dispatch, configuration formats, logging, or hardware safety. Each document is kept current; older plans were removed to reduce drift.
- Key guides: [knowledge/control_specifications.md](../knowledge/control_specifications.md) (JSON/YAML → CFNUM mapping), [knowledge/control_mechanisms_overview.md](../knowledge/control_mechanisms_overview.md) (Timer 2 dispatch), [knowledge/control_parameter_inventory.md](../knowledge/control_parameter_inventory.md) (field semantics), [knowledge/data_file_formats.md](../knowledge/data_file_formats.md) (TSV columns), [knowledge/tsv_unix_time_migration.md](../knowledge/tsv_unix_time_migration.md) (timestamping).
- Safety & runtime: [knowledge/step_completion_safety_analysis_2026-01-01.md](../knowledge/step_completion_safety_analysis_2026-01-01.md), [knowledge/board_control_fifo_ownership.md](../knowledge/board_control_fifo_ownership.md), [knowledge/voltage_reset_on_exit.md](../knowledge/voltage_reset_on_exit.md), [knowledge/timer_callback_vs_wmtimer.md](../knowledge/timer_callback_vs_wmtimer.md).
- API: [knowledge/rest_api_quickstart.md](../knowledge/rest_api_quickstart.md) for SSE/REST usage.
- Update the relevant doc when changing behavior—avoid creating new memos that scatter guidance.

## Key References
- Core flows: [src/DigitShowDSTDoc.cpp](../src/DigitShowDSTDoc.cpp) (hardware, logging), [src/DigitShowDSTView.cpp](../src/DigitShowDSTView.cpp) (timers/UI), [src/control/control.hpp](../src/control/control.hpp) (dispatch).
- Dialog resource file: [src/DigitShowDST.rc](../src/DigitShowDST.rc); menu/accelerator definitions in [src/MainFrm.cpp](../src/MainFrm.cpp).
- Deep dives: [knowledge/control_mechanisms_overview.md](../knowledge/control_mechanisms_overview.md), [knowledge/data_file_formats.md](../knowledge/data_file_formats.md)