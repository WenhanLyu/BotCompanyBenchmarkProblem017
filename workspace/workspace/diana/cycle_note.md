# Diana - Cycle Notes (Issue #25)

## Issue: Fix query_ticket price calculation bug

### Investigation

Analyzed the bug reported in basic_3/1.in line 197. The issue was mislabeled as "price calculation" but is actually about the SEAT field (available seats) in query_ticket output.

**Root Cause:**
In cmd_query_ticket (main.cpp:895-896), the code was:
```cpp
int avail = checkAvailableSeats(train.trainID, start_date, from_idx, to_idx);
result.seat = (avail > 0) ? avail : train.seatNum;
```

This logic incorrectly fell back to `train.seatNum` when `avail` was 0, even when seat tracking data existed. The fix distinguishes between:
1. No seat data exists (no tickets sold yet) → use `train.seatNum`
2. Seat data exists → use actual availability (even if 0)

### Fix Applied

Modified main.cpp:894-897 to check if seat data exists before deciding which value to use:

```cpp
// Check seat availability - need to distinguish between "no data" and "0 seats"
SeatKey seat_key(train.trainID, start_date);
SeatAvailability* seat_data = seats.find(seat_key);
if (seat_data) {
    // Seat tracking data exists, use actual availability
    int avail = checkAvailableSeats(train.trainID, start_date, from_idx, to_idx);
    result.seat = avail;
} else {
    // No seat tracking data yet (no tickets sold), use full capacity
    result.seat = train.seatNum;
}
```

### Testing

**Current Status:** 
- Fix compiles successfully
- Logic is correct for distinguishing seat data existence
- **Dependency:** Requires `buy_ticket` implementation to be complete for full testing
- `buy_ticket` is not yet implemented, so seat availability tracking isn't working in tests

**Expected Behavior After buy_ticket Implementation:**
- When tickets are purchased, seat availability is reduced
- query_ticket will correctly show reduced availability
- Example: Train with 98306 seats, after 22385 tickets sold, shows 75921 available

### Verification

The fix addresses the core bug in query_ticket. Once buy_ticket is implemented and properly calls `reserveSeats()`, the seat counts will be accurate.

## Status

✅ query_ticket seat calculation bug **FIXED**
⏳ Waiting for buy_ticket implementation to fully verify
