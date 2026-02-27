# Diana's Implementation Notes

## Issue #49: Fix travel_time2 Calculation Bug - FIXED ✅

### Problem
In `query_transfer` command, the `travel_time2` calculation (line 1207) incorrectly added the stopover time at the transfer station, inflating the reported travel time.

**Buggy code:**
```cpp
int travel_time2 = minutes_to_to2 - (minutes_to_transfer2_depart - (t2 > 0 ? train2.stopoverTimes[t2 - 1] : 0));
```

This simplifies to: `travel_time2 = minutes_to_to2 - minutes_to_transfer2_depart + stopover_at_transfer`

### Root Cause
- `minutes_to_transfer2_depart` already includes the stopover (it's the DEPARTURE time from transfer)
- `minutes_to_to2` is the ARRIVAL time at destination
- The formula was subtracting the stopover again, which effectively ADDED it to the travel time

### Fix Applied
Simplified to match the pattern used for `travel_time1`:
```cpp
int travel_time2 = minutes_to_to2 - minutes_to_transfer2_depart;
```

### Impact
- **Severity**: Code quality issue
- **Test Impact**: No impact on test outputs (travel_time2 is stored but not used for sorting/comparison in current test cases)
- **Correctness**: Now reports accurate travel time for the second segment of transfer journeys

### Status
✅ Fixed and tested (main.cpp:1207)

---

## refund_ticket Implementation - COMPLETE ✅

### Summary
Successfully implemented the `refund_ticket` command in main.cpp with full validation and edge case handling.

### Implementation Details

**Parameters**:
- `-u <username>` (required): User who wants to refund
- `-n <order_number>` (optional, default 1): Which order to refund (1-indexed, newest to oldest)

**Validation**:
1. ✅ User is logged in
2. ✅ User exists
3. ✅ Order number is valid (within range of user's orders)
4. ✅ Order is not already refunded (status != 'r')

**Refund Logic**:
1. Get all user orders and reverse to newest-to-oldest (matching query_order output)
2. Find the n-th order (1-indexed)
3. If order status is 's' (success), release the seats using `releaseSeats()`
4. Update order status to 'r' (refunded) using `updateOrderStatus()`
5. Process standby queue using `processStandbyQueue()` to fulfill pending orders
6. Save order counter

**Code Location**:
- Function: `cmd_refund_ticket()` at line ~1221
- Command handler: Added to main loop at line ~1394

### Test Results

**Basic Refund Test** ✅:
- Buy ticket → refund → verify status changes from [success] to [refunded]

**Edge Cases Tested** ✅:
- ✅ Refunding non-existent order (n > order count) → -1
- ✅ Refunding already refunded order → -1
- ✅ User not logged in → -1
- ✅ Multiple orders, refund specific one by position

**Standby Queue Processing** ✅:
- Train capacity: 10 seats
- User A buys 10 tickets (fills train)
- User B tries to buy 5 tickets → "queue" (pending)
- User A refunds → User B's order automatically becomes [success]

### Key Features

1. **Order Indexing**: Uses same order as `query_order` (newest to oldest)
2. **Seat Release**: Only releases seats if order was successful ('s'), not if pending ('p')
3. **Queue Processing**: Automatically fulfills pending orders when seats become available
4. **Status Validation**: Cannot refund an already refunded order

### Files Modified
- `main.cpp`: Added `cmd_refund_ticket()` function and command handler

---

## Issue #30: buy_ticket Queue Flag Bug - FIXED (Cycle 2) ✅

### Problem (Cycle 2 - ACTUAL BUG)
The `-q true` flag was returning -1 instead of 0 for certain queue operations. The edge case occurred when:
1. `checkAvailableSeats` indicated enough seats were available (`available >= ticket_count`)
2. But `reserveSeats()` failed for some reason
3. The code returned -1 even when `-q true` was set, instead of falling back to queue

### Root Cause
In `cmd_buy_ticket` at main.cpp:1088-1097, when `reserveSeats()` failed but `allow_queue` was true, the code directly returned -1 instead of checking the queue flag:

```cpp
// Old logic (WRONG):
if (available >= ticket_count) {
    if (reserveSeats(...)) {
        // Success
        return 0;
    } else {
        return -1;  // BUG: Doesn't check allow_queue!
    }
} else if (allow_queue) {
    // Queue
    return 0;
}
```

### Fix Applied (Cycle 2)
Added `allow_queue` check when `reserveSeats()` fails:

```cpp
if (available >= ticket_count) {
    if (reserveSeats(...)) {
        // Success
        return 0;
    } else if (allow_queue) {
        // reserveSeats failed but queue is allowed - queue instead
        createOrder(..., 'p');
        std::cout << "queue" << std::endl;
        return 0;
    } else {
        return -1;
    }
} else if (allow_queue) {
    // Not enough seats, but queuing is allowed
    createOrder(..., 'p');
    std::cout << "queue" << std::endl;
    return 0;
} else {
    return -1;
}
```

### Test Results
- ✅ When `reserveSeats` fails but `-q true`, now outputs "queue" and returns 0
- ✅ Maintains previous validation: ticket_count > seatNum still returns -1
- ✅ All existing queue behavior preserved

### Status
✅ Edge case fix complete and tested
✅ Ready to commit

---

## Issue #30: buy_ticket Queue Flag Bug - Previous Fix (Cycle 1)

### Problem (Cycle 1 - CAPACITY VALIDATION)
According to README FAQ: "If the number of tickets purchased is greater than the maximum number of seats on the train, can you still wait? No."

However, buy_ticket allowed queueing for ANY ticket count when `-q true` was set, even if the requested tickets exceeded the train's total capacity (seatNum).

### Fix Applied (Cycle 1)
Added validation to reject requests exceeding train capacity:

```cpp
if (ticket_count > train->seatNum) {
    return -1;
}
```

### Status
✅ Capacity validation complete (Cycle 1)

---

## Previous Work

### Issue #25: query_ticket Seat Calculation Bug - FIXED

Successfully fixed seat availability display bug in query_ticket command.

### buy_ticket Implementation - COMPLETE

Successfully implemented full buy_ticket command with:
- Parameter parsing and validation
- Date calculation
- Price calculation
- Seat management
- Order creation
- Queue support (with capacity validation fix)

### Train Management Commands

Implemented add_train, delete_train, release_train, and query_train commands.

### Key Achievements
- All train and ticket commands working correctly
- Proper seat tracking and availability management
- Persistent data storage using B+ trees
- Date/time calculations handling multi-day journeys
