# query_order Implementation Plan

## Issue #32: Analyze query_order requirements and create implementation plan

### 1. Requirements Analysis

**Command**: `query_order` (Frequency: F - ~100,000 operations)

**Parameters**:
- `-u`: username (required)

**Permission Requirements**:
- User `-u` must be logged in

**Return Value**:
- **Success**:
  - First line: integer count of orders
  - Following lines: each order in format `[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>`
  - STATUS values: `success` (ticket purchased), `pending` (in standby queue), `refunded` (already refunded)
  - Orders sorted by transaction time from **newest to oldest**
  - Even if standby order is fulfilled, sort by original placement time
- **Failure**: `-1`

**Example**:
```
> query_order -u Lappland
1
[pending] HAPPY_TRAIN 上院 08-17 05:24 -> 下院 08-17 15:24 628 500
```

### 2. Existing Infrastructure Analysis

#### 2.1 Data Structures (types.hpp)

**Order struct** (lines 338-373):
- `username[25]`: User who made the order
- `trainID[25]`: Train ID
- `departure_date`: Date of departure from starting station (Date type)
- `from_station[35]`: Boarding station name
- `to_station[35]`: Destination station name
- `from_idx`: Index of boarding station in train
- `to_idx`: Index of destination station in train
- `ticket_count`: Number of tickets
- `total_price`: Total price for all tickets
- `status`: 's' (success), 'p' (pending), 'r' (refunded)
- `timestamp`: Monotonic counter for ordering (long long)

**Memory size**: ~220 bytes per Order

**OrderKey struct** (lines 376-410):
- `username[25]`
- `timestamp` (long long)
- Comparison: sorted by username first, then timestamp ascending

**Memory size**: ~32 bytes per OrderKey

#### 2.2 Storage System

**B+ Tree for Orders**:
- Type: `BPTree<OrderKey, Order, 32>` (main.cpp:21)
- Degree 32 means ~31 keys per node
- File: "orders.dat"
- Index: (username, timestamp) - naturally sorted in ascending order

**forEach iteration**:
- Traverses leaf nodes left to right
- Orders for same user come in chronological order (oldest first)
- Need to collect and reverse for newest-first output

#### 2.3 Existing Helper Function

**queryUserOrders()** (order_system.hpp:189-201):
```cpp
template<typename Func>
inline void queryUserOrders(const char* username, Func callback) {
    OrderKey start(username, 0);
    OrderKey end(username, 0x7FFFFFFFFFFFFFFFLL);

    orders.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            callback(order);
        }
    });
}
```
- Already filters orders by username
- Iterates in chronological order (oldest to newest)

### 3. Implementation Strategy

#### 3.1 Algorithm Design

**Overall approach**: Collect → Sort → Format → Output

1. **Parse parameters**: Extract username from `-u` parameter
2. **Validate permissions**: Check if user is logged in
3. **Collect orders**: Use queryUserOrders() to gather all orders for user
4. **Sort orders**: Reverse order (newest first) - timestamp descending
5. **Format each order**:
   - Map status char to string ('s' → "success", 'p' → "pending", 'r' → "refunded")
   - Calculate leaving/arriving times from train data and departure_date
   - Format output line
6. **Output results**: Count + sorted order list

#### 3.2 Time Complexity Analysis

For a user with N orders:
- **Collect orders**: O(M) where M = total orders in system
  - forEach iterates all orders, filters by username
  - Optimization possible: Early exit when past user's range
- **Sort orders**: O(N log N) - but likely small N per user
- **Format each order**: O(N × T) where T = time to lookup train data
  - Each order needs train lookup: O(log T_total) in B+ tree
  - Calculate datetime for leaving/arriving
- **Total**: O(M + N log N + N log T)

**For frequency F (~100,000 ops)**:
- Critical optimization: Minimize work in forEach loop
- Consider: Stop iteration early when past target username range

#### 3.3 Memory Requirements

**Stack memory per invocation**:
- Order collection buffer: ~10 orders typical, max ~1000 per user
- Buffer size: 1000 orders × 220 bytes = ~220 KB
- **Recommendation**: Use fixed-size array of 1000 orders max

**Memory budget**: 42-47 MiB total
- This function is temporary (stack-based)
- After function returns, memory freed
- Safe to use ~500 KB for order collection

### 4. Implementation Details

#### 4.1 Core Function Signature

```cpp
int cmd_query_order(const CommandParser& parser);
```

Returns:
- 0 on success (after printing results)
- -1 on failure

#### 4.2 Detailed Steps

**Step 1: Parameter parsing and validation**
```cpp
const char* username = parser.get('u');
if (!username) return -1;

// Check if user is logged in
bool* is_logged_in = logged_in_users.find(username);
if (!is_logged_in || !(*is_logged_in)) {
    std::cout << "-1" << std::endl;
    return -1;
}
```

**Step 2: Collect orders**
```cpp
Order order_list[1000];  // Fixed buffer
int order_count = 0;

queryUserOrders(username, [&](const Order& order) {
    if (order_count < 1000) {
        order_list[order_count++] = order;
    }
});
```

**Step 3: Sort orders (newest first)**
```cpp
// Bubble sort by timestamp descending
for (int i = 0; i < order_count - 1; i++) {
    for (int j = 0; j < order_count - i - 1; j++) {
        if (order_list[j].timestamp < order_list[j + 1].timestamp) {
            Order temp = order_list[j];
            order_list[j] = order_list[j + 1];
            order_list[j + 1] = temp;
        }
    }
}
```

**Step 4: Format and output**
```cpp
std::cout << order_count << std::endl;

for (int i = 0; i < order_count; i++) {
    const Order& order = order_list[i];

    // Map status to string
    const char* status_str;
    if (order.status == 's') status_str = "success";
    else if (order.status == 'p') status_str = "pending";
    else status_str = "refunded";

    // Find train to get times
    TrainKey key(order.trainID);
    Train* train = trains.find(key);
    if (!train) continue;  // Should not happen

    // Calculate leaving time from order.from_station
    DateTime leaving_time = calculateLeavingTime(train, order.departure_date, order.from_idx);

    // Calculate arriving time at order.to_station
    DateTime arriving_time = calculateArrivingTime(train, order.departure_date, order.to_idx);

    // Format times
    char leaving_date[6], leaving_time_str[6];
    leaving_time.date.format(leaving_date);
    leaving_time.time.format(leaving_time_str);

    char arriving_date[6], arriving_time_str[6];
    arriving_time.date.format(arriving_date);
    arriving_time.time.format(arriving_time_str);

    // Output line
    std::cout << "[" << status_str << "] "
              << order.trainID << " "
              << order.from_station << " "
              << leaving_date << " " << leaving_time_str << " -> "
              << order.to_station << " "
              << arriving_date << " " << arriving_time_str << " "
              << order.total_price << " "
              << order.ticket_count << std::endl;
}
```

#### 4.3 DateTime Calculation Helper Functions

Need to replicate logic from query_ticket for calculating times:

**Calculate leaving time from a station**:
```cpp
DateTime calculateLeavingTime(const Train* train, const Date& start_date, int station_idx) {
    DateTime result(start_date, train->startTime);

    // Add travel times and stopover times up to this station
    int minutes = 0;
    for (int i = 0; i < station_idx; i++) {
        minutes += train->travelTimes[i];
        if (i > 0) {
            minutes += train->stopoverTimes[i - 1];
        }
    }
    // Add stopover at this station (if not first)
    if (station_idx > 0) {
        minutes += train->stopoverTimes[station_idx - 1];
    }

    result.addMinutes(minutes);
    return result;
}
```

**Calculate arriving time at a station**:
```cpp
DateTime calculateArrivingTime(const Train* train, const Date& start_date, int station_idx) {
    DateTime result(start_date, train->startTime);

    // Add travel times and stopover times up to but not including this station's stopover
    int minutes = 0;
    for (int i = 0; i < station_idx; i++) {
        minutes += train->travelTimes[i];
        if (i > 0) {
            minutes += train->stopoverTimes[i - 1];
        }
    }

    result.addMinutes(minutes);
    return result;
}
```

**Important**: The `order.departure_date` field stores the date the train **departs from its starting station**, not the date it departs from the boarding station. This matches buy_ticket's usage.

### 5. Integration Points

#### 5.1 Add to main.cpp

In the main command loop (around line 1197):

```cpp
} else if (command == "query_order") {
    std::getline(std::cin, line);
    CommandParser parser;
    parser.parse(line);
    int result = cmd_query_order(parser);
    if (result == -1) {
        std::cout << "-1" << std::endl;
    }
}
```

#### 5.2 Function Location

Add `cmd_query_order()` in main.cpp after `cmd_buy_ticket()` (after line 1105).

### 6. Testing Considerations

**Test cases to verify**:
1. User not logged in → return -1
2. User with no orders → return "0"
3. User with 1 order → correct format
4. User with multiple orders → correct sorting (newest first)
5. Orders with different statuses (success, pending, refunded)
6. DateTime calculations match expected format
7. Persistence: orders survive program restart

**Edge cases**:
- User with >1000 orders (buffer overflow)
- Order references deleted/non-existent train
- Timestamp ties (should maintain insertion order)

### 7. Performance Estimates

**Memory usage**:
- Stack: ~220 KB (1000 orders buffer)
- Temporary: ~100 bytes for datetime formatting
- Total per call: <300 KB

**Time per operation** (assuming 10 orders per user average):
- Collect orders: ~0.1ms (iterate through B+ tree)
- Sort: ~0.01ms (10 orders)
- Format output: ~0.5ms (10 train lookups + formatting)
- **Total: ~1ms per query_order call**

**For F frequency (~100,000 calls)**:
- Total time: ~100 seconds in worst case
- Acceptable within time limits

### 8. Potential Optimizations

**If performance becomes an issue**:

1. **Early exit from forEach**: Stop iterating when past target username
   - Current forEach always iterates all orders
   - Could add range query support to B+ tree
   - Would reduce from O(M) to O(N + log M)

2. **Cache train lookups**: If same train appears in multiple orders
   - Store last train lookup in local cache
   - Avoid repeated B+ tree lookups

3. **Reduce buffer copies**: Use pointers instead of copying Order structs
   - Save memory and time
   - Requires careful lifetime management

**Recommendation**: Implement basic version first, optimize only if needed.

### 9. Implementation Priority

**Phase 1** (MVP):
- [ ] Add cmd_query_order() function in main.cpp
- [ ] Implement order collection using queryUserOrders()
- [ ] Implement sorting by timestamp descending
- [ ] Implement basic output formatting

**Phase 2** (DateTime calculation):
- [ ] Add helper functions for calculating leaving/arriving times
- [ ] Integrate train lookups
- [ ] Format DateTime output correctly

**Phase 3** (Integration):
- [ ] Add command handler in main() loop
- [ ] Test with example data
- [ ] Verify output format matches spec

**Phase 4** (Testing):
- [ ] Test all edge cases
- [ ] Verify persistence across restarts
- [ ] Performance testing with large datasets

### 10. Risk Assessment

**Low Risk**:
- Data structures already exist
- Helper function (queryUserOrders) already implemented
- Similar patterns exist in other commands (query_ticket)

**Medium Risk**:
- DateTime calculation complexity (but can copy from query_ticket)
- Sorting performance with many orders per user

**High Risk**:
- None identified

**Overall Risk**: LOW - Straightforward implementation with existing infrastructure

### 11. Dependencies

**Required**:
- Order struct (types.hpp) ✓
- OrderKey struct (types.hpp) ✓
- BPTree<OrderKey, Order> (main.cpp) ✓
- queryUserOrders() (order_system.hpp) ✓
- Train lookup via trains.find() ✓
- DateTime calculation utilities ✓

**Blocked by**: NONE - All dependencies met

### 12. Estimated Implementation Time

- Core function: 30 minutes
- DateTime helpers: 20 minutes
- Integration & testing: 30 minutes
- **Total: ~1.5 hours**

### 13. Memory Analysis Summary

**Per-call memory usage**:
```
Stack frame:           ~100 bytes
Order buffer:          ~220 KB (1000 × 220 bytes)
Temp variables:        ~100 bytes
-----------------------------------------
Total per call:        ~220 KB
```

**System-wide impact**:
- query_order is synchronous - only one call at a time
- Memory freed immediately after function returns
- No persistent memory overhead
- Well within 42 MiB limit (uses <1% of budget per call)

**Recommendation**: Safe to implement as designed.

---

## Conclusion

The query_order command has a straightforward implementation path:
1. Use existing queryUserOrders() to collect orders
2. Sort by timestamp descending
3. Format output with train time calculations
4. Integrate into main command loop

All required infrastructure exists. Main work is formatting logic and datetime calculations (can copy patterns from query_ticket). Low risk, high confidence in success.

**Ready for implementation.**
