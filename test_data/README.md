# Test YAML Configuration Files

This directory contains test fixtures for YAML control configuration validation.

## Test Cases

### Valid Configurations
- `valid_minimal.yaml`: Minimal valid document with single no_control step
- `valid_complex.yaml`: Multi-step workflow covering all strategy types
- `valid_pre_consolidation.yaml`: Pre-consolidation workflow example
- `valid_consolidation_sweep.yaml`: Complex consolidation with cyclic loading

### Invalid Configurations
- `invalid_missing_version.yaml`: Missing version field
- `invalid_wrong_version.yaml`: Unsupported version number
- `invalid_missing_metadata.yaml`: Missing required metadata fields
- `invalid_duplicate_step_ids.yaml`: Duplicate step IDs
- `invalid_unknown_strategy.yaml`: Unknown strategy type
- `invalid_missing_params.yaml`: Missing required parameters
- `invalid_unit_suffix.yaml`: Unsupported unit suffix
- `invalid_out_of_range.yaml`: Parameter value outside allowed range
- `invalid_hold_and_repeat.yaml`: Both hold and repeat specified

## Usage

These test files are used for:
1. Parser regression testing during development
2. Schema validation verification
3. Documentation examples for migration guide
4. User reference for YAML format

## Validation

Run validation tests using:
```
# Manual testing (once parser is integrated)
DigitShowDST.exe --validate-config test_data\valid_minimal.yaml
```

Automated tests will be added when test infrastructure is established.
