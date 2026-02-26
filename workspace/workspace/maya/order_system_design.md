# Order/Ticket Tracking System Design

## Overview
Design for order management and seat availability tracking for the train ticket system.

## 1. Order Data Structure

### Order Struct
```cpp
struct Order {
    char username[25];        // User who made the order
    char trainID[25];        // Train ID
    Date departure_date;     // Date of departure from starting station
    char from_station[35];   // Starting station
    char to_station[35];     // Ending station
    int from_idx;            // Index of starting station in train
    int to_idx;              // Index of ending station in train
    int ticket_count;        // Number of tickets
    int total_price;         // Total price for all tickets
    char status;             // 's'=success, 'p'=pending, 'r'=refunded
    long long timestamp;     // For ordering (monotonic counter)

    Order() : from_idx(0), to_idx(0), ticket_count(0), total_price(0),
              status('s'), timestamp(0) {
        username[0] = '\0';
        trainID[0] = '\0';
        from_station[0] = '\0';
        to_station[0] = '\0';
    }
};
```

### Key Design Points:
- **Status codes**:
  - 's' = success (ticket purchased)
  - 'p' = pending (on standby queue, train full)
  - 'r' = refunded
- **Timestamp**: Monotonic counter to preserve order of operations
- **Station indices**: Stored to avoid re-searching during operations
- **Total price**: Pre-calculated and stored

## 2. Seat Availability Tracking

### Approach: Per-Train-Date Seat Array
For each (trainID, date) pair, maintain an array tracking available seats per segment.

```cpp
struct SeatAvailability {
    char trainID[25];
    Date date;
    int available[99];  // available[i] = seats available from station i to i+1

    SeatAvailability() {
        trainID[0] = '\0';
        for (int i = 0; i < 99; i++) {
            available[i] = 0;
        }
    }
};
```

### Key Operations:
1. **Initialize**: When train is released, create SeatAvailability for each date in sale range
2. **Query**: Check minimum available seats across all segments in journey
3. **Reserve**: Decrease available seats for all segments in journey
4. **Release**: Increase available seats for all segments in journey

### Storage Strategy:
- Use BPTree with composite key: `(trainID, date)`
- Key type: `struct SeatKey { TrainKey trainID; Date date; }`
- Value type: `SeatAvailability`

## 3. Order Persistence

### Primary Order Storage
Use BPTree with composite key for efficient queries:
- **Key**: `struct OrderKey { char username[25]; long long timestamp; }`
- **Value**: `Order`
- This allows efficient `query_order` by username with chronological ordering

### Secondary Index (Optional)
If needed for other queries:
- Could add separate index by (trainID, date) for train-centric queries
- But for M3/M4, username-based queries are primary

## 4. Data Files

### File Structure:
```
orders.dat          - BPTree<OrderKey, Order> - primary order storage
seats.dat           - BPTree<SeatKey, SeatAvailability> - seat tracking
order_counter.dat   - Single long long for timestamp counter
```

## 5. Key Interfaces

### For buy_ticket:
```cpp
// 1. Check seat availability for (trainID, date, from_idx, to_idx)
int checkAvailableSeats(trainID, date, from_idx, to_idx);

// 2. Reserve seats (returns true if successful)
bool reserveSeats(trainID, date, from_idx, to_idx, count);

// 3. Create order with status 's' or 'p'
long long createOrder(username, trainID, date, from_station, to_station,
                      from_idx, to_idx, count, price, status);
```

### For query_order:
```cpp
// Query all orders for a user (chronological order)
void queryUserOrders(username, callback);
```

### For refund_ticket:
```cpp
// 1. Find specific order by (username, trainID, from, to)
Order* findOrder(username, trainID, from_station, to_station);

// 2. Release seats
bool releaseSeats(trainID, date, from_idx, to_idx, count);

// 3. Update order status to 'r'
bool refundOrder(order);

// 4. Process standby queue for this train/date
void processStandbyQueue(trainID, date);
```

## 6. Implementation Order

1. **Phase 1**: Define data structures in types.hpp
   - Order struct
   - SeatAvailability struct
   - OrderKey, SeatKey structs

2. **Phase 2**: Implement seat management
   - Initialize seats when train released
   - Check/reserve/release functions

3. **Phase 3**: Implement order management
   - Create order function
   - Query orders function
   - Order counter management

4. **Phase 4**: Integrate with commands
   - buy_ticket command
   - query_order command
   - refund_ticket command

## 7. Memory Considerations

- Order struct: ~200 bytes
- SeatAvailability: ~450 bytes
- With BPTree degree=16: ~40 orders or ~20 seat records per node
- Most data stays on disk, minimal memory footprint

## 8. Edge Cases to Handle

1. **Date calculations**: Train journey may span multiple days
2. **Seat segments**: Must check ALL segments in journey for minimum
3. **Standby queue**: Orders with same timestamp must maintain FIFO order
4. **Refund processing**: Must try to fulfill pending orders after refund
5. **Train deletion**: Cannot delete released trains with active orders

## Next Steps

1. Add data structures to types.hpp
2. Create seat management functions
3. Test seat availability tracking independently
4. Implement order CRUD operations
5. Test with buy_ticket scenarios
