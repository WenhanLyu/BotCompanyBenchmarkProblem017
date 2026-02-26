# Diana's Implementation Notes

## Issue #30: buy_ticket Queue Flag Bug - FIXED ✅

### Problem
According to README FAQ: "If the number of tickets purchased is greater than the maximum number of seats on the train, can you still wait? No."

However, buy_ticket allowed queueing for ANY ticket count when `-q true` was set, even if the requested tickets exceeded the train's total capacity (seatNum).

### Root Cause
In `cmd_buy_ticket` (main.cpp:1076-1098), the code checked seat availability but didn't validate that `ticket_count <= train->seatNum` before allowing queue.

```cpp
// Old logic (WRONG):
if (available >= ticket_count) {
    // Purchase succeeds
} else if (allow_queue) {
    // Queue allowed - but didn't check if ticket_count > seatNum!
    createOrder(..., 'p');
    std::cout << "queue" << std::endl;
    return 0;
}
```

### Fix Applied
Added validation BEFORE price calculation to reject requests exceeding train capacity:

```cpp
// Validate ticket count doesn't exceed train capacity
// Per FAQ: "If the number of tickets purchased is greater than the maximum number of seats on the train, can you still wait? No."
if (ticket_count > train->seatNum) {
    return -1;
}
```

### Test Results
Created test with train capacity 100 seats:
- ✅ buy_ticket -n 50 (within capacity) → Success (price 2500)
- ✅ buy_ticket -n 60 -q false (insufficient, no queue) → -1 (correct)
- ✅ buy_ticket -n 60 -q true (insufficient, queue allowed) → queue (correct)
- ✅ buy_ticket -n 101 -q true (exceeds capacity) → -1 (FIXED!)

### Status
✅ Fix complete and tested
✅ Committed to repository

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
