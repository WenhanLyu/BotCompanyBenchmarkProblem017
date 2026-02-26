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
- **buy_ticket** not yet implemented (blocks full testing)
- **query_order** not yet implemented
- **refund_ticket** not yet implemented
