# Diana's Implementation Notes

## Issue #25: query_ticket Seat Calculation Bug - FIXED

### Problem
The "price calculation bug" in query_ticket was actually a seat availability calculation bug. The code always returned `train.seatNum` (full capacity) instead of showing reduced availability after ticket purchases.

### Root Cause
In `cmd_query_ticket` (main.cpp:895-896):
```cpp
int avail = checkAvailableSeats(train.trainID, start_date, from_idx, to_idx);
result.seat = (avail > 0) ? avail : train.seatNum;
```

This logic couldn't distinguish between:
1. No seat tracking data exists → should use `train.seatNum`
2. Seat data exists with reduced availability → should use actual `avail`

### Fix Applied
Modified code to explicitly check if seat data exists:
```cpp
SeatKey seat_key(train.trainID, start_date);
SeatAvailability* seat_data = seats.find(seat_key);
if (seat_data) {
    int avail = checkAvailableSeats(train.trainID, start_date, from_idx, to_idx);
    result.seat = avail;  // Use actual availability
} else {
    result.seat = train.seatNum;  // No purchases yet
}
```

### Status
✅ Fix complete and compiles successfully
⏳ Full verification requires `buy_ticket` implementation (not yet done)

### Test Case Reference
- basic_3/1.in line 197: `query_ticket -s 新疆塔城市 -t 浙江省慈溪市 -d 07-01 -p time`
- Expected seat count: 75921 (after 22385 tickets sold)
- Was showing: 98306 (full capacity - wrong!)
- Will show: 75921 (once buy_ticket works)

---

## Previous Work (From Earlier Cycles)

### Train Management Commands
Successfully implemented all 4 train management commands:
1. **add_train** - Parses and stores train data
2. **delete_train** - Deletes unreleased trains
3. **release_train** - Marks trains as released, initializes seat tracking
4. **query_train** - Queries train schedule by trainID and date

### query_ticket Command
Successfully implemented query_ticket with:
- BPTree forEach method for iterating all trains
- Route finding between two stations
- Date/time calculations for departures and arrivals
- Sorting by time or cost
- Seat availability checking (NOW FIXED)

### Key Data Structures
- **BPTree<TrainKey, Train>** for persistent train storage
- **BPTree<SeatKey, SeatAvailability>** for tracking sold seats per date/train
- **TicketResult** struct for query results
- All data persists across program restarts

### Known Dependencies
- **query_order** not yet implemented
- **refund_ticket** not yet implemented

---

## Cycle: buy_ticket Implementation - COMPLETE ✅

### Task
Implement buy_ticket command according to README.md spec (lines 339-365).

### Implementation Details

Successfully implemented `cmd_buy_ticket` in main.cpp with full functionality:

1. **Parameter parsing**: -u, -i, -d, -n, -f, -t, -q (queue flag, default false)

2. **Validation**:
   - User logged in check
   - Train exists and is released
   - Stations exist and in correct order (from before to)
   - Date within train's sale range (after calculating start_date)

3. **Date calculation**:
   - Reverse-engineers start_date from departure date at departure station
   - Same algorithm as query_ticket for consistency
   - Handles multi-day travel correctly

4. **Price calculation**:
   - `price_per_ticket = sum(train->prices[from_idx..to_idx-1])`
   - `total_price = price_per_ticket * ticket_count`

5. **Seat management** (uses order_system.hpp):
   - `checkAvailableSeats()` - finds minimum across all segments
   - `reserveSeats()` - updates SeatAvailability BPTree
   - Properly reduces seat counts for all segments in journey

6. **Order creation**:
   - Uses `createOrder()` with unique timestamp
   - Status 's' for successful purchases
   - Status 'p' for pending (queued) orders

7. **Return behavior**:
   - Success: outputs total_price, returns 0
   - Queue: outputs "queue", returns 0
   - Failure: returns -1 (main prints "-1")

### Test Results

**Simple test**: 10 tickets A→C, price 300 each
- ✅ Outputs: 3000
- ✅ Seats reduce: 1000 → 990

**Maya's query_ticket_test_30.in** (lines 159-197):
- ✅ Line 161: buy_ticket returns 7142328 (was -1)
- ✅ Line 162: buy_ticket returns 53744575 (was -1)
- ✅ Line 186: buy_ticket returns 22150297 (was -1)
- ✅ **Line 197: query_ticket shows 75921 seats (reduced from 98306)** ← KEY SUCCESS

### Success Criteria Met ✅

From assignment:
> "Success means buy_ticket commands return order prices instead of -1, and subsequent query_ticket commands show reduced seat availability."

Both criteria achieved:
1. ✅ buy_ticket returns prices (not -1)
2. ✅ query_ticket shows reduced availability (75921 vs 98306)

### Commit
- Hash: b9e153e
- Message: "[Diana] Implement buy_ticket command with seat reservation and order creation"
