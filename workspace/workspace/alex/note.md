# Alex - Cycle Notes

## Latest Task (2026-02-26)
Fixed updateOrderStatus B+ tree corruption (Issue #38)

### Problem
updateOrderStatus used find() pointer that points to cached find_result_node. When insert() was called, it could corrupt the tree because the pointer became invalidated.

### Fix
Changed from update-in-place to delete-then-insert approach:

```cpp
// OLD (buggy):
Order* order = orders.find(key);
order->status = new_status;
orders.insert(key, *order);  // Uses invalidated pointer!

// NEW (fixed):
Order* order = orders.find(key);
Order order_copy = *order;     // Copy before modifications
order_copy.status = new_status;
orders.remove(key);            // Delete old entry
orders.insert(key, order_copy); // Insert new entry
```

### Testing
Created minimal test (workspace/workspace/alex/minimal_test.cpp):
- Order status updated from 's' to 'r'
- All order details preserved correctly
- No B+ tree corruption or data loss

### Commit
97f1a05 - "[Alex] Fix updateOrderStatus B+ tree corruption - use delete-then-insert"

---

## Previous Task (2026-02-26)
Fixed query_ticket bug in main.cpp (lines 888-890)

### Bug
query_ticket returned -1 when no trains found

### Fix
Changed to output '0' instead:
- Modified line 889: `return -1;` → `std::cout << "0" << std::endl; return 0;`

### Verification
Tested with non-existent stations - confirmed output is now '0' when no matching trains

### Commit
2b21063 - "[Alex] Fix query_ticket to output 0 when no trains found (instead of -1)"

---

## Previous Task
Fixed query_order command in main.cpp (lines 1107-1214)

## Changes Made

1. **Added required headers**:
   - `#include <vector>` for storing orders
   - `#include <algorithm>` for std::reverse

2. **Fixed output format**:
   - Changed from: `[timestamp] [trainID] [from] [to] [status] [price] [count]`
   - Changed to: `[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>`
   - STATUS now shows [success]/[pending]/[refunded] in brackets
   - Times are calculated from train schedule and formatted as mm-dd hh:mm

3. **Fixed sort order**:
   - Collect all orders into a vector
   - Reverse the vector to output newest-to-oldest (instead of oldest-to-newest)

4. **Price calculation**:
   - Output single ticket price (total_price / ticket_count) instead of total_price

## Implementation Details

- Used DateTime structure with addMinutes() to calculate leaving and arriving times
- Followed the same pattern as query_ticket (lines 758-829) for time calculations
- Used Date::format() and Time::format() methods to output times in mm-dd hh:mm format

## Testing

Tested with basic_3 test case:
- Test 1: **ALL 29 query_order outputs match perfectly**
- Tests 2-5: Have failures in other commands (not query_order related) that prevent reaching query_order commands

## Files Modified

- main.cpp (lines 1-10 for headers, lines 1107-1214 for query_order function)
