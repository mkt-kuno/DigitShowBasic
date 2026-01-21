# Voltage Reset on Program Exit

**Status:** Active (2025-12-19)
**Related PR:** #TBD - "Add 0V voltage reset on program termination"

## Overview

When the DigitShowDST application exits, all D/A voltage outputs are now automatically reset to 0V. This ensures that devices controlled via the D/A converter (such as motors, pneumatic actuators, etc.) are safely stopped when the program closes.

## Problem

Previously, when the program terminated:
- Only `CloseBoard()` was called (which closes the hardware connection)
- No voltage reset was performed for D/A channels
- Devices would continue operating at their last commanded voltage
- Only the `Stop_Control()` function reset some channels (CH_Motor, CH_MotorSpeed), but this was only called when explicitly stopping control, not on program exit

## Solution

A new function `ResetAllVoltageOutputs()` has been added to the shutdown sequence:

1. **board_control::ResetAllVoltageOutputs()** - Sets all DAVout[] channels to 0.0f and writes them to hardware
2. **CDigitShowDSTDoc::ResetAllVoltageOutputs()** - Wrapper that calls the board_control layer function
3. **~CDigitShowDSTView()** - Destructor now calls `digitshow::shutdown_boards()` which internally resets voltages before closing boards (PR #135)

## Implementation Details

### Shutdown Sequence

The View destructor (`~CDigitShowDSTView()`) now executes:

```cpp
CDigitShowDSTView::~CDigitShowDSTView()
{
    // Reset all voltage outputs to 0V before closing boards to stop all devices
    GetDocument()->ResetAllVoltageOutputs();
    GetDocument()->CloseBoard();
}
```

### Voltage Reset Function

```cpp
std::expected<void, std::string> board_control::ResetAllVoltageOutputs() noexcept
{
    // Check if board is initialized
    if (!Flag_SetBoard) {
        return {};  // Skip if offline mode
    }

    // Reset all 8 D/A channels to 0.0f
    for (size_t i = 0; i < MAX_DA_CHANNELS; ++i) {
        DAVout[i] = 0.0f;
    }

    // Write 0V to hardware
    return WriteAnalogOutputs();
}
```

## Channels Affected

All 8 D/A channels are reset to 0V:
- CH_Motor (0) - Motor On/Off
- CH_MotorCruch (1) - Motor Clutch (Loading/Unloading)
- CH_MotorSpeed (2) - Motor Speed
- CH_EP_Cell_f (3) - Front Bellofram Pressure
- CH_EP_Cell_r (4) - Rear Bellofram Pressure
- Channels 5-7 - Reserved/Future use

## Offline Mode Behavior

The function checks `Flag_SetBoard` before attempting hardware operations:
- If boards are not initialized (offline mode), the function returns successfully without error
- This allows the program to close cleanly even without hardware connected

## Logging

The function logs its actions using spdlog:
- Info: "Resetting all D/A voltage outputs to 0V"
- Debug: "Board not initialized (Flag_SetBoard is false), skipping voltage reset" (offline mode)
- Error: "Failed to reset voltage outputs: {error}" (hardware error)
- Info: "All D/A voltage outputs reset to 0V successfully"

## Safety Considerations

1. **Graceful Shutdown**: Ensures devices stop safely when program exits normally
2. **Emergency Stop**: The function is called even during error conditions in the destructor
3. **Hardware Protection**: Prevents devices from continuing to run uncontrolled after program exit
4. **Offline Compatibility**: Works correctly in both online (hardware connected) and offline (testing) modes

## Testing Checklist

- [ ] Normal program exit resets all voltages to 0V
- [ ] Devices stop operating after program closure
- [ ] Offline mode (no hardware) exits cleanly without errors
- [ ] Hardware errors during voltage reset are logged but don't prevent program closure
- [ ] Function works with both single-board and dual-board configurations

## Related Files

- `src/board_control.hpp` - Function declaration
- `src/board_control.cpp` - Implementation
- `src/DigitShowDSTDoc.h` - Doc wrapper declaration
- `src/DigitShowDSTDoc.cpp` - Doc wrapper implementation
- `src/DigitShowDSTView.cpp` - Destructor that calls the reset function
- `src/Variables.hpp` - DAVout array and MAX_DA_CHANNELS constant

## Future Enhancements

Potential improvements for consideration:
- Add timeout/retry logic for hardware write failures
- Log the previous voltage values before reset for diagnostics
- Configurable shutdown behavior (e.g., ramp down instead of immediate 0V)
- Integration with emergency stop button or safety interlock system
