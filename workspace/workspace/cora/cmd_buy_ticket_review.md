# Code Review: cmd_buy_ticket (lines 1088-1116)

**Reviewer:** Cora
**Date:** 2026-02-26
**File:** main.cpp
**Lines Reviewed:** 1088-1116 (with full function context 941-1117)

## Assignment Verification

✅ **PRIMARY OBJECTIVE: Verify saveOrderCounter() after EVERY createOrder() call**

## Code Summary

The reviewed section (lines 1088-1116) represents the final decision logic of `cmd_buy_ticket()`, which handles ticket purchase based on seat availability and queueing preferences.

## Implementation Analysis

### createOrder() and saveOrderCounter() Verification

**Found 3 createOrder() calls, all properly paired with saveOrderCounter():**

1. **Line 1091-1092**: Success path when seats are available and reserved
   - `createOrder(username, trainID, start_date, from_station, to_station, from_idx, to_idx, ticket_count, total_price, 's');`
   - **Line 1093**: `saveOrderCounter();` ✅

2. **Line 1098-1099**: Queue path when reservation fails but queuing allowed
   - `createOrder(username, trainID, start_date, from_station, to_station, from_idx, to_idx, ticket_count, total_price, 'p');`
   - **Line 1100**: `saveOrderCounter();` ✅

3. **Line 1108-1109**: Queue path when not enough seats but queuing allowed
   - `createOrder(username, trainID, start_date, from_station, to_station, from_idx, to_idx, ticket_count, total_price, 'p');`
   - **Line 1110**: `saveOrderCounter();` ✅

**RESULT: All createOrder() calls correctly followed by saveOrderCounter()**

## Logic Flow Review

The code implements this decision tree:

```
if (available >= ticket_count)
    if (reserveSeats succeeds)
        → Create SUCCESS order ('s'), save counter, output price
    else if (allow_queue)
        → Create PENDING order ('p'), save counter, output "queue"
    else
        → Fail (return -1)
else if (allow_queue)
    → Create PENDING order ('p'), save counter, output "queue"
else
    → Fail (return -1)
```

**Logic Correctness:** ✅ CORRECT
- Properly handles all combinations of seat availability and queue preference
- Correctly distinguishes between successful reservation ('s') and queued orders ('p')
- Appropriate error handling with return -1

## Additional Code Quality Review

Reviewed entire function (lines 941-1117) for context:

### Parameter Validation (951-965)
- ✅ Null checks for required parameters
- ✅ Ticket count validation (> 0)
- ✅ Queue flag parsing

### Business Logic Validation (967-1076)
- ✅ User login check (968-971)
- ✅ Train existence and release status (974-983)
- ✅ Station validation with from < to check (986-1000)
- ✅ Date calculations with boundary handling (1002-1065)
- ✅ Sale date range validation (1068-1070)
- ✅ Ticket count vs train capacity validation (1074-1076)
- ✅ Price calculation (1078-1083)

### Date Arithmetic (1008-1065)
Complex but correct logic for calculating train start_date from departure_date:
- Properly accounts for travel times and stopovers
- Handles month boundaries correctly
- Validates date ranges (months 6-8)
- Uses iterative adjustment to match departure date precisely

## Bugs Found

**NONE**

## Issues Found

**NONE**

## Recommendations

**NO CHANGES NEEDED**

The code is correct as implemented:
- All createOrder() calls are properly paired with saveOrderCounter()
- Logic structure handles all edge cases correctly
- Date calculations are complex but correct
- Error handling is appropriate

## Conclusion

The reviewed code section (lines 1088-1116) and the entire cmd_buy_ticket function meet quality standards. The primary objective is satisfied: every createOrder() call is immediately followed by saveOrderCounter().
