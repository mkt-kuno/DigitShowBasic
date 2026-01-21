# Step Completion Safety Analysis (2026-01-01)

**Status:** Completed Implementation

**Related Issue:** PR #157 (voltage reset on control exit)

## Problem Statement

The original `execute_control_step()` implementation unconditionally reset `control_output` to zero values when a step completed:

```cpp
// OLD CODE (line 297)
if (step_completed)
{
    control::step_elapsed = decltype(control::step_elapsed){};
    control::current_step_index++;
    control_output = ControlOutput{};  // ✗ Dangerous reset
}
```

### Why This Is Dangerous

1. **Abrupt pressure loss (EP cells):** Resetting `control_output` to `{0.0, 0.0, 0.0, ...}` during step transition causes:
   - Air cylinders mounted on the upper shear box to suddenly rise
   - Potential specimen rupture (splitting/tearing)
   - Damage to peripheral equipment caught in gaps

2. **Loss of output state:** `control_output` is a `static` variable that tracks the current hardware state across ticks. Resetting it breaks continuity.

3. **Design violation:** Each step should own its output transitions, not rely on a global reset.

## Safety Principle: "Safe Stop = State Maintenance"

For a hardware control system:

| Component | Safe Stop Behavior |
|-----------|-------------------|
| **Motor** | Stop rotation (RPM = 0) |
| **EP Pressure** | Maintain current pressure (voltage held constant) |

This prevents damage from sudden state changes while transitioning between control steps.

## Implementation Analysis

### Current Code Flow (After Fix)

```cpp
// Line 257-261: Initial completion check
bool step_completed = evaluate_completion(control_output);
if (step_completed)
{
    control_output.motor_rpm = 0.0;  // ✓ Motor stops gracefully
    // EP pressure unchanged - maintained at last commanded value
}

// Line 267-285: Strategy execution (if not completed or side effects needed)
if (!step_completed || needs_side_effects)
{
    control_output = current_strategy(ctx);
    // D/A outputs updated with strategy result
}

// Line 291-300: Step transition (FIXED)
if (step_completed)
{
    spdlog::info("Control step {} ({}) completed, advancing to next step", ...);
    control::step_elapsed = decltype(control::step_elapsed){};
    control::current_step_index++;
    // ✓ control_output preserved - NO RESET
}
```

### Next Step Continuity Verification

When the next step begins execution, `execute_control_step()` re-enters with:

```cpp
// Line 181-184: Re-initialize from current hardware state
control_output.front_ep_kpa = fromVoltage(DAVout[CH_EP_Cell_f], ...);
control_output.rear_ep_kpa = fromVoltage(DAVout[CH_EP_Cell_r], ...);
control_output.motor_rpm = fromIISMotorVoltage(DAVout[CH_Motor], ...);
```

**Key insight:** The next step reads the **actual voltage values** from `DAVout[]` (the hardware output register), not the reset `control_output`. This ensures smooth initialization even though `static control_output` is preserved.

### Strategy Factory Validation

```cpp
// Line 210-216: Strategy creation per step
if (strategy_step_index != control::current_step_index)
{
    current_strategy = control::create_strategy(current_step.parameters.pattern, ctx);
    strategy_step_index = control::current_step_index;
}
```

- A fresh strategy is created for each new step ✓
- The strategy factory (`src/control/strategy_factory.cpp`) handles initialization for each control pattern ✓
- All strategy implementations initialize state from `ctx.control_output` ✓

## Risk Assessment

### Eliminated Risks
- ✓ Abrupt pressure loss during step transition
- ✓ Specimen rupture from sudden EP cylinder movement
- ✓ Equipment damage from rapid state changes
- ✓ Loss of output continuity

### Maintained Safeguards
- ✓ Motor stops (RPM = 0.0) when step completes
- ✓ Next step's strategy defines its own output behavior
- ✓ Hardware state (`DAVout[]`) is the source of truth
- ✓ `static control_output` cache is consistent with hardware

### Validation Requirements
- [ ] Hardware test: Verify EP pressure remains stable during step transitions
- [ ] Hardware test: Confirm motor stops without jerk
- [ ] Integration test: Multi-step control sequences (cyclic, creep, etc.)
- [ ] Edge case: Last step → end of control sequence (verify shutdown)

## Code Changes

**File:** `src/digitshow_operations.cpp`
**Lines:** 291-300

**Removed:**
```cpp
control_output = ControlOutput{};  // Dangerous reset
```

**Added:**
```cpp
// Note: control_output is preserved to maintain hardware state continuity.
// The next step will read current DAVout values (line ~181) to initialize itself,
// ensuring smooth transition without abrupt voltage changes that could damage equipment.
// Motor is already stopped (motor_rpm = 0.0 at line 259), and EP pressure is maintained.
```

## Related Documentation
- `control_mechanisms_overview.md` — Timer 2 dispatch and strategy architecture
- `control_specifications.md` — JSON → CFNUM mapping and control patterns
- `knowledge/voltage_reset_on_exit.md` — Shutdown voltage behavior (different from step transitions)

## Decision Rationale

This fix prioritizes **safety over simplicity**. Rather than relying on a "reset to safe defaults" pattern that is inherently brittle in hardware systems, we now leverage the actual hardware state (`DAVout[]`) as the source of truth for initialization. This is more robust and aligns with real-world practice: gradual state changes are always safer than abrupt resets in physical systems.
