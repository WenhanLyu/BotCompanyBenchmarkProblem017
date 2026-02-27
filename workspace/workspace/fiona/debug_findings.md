# Bug #48 Debug Findings

## Summary
Found ROOT CAUSE of seat availability bugs through systematic logging approach.

## Key Evidence

### 1. Seat Data Corruption Pattern
Test 3 final state for LeavesofGrass 06-28:
- Segments 3-5: [55258, 27765, 8268]

Test 4 first read (same train/date):
- Segments 0-10: [82874, 67514, 67514, **55258, 27765, 8268**, 30580, 20206, 21364, 32524, 16706]

**Segments 3-5 match EXACTLY**, but all other segments are GARBAGE!

### 2. Initialization vs Runtime
- Initialization: ALL segments correctly set to train.seatNum (98306)
- Test 1 operations: Work correctly, values properly decremented
- Test 4 (after file persistence): PARTIAL corruption

### 3. Root Cause
**B+ Tree File I/O is CORRUPTING the SeatAvailability structure!**

The structure has:
- `char trainID[25]` - 25 bytes
- `Date date` - 2 bytes
- `int available[99]` - 396 bytes
- Total: 423 bytes

But C++ structure PADDING causes `sizeof(SeatAvailability)` to be 432 bytes (confirmed by test).

**The issue**: FileIO uses `sizeof(T)` which includes padding bytes. When data is written/read, the padding contains UNINITIALIZED MEMORY, which gets mixed into the actual data on subsequent reads.

### 4. Why Only Segments 3-5 are Correct?
The corruption is NOT random - it's deterministic based on memory layout. The fact that consecutive segments 3-5 survive suggests that this portion of the array aligns properly, while other parts overlap with padding bytes.

## The Fix
The SeatAvailability structure needs to be **packed** to eliminate padding:

```cpp
struct __attribute__((packed)) SeatAvailability {
    char trainID[25];
    Date date;
    int available[99];
};
```

OR implement custom serialization that writes/reads only the meaningful bytes.

## Impact
This bug causes:
1. **Bug #1 (Test 4, line 1418)**: Order shows [success] instead of [pending]
   - Reason: inflated seat availability (31454 instead of actual value)
   - System thinks enough seats available when they're not

2. **Bug #2 (Test 5, line 1559)**: buy_ticket outputs price instead of "queue"
   - Same root cause: corrupted seat data makes system think seats are available

## Time Spent
- Phase 1 (Logging): 30 min
- Phase 2 (Analysis): 45 min
- Total: ~1.25 hours

## Next Steps
1. Add `__attribute__((packed))` to SeatAvailability
2. Rebuild and test
3. Verify both bugs are fixed
