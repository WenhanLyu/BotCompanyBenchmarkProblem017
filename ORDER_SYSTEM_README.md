# Order/Ticket Tracking System

**Author**: Maya
**Status**: ✅ Implemented and Tested
**Files**: `order_system.hpp`, `types.hpp`

## Overview

Complete order management and seat availability tracking system for the train ticket booking application. This system provides the foundation for implementing `buy_ticket`, `query_order`, and `refund_ticket` commands.

## Architecture

### Data Structures

#### Order (types.hpp)
Represents a ticket order with full journey information:
```cpp
struct Order {
    char username[25];        // User who made the order
    char trainID[25];        // Train ID
    Date departure_date;     // Departure date from starting station
    char from_station[35];   // Starting station
    char to_station[35];     // Ending station
    int from_idx;            // Index of starting station
    int to_idx;              // Index of ending station
    int ticket_count;        // Number of tickets
    int total_price;         // Total price
    char status;             // 's'=success, 'p'=pending, 'r'=refunded
    long long timestamp;     // Monotonic ordering
};
```

#### SeatAvailability (types.hpp)
Tracks available seats per segment for a train on a specific date:
```cpp
struct SeatAvailability {
    char trainID[25];
    Date date;
    int available[99];  // available[i] = seats from station i to i+1
};
```

### Persistence

Two B+ trees provide disk-based storage:
- `orders`: Indexed by `(username, timestamp)` for efficient user queries
- `seats`: Indexed by `(trainID, date)` for seat lookups

### Status Codes

- `'s'` = Success (ticket purchased)
- `'p'` = Pending (on standby queue, train full)
- `'r'` = Refunded

## API Reference

### Initialization

```cpp
#include "order_system.hpp"

// In main.cpp, declare globals:
BPTree<OrderKey, Order, 32> orders;
BPTree<SeatKey, SeatAvailability, 16> seats;

// In main():
orders.open("orders.dat");
seats.open("seats.dat");
initOrderSystem();  // Loads order counter
```

### Seat Management

#### Initialize Seats for Released Train
Call this when a train is released:
```cpp
void initializeSeatsForTrain(const Train& train);
```

#### Check Available Seats
Returns minimum available seats across all segments in journey:
```cpp
int checkAvailableSeats(const char* trainID, const Date& date,
                       int from_idx, int to_idx);
```

#### Reserve Seats
Decreases available seats for all segments. Returns true if successful:
```cpp
bool reserveSeats(const char* trainID, const Date& date,
                 int from_idx, int to_idx, int count);
```

#### Release Seats
Increases available seats (for refunds):
```cpp
bool releaseSeats(const char* trainID, const Date& date,
                 int from_idx, int to_idx, int count);
```

### Order Management

#### Create Order
Creates and persists an order. Returns timestamp:
```cpp
long long createOrder(const char* username, const char* trainID,
                     const Date& date, const char* from_station,
                     const char* to_station, int from_idx, int to_idx,
                     int count, int price, char status);
```

#### Query User Orders
Iterates through all orders for a user in chronological order:
```cpp
queryUserOrders("username", [](const Order& order) {
    // Process each order
});
```

#### Update Order Status
Changes order status (e.g., pending → success, success → refunded):
```cpp
bool updateOrderStatus(const char* username, long long timestamp,
                      char new_status);
```

#### Process Standby Queue
Tries to fulfill pending orders after seats become available:
```cpp
void processStandbyQueue(const char* trainID, const Date& date);
```

## Integration Examples

### buy_ticket Command

```cpp
int cmd_buy_ticket(const CommandParser& parser) {
    const char* username = parser.get('u');
    const char* trainID = parser.get('i');
    const char* date_str = parser.get('d');
    const char* from_st = parser.get('f');
    const char* to_st = parser.get('t');
    const char* count_str = parser.get('n');

    // 1. Get train and find station indices
    Train* train = trains.find(TrainKey(trainID));
    int from_idx = ...; // Find index
    int to_idx = ...;   // Find index

    // 2. Calculate price
    int price = ...;

    // 3. Check availability
    int available = checkAvailableSeats(trainID, date, from_idx, to_idx);

    // 4. Reserve or create pending order
    char status;
    if (available >= count) {
        reserveSeats(trainID, date, from_idx, to_idx, count);
        status = 's';
    } else {
        status = 'p';  // Standby queue
    }

    // 5. Create order
    createOrder(username, trainID, date, from_st, to_st,
               from_idx, to_idx, count, price, status);

    return 0;
}
```

### query_order Command

```cpp
int cmd_query_order(const CommandParser& parser) {
    const char* username = parser.get('u');

    queryUserOrders(username, [](const Order& order) {
        // Format and output order
        std::cout << "[" << (order.status == 's' ? "success" :
                            order.status == 'p' ? "pending" : "refunded")
                  << "] " << order.trainID << " "
                  << order.from_station << " ... " << order.to_station
                  << " " << order.total_price << " " << order.ticket_count
                  << std::endl;
    });

    return 0;
}
```

### refund_ticket Command

```cpp
int cmd_refund_ticket(const CommandParser& parser) {
    const char* username = parser.get('u');
    const char* trainID = parser.get('i');
    const char* from_st = parser.get('f');
    const char* to_st = parser.get('t');

    // 1. Find the order
    Order* order = findOrder(username, trainID, from_st, to_st);
    if (!order || order->status == 'r') return -1;

    // 2. Release seats (if it was successful order)
    if (order->status == 's') {
        releaseSeats(order->trainID, order->departure_date,
                    order->from_idx, order->to_idx, order->ticket_count);
    }

    // 3. Mark as refunded
    updateOrderStatus(username, order->timestamp, 'r');

    // 4. Process standby queue
    processStandbyQueue(order->trainID, order->departure_date);

    return 0;
}
```

### Updating release_train Command

Add this after marking train as released:
```cpp
int cmd_release_train(const CommandParser& parser) {
    // ... existing code to mark train.released = true ...

    // Initialize seat availability
    initializeSeatsForTrain(train);

    return 0;
}
```

## Key Design Decisions

### 1. Composite Keys
- Orders indexed by `(username, timestamp)` for efficient user-based queries
- Timestamps ensure chronological ordering within user's orders

### 2. Segment-Based Seat Tracking
- Track availability per segment (station i → i+1)
- Correctly handles overlapping journeys
- Example: Train A→B→C→D with 100 seats
  - User1 books A→C (uses segments AB, BC)
  - User2 can still book C→D (segment CD unaffected)

### 3. Monotonic Timestamps
- Global counter ensures strict ordering
- Critical for FIFO standby queue processing
- Persisted in `order_counter.dat`

### 4. Status Transitions
- Create with 's' if seats available, 'p' if full
- Pending orders processed FIFO when seats freed
- Only one status transition allowed (no refund → success)

## Testing

Comprehensive test suite in `test_order_system.cpp`:
```bash
g++ -std=c++14 -O2 -o test_order test_order_system.cpp
./test_order
```

Tests cover:
- Order creation and persistence
- Seat availability initialization
- Segment-based reservation
- Multi-order queries
- Status updates
- Standby queue processing

All 6 tests passing ✅

## Memory Considerations

- Order: ~200 bytes
- SeatAvailability: ~450 bytes
- BPTree degree 32 for orders (~40 orders/node)
- BPTree degree 16 for seats (~20 records/node)
- Most data on disk, minimal memory footprint

## Edge Cases Handled

✅ Overlapping journey segments
✅ Multiple orders per user (chronologically ordered)
✅ Standby queue FIFO fairness
✅ Seat checks across all segments
✅ Order status transitions
✅ Train spanning multiple days

## Next Steps

1. Integrate into main.cpp (add global declarations)
2. Update `release_train` command to initialize seats
3. Implement `buy_ticket` command using the API
4. Implement `query_order` command using the API
5. Implement `refund_ticket` command using the API

## Questions?

See:
- `workspace/workspace/maya/order_system_design.md` - Detailed design document
- `workspace/workspace/maya/note.md` - Implementation summary
- `test_order_system.cpp` - Usage examples

---
**Status**: Ready for M3/M4 command implementation
