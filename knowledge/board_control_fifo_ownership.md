# Board Control FIFO Buffer Ownership (Phase B)

**Status:** Active (2025-11-21)
**Related PR:** #TBD - "Move FIFO buffer ownership into board_control (phase B)"

## Overview

Phase B of the board_control refactoring moves FIFO sample buffer ownership and memory management from `DigitShowDSTDoc.cpp` into the `board_control` layer. This provides clearer separation of concerns and enables future optimizations.

## What Changed

### Before (Phase A)

- FIFO buffers (`pSmplData`) were allocated/freed in `CDigitShowDSTDoc::Allocate_Memory()`
- Used `HeapAlloc`/`HeapFree` with `hHeap` handles stored as static variables in Doc
- `DefWindowProc` in View wrote directly to `pSmplData` using pointer arithmetic
- `SaveToFile2()` in Doc read directly from `pSmplData`

### After (Phase B)

- FIFO buffers are now owned by `board_control` layer
- Internal storage uses `std::unique_ptr<LONG[]>` with automatic lifetime management
- Thread-safe access via `std::mutex` protecting concurrent read/write
- Clean API for buffer operations:
  - `AllocateFifoBuffer(boardIndex, totalSamples, channels)` - allocate
  - `FreeFifoBuffer(boardIndex)` - deallocate
  - `StoreFifoSample(boardIndex, sampleIndex, channelData)` - write samples
  - `FetchFifoSamplesTo(boardIndex, startIndex, count, dest)` - read samples
  - `SaveFifoToFile(basePath, startIndex, count, metadata)` - direct file export

## Buffer Layout

Internal buffer layout matches legacy format:
- Size: `totalSamples * channels * sizeof(LONG)`
- Channels = `AdChannels[boardIndex] / 2` (physical channels, not ADC channels)
- Sample stride: `channels` LONGs per sample
- Board-specific buffers: 2 boards supported (indices 0 and 1)

## Thread Safety

The `std::mutex s_fifoMutex` protects:
1. **Write path:** `DefWindowProc` → `StoreFifoSample()` during acquisition
2. **Read path:** `SaveToFile2()` or `OnBUTTONWriteData` → `FetchFifoSamplesTo()` or `SaveFifoToFile()`

Prevents race conditions between concurrent acquisition and file export.

## File Format Compatibility

No changes to output file format:
- TSV files maintain same headers and column order
- `SaveToFile2()` preserves timestamp reconstruction logic using `FifoStartTime` and `SavingClock`
- `SaveFifoToFile()` helper uses identical format when called directly

## Migration Notes

### Updated Files

1. **src/board_control.hpp / src/board_control.cpp**
   - Added FIFO buffer management API
   - Internal `FifoBuffer` struct with `std::unique_ptr<LONG[]>`
   - Mutex for thread safety

2. **src/DigitShowDSTDoc.cpp**
   - `Allocate_Memory()` now calls `board_control::AllocateFifoBuffer/FreeFifoBuffer`
   - `SaveToFile2()` uses `board_control::FetchFifoSamplesTo()` instead of direct `pSmplData` access
   - Removed `hHeap` static variable

3. **src/DigitShowDSTView.cpp**
   - `DefWindowProc` AIOM_AIE_DATA_NUM handler uses `board_control::StoreFifoSample()`
   - Button handlers (`OnBUTTONStartSave`, `OnBUTTONStopSave`, `OnBUTTONWriteData`) unchanged - flow preserved

4. **Board.hpp**
   - `pSmplData` marked as deprecated with comment
   - Kept for ABI compatibility but should not be used in new code

### API Usage Examples

```cpp
// Allocate buffer (OnBUTTONStartSave)
for (size_t i = 0; i < dsb::NUMAD; i++) {
    auto result = board_control::AllocateFifoBuffer(i, TotalSamplingTimes, AdChannels[i] / 2);
    if (!result) {
        spdlog::error("Failed to allocate buffer: {}", result.error());
    }
}

// Store sample during acquisition (DefWindowProc)
std::vector<LONG> channelData = {...};
auto result = board_control::StoreFifoSample(boardIndex, sampleIndex, std::span{channelData});

// Fetch for processing (SaveToFile2)
std::vector<LONG> tempBuffer(numChannels);
auto result = board_control::FetchFifoSamplesTo(boardIndex, sampleIndex, 1, tempBuffer.data());

// Free buffer (OnBUTTONWriteData via Allocate_Memory with Flag_SaveData=false)
board_control::FreeFifoBuffer(boardIndex);
```

## Future Enhancements

With FIFO buffers now in board_control:
- Could enable zero-copy direct-to-file streaming
- Easier to add circular buffer support
- Could optimize with SIMD/batch processing
- Cleaner integration with hardware DMA if supported

## Testing Checklist

- [ ] FIFO capture with OnBUTTONStartSave → OnBUTTONStopSave → OnBUTTONWriteData
- [ ] Verify TSV output matches pre-Phase-B format (headers, columns, timestamps)
- [ ] Multiple capture cycles without memory leaks
- [ ] Concurrent acquisition and file save (should be thread-safe)
- [ ] Buffer overflow handling (CurrentSamplingTimes >= TotalSamplingTimes)
- [ ] Both single-board and dual-board configurations

## References

- [board_control.hpp](../board_control.hpp) - API declarations
- [board_control.cpp](../board_control.cpp) - Implementation
- [data_file_formats.md](./data_file_formats.md) - TSV format specification
- Phase A PR: #93 - Board control layer foundation
