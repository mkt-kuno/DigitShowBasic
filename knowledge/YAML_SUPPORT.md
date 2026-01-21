# YAML Support for Configuration Files

## Overview

DigitShowDST now supports both JSON and YAML formats for calibration factors and control scripts. Users can seamlessly load and save configuration files in either format through the UI dialogs.

## Supported File Extensions

- **JSON**: `.json`
- **YAML**: `.yml`, `.yaml`

## Features

### Automatic Format Detection

The system automatically detects the file format based on the file extension:
- `.json` files are parsed as JSON
- `.yml` and `.yaml` files are parsed as YAML
- Unknown extensions default to JSON

### Unified Loading

Both calibration factor and control script dialogs can load files in either format:
- **Calibration Factor Dialog**: Click "Load" button and select `.json`, `.yml`, or `.yaml` files
- **Control Script Dialog**: Click "Load" button and select `.json`, `.yml`, or `.yaml` files

### Format-Specific Saving

When saving, the file format is automatically determined by the chosen file extension in the save dialog:
- Save as `.json` → JSON format
- Save as `.yml` or `.yaml` → YAML format

### Data Preservation

Round-trip conversion between JSON and YAML preserves all data:
- Load a JSON file → Save as YAML → No data loss
- Load a YAML file → Save as JSON → No data loss

## Usage Examples

### Calibration Factor Files

**JSON Format** (`calibration.json`):
```json
{
  "calibration_data": [
    {
      "channel": 0,
      "cal_a": 0.0,
      "cal_b": 1.0,
      "cal_c": 0.0
    }
  ],
  "initial_specimen": {
    "height_mm": 120.0,
    "area_mm2": 14400.0,
    "weight_g": 0.0,
    "box_weight_g": 10000.0
  }
}
```

**YAML Format** (`calibration.yaml`):
```yaml
calibration_data:
  - channel: 0
    cal_a: 0.0
    cal_b: 1.0
    cal_c: 0.0
initial_specimen:
  height_mm: 120.0
  area_mm2: 14400.0
  weight_g: 0.0
  box_weight_g: 10000.0
```

### Control Script Files

**JSON Format** (`control.json`):
```json
{
  "$schema": "../schemas/control_script.schema.json",
  "steps": [
    {
      "name": "Initial consolidation",
      "use": "constant_tau_consolidation",
      "with": {
        "direction": "compression",
        "motor_rpm": 50.0,
        "tau_kPa": 5.0,
        "consolidation_rate_kPa_per_min": 20.0,
        "target_sigma_kPa": 60.0
      }
    }
  ]
}
```

**YAML Format** (`control.yaml`):
```yaml
steps:
  - name: Initial consolidation
    use: constant_tau_consolidation
    with:
      direction: compression
      motor_rpm: 50.0
      tau_kPa: 5.0
      consolidation_rate_kPa_per_min: 20.0
      target_sigma_kPa: 60.0
```

## Implementation Details

### Core Functions

The YAML support is implemented through three main functions in `control/json.hpp`:

1. **`DetectFormat(filepath)`**: Detects file format from extension
2. **`LoadConfigFile(filepath)`**: Loads JSON or YAML file into a ryml tree
3. **`SaveConfigFile(filepath, tree, format)`**: Saves ryml tree as JSON or YAML

### File Dialog Filters

Both dialogs now use unified file filters:
```
Config Files (*.json;*.yml;*.yaml)|*.json;*.yml;*.yaml|
JSON Files (*.json)|*.json|
YAML Files (*.yml;*.yaml)|*.yml;*.yaml|
All Files (*.*)|*.*||
```

### Technology Stack

- **RapidYAML (ryml)**: Used for both JSON and YAML parsing/serialization
- **No additional dependencies**: RapidYAML was already in `vcpkg.json`
- **Unified API**: Same ryml tree structure for both formats

## Benefits

### For Users

- **Human-readable YAML**: Easier to read and edit by hand
- **JSON compatibility**: Continue using existing JSON files
- **Flexible workflows**: Convert between formats as needed
- **No retraining needed**: Same UI, just pick the format you prefer

### For Developers

- **Single code path**: One parser handles both formats
- **Type-safe**: Same validation logic applies to both formats
- **Maintainable**: Centralized format handling in `control/json.hpp`
- **Extensible**: Easy to add new formats if needed

## Testing

Comprehensive test suite (`test_yaml_support.cpp`) covers:
- Format detection for all extensions
- JSON file loading
- YAML file loading
- JSON file saving
- YAML file saving
- JSON ↔ YAML round-trip conversion
- Calibration data format
- Control script format

## Sample Files

Example files are provided in `test_data/`:
- `sample_calibration.json` / `sample_calibration.yaml`
- `sample_control.json` / `sample_control.yaml`

## Migration Guide

### From JSON to YAML

1. Open your existing JSON file in the application
2. Click "Save" and choose a `.yaml` extension
3. The file is automatically converted to YAML format

### From YAML to JSON

1. Open your YAML file in the application
2. Click "Save" and choose a `.json` extension
3. The file is automatically converted to JSON format

## Schema Support

- JSON files can reference the schema via `$schema` field
- YAML files don't require schema reference (but the same schema applies)
- VS Code provides autocomplete for JSON files with schema
- YAML validation follows the same schema rules

## Known Limitations

- Schema reference (`$schema`) is preserved only in JSON format
- YAML comments are not preserved when converting to JSON and back
- File format is determined solely by extension (no content detection)

## Future Enhancements

- Add YAML schema support for IDE integration
- Support for YAML-specific features (anchors, aliases)
- Automatic backup when converting between formats
- Batch conversion tool for multiple files

## References

- JSON Schema: `schemas/calibration_factor.schema.json`, `schemas/control_script.schema.json`
- Control specifications: `knowledge/control_specifications.md`
- YAML migration guide: `knowledge/ctl_to_yaml_migration.md`
