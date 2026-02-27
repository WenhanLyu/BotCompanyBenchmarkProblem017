# Issue #48 Bug Analysis - Leo's Investigation

## Current Status
- **Tests 1-3**: ✓ PASS (60% pass rate)
- **Test 4**: ✗ FAIL (Bug #1)
- **Test 5**: ✗ FAIL (Bug #2)

## Bug #1: Order Status Incorrect (Test 4, Line 1418)
- **Expected**: `[pending] LeavesofGrass 河南省灵宝市 06-28 23:45 -> 河南省偃师市 06-29 03:28 346 15759`
- **Actual**: `[success] LeavesofGrass 河南省灵宝市 06-28 23:45 -> 河南省偃师市 06-29 03:28 346 15759`
- **Impact**: Order shows as fulfilled when it should remain pending
- **Root Cause**: processStandbyQueue incorrectly fulfills pending orders

## Bug #2: buy_ticket Output Incorrect (Test 5, Line 1559)
- **Expected**: `queue`
- **Actual**: `15169000` (a price value)
- **Impact**: buy_ticket succeeds when it should be queued
- **Root Cause**: checkAvailableSeats reports more seats than actually exist

## Investigation Summary

### Areas Examined
1. ✓ `checkAvailableSeats()` - Loop logic appears correct
2. ✓ `reserveSeats()` - Segment reservation logic correct
3. ✓ `releaseSeats()` - Segment release logic correct
4. ✓ `processStandbyQueue()` - Already filters by departure_date
5. ✓ `initializeSeatsForTrain()` - Initialization looks correct
6. ✓ Date calculation in buy_ticket - Fine-tuning logic seems sound

### Hypotheses Explored
1. **Off-by-one in checkAvailableSeats loop**: Changed loop to start at from_idx with INT_MAX initialization - NO EFFECT
2. **Wrong date in processStandbyQueue**: Changed to use order.departure_date - NO EFFECT (already filtered)
3. **Date calculation inconsistency**: Traced through examples, logic appears correct

### Remaining Suspects
The bugs persist despite code appearing correct. Possible causes:
1. **Date calculation edge case**: Some specific combination of train start time, station position, and departure date produces wrong start_date
2. **B+ tree corruption**: Seat data might be corrupted during insert/update operations
3. **Floating point/rounding issue**: Integer division in date calculation might have edge cases
4. **Month boundary bug**: Date arithmetic across month boundaries might have subtle bug

### Next Steps
1. Add debug logging to trace actual start_dates calculated for failing orders
2. Compare start_dates for different orders on same train run
3. Examine B+ tree insert/update logic for potential corruption
4. Test with simplified scenarios to isolate the bug

## Files Modified
- `order_system.hpp`: Attempted fixes (reverted after no effect)

## Time Spent
- ~2 hours of investigation
- Multiple hypotheses tested
- Bugs remain unfixed

## Recommendation
Given the complexity and time spent, consider:
1. Escalating to manager for additional eyes
2. Using a debugger to trace actual execution
3. Creating minimal reproduction case
4. Reviewing commit history for related changes
