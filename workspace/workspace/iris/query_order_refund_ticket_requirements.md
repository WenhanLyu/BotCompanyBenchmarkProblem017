# Requirements Analysis: query_order and refund_ticket Commands

**Analyst:** Iris
**Date:** 2026-02-26
**Status:** Complete

## Executive Summary

This document provides a comprehensive analysis of the `query_order` and `refund_ticket` commands, including critical requirements, edge cases, data structure needs, and implementation recommendations. Both commands are critical path features with complex state management and ordering requirements.

---

## 1. query_order Command Analysis

### 1.1 Critical Requirements

**Basic Specifications (from README lines 376-403):**
- **Frequency:** F (Frequently used, ~100,000 operations)
- **Parameters:** `-u` (username)
- **Permission:** User `-u` must be logged in
- **Output:** All orders for the user, sorted by transaction time from newest to oldest
- **Return:**
  - Success: First line = number of orders, then one line per order
  - Failure: `-1`

**Output Format:**
```
[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>
```

**Status Values:**
- `success` - Ticket purchase successful (status='s' in DB)
- `pending` - In standby ticket queue (status='p' in DB)
- `refunded` - Already refunded (status='r' in DB)

### 1.2 Data Structures Needed

**Existing Infrastructure (AVAILABLE):**
1. **Order struct** (types.hpp:337-373)
   - Contains all necessary data: username, trainID, dates, stations, indices, ticket count, price
   - Has `status` field: 's', 'p', 'r'
   - Has `timestamp` field for ordering

2. **OrderKey struct** (types.hpp:375-410)
   - Key: (username, timestamp)
   - Already implements comparison operators
   - Orders are naturally sorted by timestamp within username

3. **B+ Tree storage** (order_system.hpp:21)
   - `BPTree<OrderKey, Order, 32> orders`
   - Already persisted to disk
   - Has `forEach` for iteration

4. **Helper Function** (order_system.hpp:189-201)
   - `queryUserOrders(username, callback)` - already implemented!
   - Iterates through orders for a specific user

### 1.3 Implementation Requirements

**Core Logic:**
1. Validate user is logged in
2. Collect all orders for the user
3. Sort by timestamp DESC (newest first)
4. Format and output each order with correct status string

**Date/Time Calculation:**
Must calculate `LEAVING_TIME` and `ARRIVING_TIME` for display:
- Use `departure_date` from Order (this is the start date from starting station)
- Calculate actual departure time from `from_station`:
  - Start with `departure_date` and train's `startTime`
  - Add travel times and stopover times up to `from_station`
  - Add stopover time at `from_station` for departure
- Calculate arrival time at `to_station`:
  - Continue from departure time
  - Add travel times between stations

### 1.4 Edge Cases

**Critical Edge Cases:**

1. **User Not Logged In**
   - Must return `-1`
   - Check `logged_in_users` hash table

2. **User Has No Orders**
   - Output: `0` (zero orders)
   - This is success, not failure

3. **Order Timestamp Ordering**
   - MUST sort by timestamp DESC (newest first)
   - B+ tree gives ASC order, need to reverse
   - Per README line 384: "sorted by transaction time from newest to oldest (even if a standby order is successfully ticketed, the transaction time is based on the time of placing the order)"
   - **CRITICAL:** Standby orders that get fulfilled later still keep their original timestamp

4. **Mixed Status Types**
   - User may have success, pending, and refunded orders mixed
   - All must be shown in timestamp order
   - Status must map correctly: 's'->"success", 'p'->"pending", 'r'->"refunded"

5. **Date Calculation Across Days**
   - Train journey may span multiple days
   - Must handle month boundaries (June-August)
   - DateTime.addMinutes already handles this

6. **Large Number of Orders**
   - Frequency is F (~100k operations)
   - Users could have many orders
   - Need efficient collection and sorting

7. **Price Display**
   - Show total price (already stored in Order.total_price)
   - NOT per-ticket price

### 1.5 Failure Modes

**When to return -1:**
- User parameter missing
- User not logged in
- User doesn't exist (though this is covered by "not logged in")

**When NOT to fail:**
- User has zero orders (output "0")
- All orders are refunded (still show them)

---

## 2. refund_ticket Command Analysis

### 2.1 Critical Requirements

**Basic Specifications (from README lines 404-420):**
- **Frequency:** N (Normally used, ~10,000 operations)
- **Parameters:** `-u` (username), `-n` (order number, default=1)
- **Description:** Refund the n-th order from newest to oldest (same order as query_order)
- **Permission:** User `-u` must be logged in
- **Return:**
  - Success: `0`
  - Failure: `-1`

**Important Note (from README line 473-475):**
> "Q: When refunding the n-th order, is the serial number n the serial number including refunded orders?"
> "A: Yes, it is equivalent to the n-th order output by `query_order`."

This means: refunded orders COUNT in the numbering!

### 2.2 Data Structures Needed

**Existing Infrastructure (AVAILABLE):**
1. Order storage (same as query_order)
2. `updateOrderStatus(username, timestamp, new_status)` - already implemented!
3. `releaseSeats(trainID, date, from_idx, to_idx, count)` - already implemented!
4. `processStandbyQueue(trainID, date)` - already implemented!

**State Transitions:**
- `'s'` (success) → `'r'` (refunded) ✓ ALLOWED
- `'p'` (pending) → `'r'` (refunded) ✓ ALLOWED
- `'r'` (refunded) → NO CHANGE (already refunded)

### 2.3 Implementation Requirements

**Core Logic:**
1. Validate user is logged in
2. Get all user orders sorted by timestamp DESC (same as query_order)
3. Check if n is valid (1 ≤ n ≤ number of orders)
4. Select the n-th order
5. Check if order is already refunded (status='r')
   - If already refunded: return -1 (cannot refund twice)
6. If status='s' (successful order):
   - Release seats back to train
   - Process standby queue
7. Update order status to 'r'

**Standby Queue Processing (CRITICAL):**
- Per README line 461-463: "Is the priority of the standby ticket queue that the first order is satisfied first, and if it cannot be satisfied, check if the next one can be satisfied? Yes."
- After releasing seats, must try to fulfill pending orders in timestamp order (FIFO)
- Already implemented in `processStandbyQueue()`

### 2.4 Edge Cases

**Critical Edge Cases:**

1. **User Not Logged In**
   - Must return `-1`

2. **Invalid Order Number**
   - n < 1: return `-1`
   - n > total orders: return `-1`
   - Must count ALL orders including refunded ones

3. **Already Refunded Order**
   - Attempting to refund an order with status='r'
   - **CRITICAL DECISION NEEDED:** Should this fail or succeed?
   - **My Analysis:** README says "refund the n-th order" - attempting to refund an already-refunded order should fail (-1)
   - This prevents double-releasing seats

4. **Refunding Pending Orders**
   - Order with status='p' (in standby queue)
   - Should succeed (change status to 'r')
   - **NO seat release** needed (seats were never allocated)
   - **NO standby queue processing** needed (no seats freed)

5. **Refunding Successful Orders**
   - Order with status='s'
   - Must release seats (call `releaseSeats()`)
   - Must trigger standby queue processing for same train/date
   - Standby orders can now be fulfilled

6. **Standby Queue Fulfillment Chain**
   - Refund may free enough seats to fulfill one pending order
   - That might not be the first pending order (if first needs more seats)
   - Per FAQ: try each pending order in timestamp order (FIFO), fulfill if possible
   - Already handled by existing `processStandbyQueue()`

7. **Multiple Segments**
   - Refund must release seats for ALL segments (from_idx to to_idx)
   - Each segment's availability increases by ticket_count
   - Already handled by existing `releaseSeats()`

8. **Date Calculation**
   - Order stores `departure_date` (date train leaves starting station)
   - This is the correct key for SeatAvailability
   - No complex date calculation needed for refund

9. **Train No Longer Exists**
   - What if train was deleted after order was placed?
   - **Cannot happen:** Released trains cannot be deleted (per README line 229)
   - Orders can only exist for released trains
   - **Edge case:** Train added, orders placed, train NOT released yet?
   - **Safe:** Orders can only be created for released trains (buy_ticket checks this)

10. **Default Parameter Value**
    - `-n` is optional, defaults to 1
    - Must handle case where `-n` is not provided

### 2.5 Failure Modes

**When to return -1:**
- User parameter missing
- User not logged in
- n < 1 or n > total number of orders
- Order is already refunded (status='r')
- Order doesn't exist (covered by n > total orders)

**When NOT to fail:**
- Refunding a pending order (just update status)
- Refunding the only order a user has

### 2.6 Concurrency Considerations

**Potential Race Conditions:**
While this is a single-threaded system (no explicit concurrency in spec), we should be aware of data consistency:

1. **Seat Release + Standby Fulfillment:**
   - Must be atomic: release seats, THEN process queue
   - If queue processing fails mid-way, some seats remain available
   - This is acceptable (conservative approach)

2. **Order Status Updates:**
   - Must update status AFTER seats are released and queue processed
   - If failure occurs during seat release, order remains in original state

---

## 3. Data Structure Requirements Summary

### 3.1 Already Available (No New Structures Needed!)

All necessary data structures already exist:

1. **Order struct** - Complete with all fields
2. **OrderKey struct** - Properly indexed by (username, timestamp)
3. **B+ Tree storage** - Persistent, efficient
4. **Helper functions:**
   - `queryUserOrders()` - Iterate user's orders
   - `updateOrderStatus()` - Change order status
   - `releaseSeats()` - Return seats to pool
   - `processStandbyQueue()` - Fulfill pending orders

### 3.2 Additional Needs

**For query_order:**
- **Temporary array** to collect orders (for reverse sorting)
- Suggested size: 1000 orders (reasonable upper bound)
- Alternative: Collect in array, reverse iterate

**For refund_ticket:**
- Same temporary collection as query_order
- No additional structures needed

---

## 4. Implementation Complexity Analysis

### 4.1 query_order Complexity

**Time Complexity:**
- Iterate all user orders: O(O_u) where O_u = orders per user
- B+ tree forEach with filtering: O(log N + O_u)
- Sorting: O(O_u log O_u)
- **Overall: O(O_u log O_u)**

**Space Complexity:**
- Temporary array: O(O_u)

**Date Calculation Per Order:**
- Need to recalculate leaving/arriving times
- Requires train lookup: O(log T) where T = total trains
- Per order: O(log T + S) where S = stations per train
- **Total: O(O_u × (log T + S))**

### 4.2 refund_ticket Complexity

**Time Complexity:**
- Get n-th order: Same as query_order - O(O_u log O_u)
- Update order status: O(log N)
- Release seats: O(S) where S = segments
- Process standby queue: O(P × S) where P = pending orders for this train/date
- **Overall: O(O_u log O_u + P × S)**

**Space Complexity:**
- Temporary array: O(O_u)
- Standby queue: O(P) (already in system)

---

## 5. Critical Correctness Requirements

### 5.1 Order Numbering Consistency

**CRITICAL:** The n-th order in refund_ticket MUST match the n-th line in query_order output.

**Test Case:**
```
query_order -u Alice
3
[success] TRAIN1 A 06-01 10:00 -> B 06-01 12:00 100 2
[refunded] TRAIN2 C 06-02 14:00 -> D 06-02 16:00 200 1
[pending] TRAIN3 E 06-03 08:00 -> F 06-03 10:00 150 3

refund_ticket -u Alice -n 2
# Must refund TRAIN2 (even though already refunded -> should fail)
# Output: -1

refund_ticket -u Alice -n 1
# Must refund TRAIN1 (the newest order)
# Output: 0
```

### 5.2 Status Mapping

**CRITICAL:** Status characters must map correctly:
- 's' → "success"
- 'p' → "pending"
- 'r' → "refunded"

### 5.3 Standby Queue Fairness

**CRITICAL:** After refund, pending orders must be processed in timestamp order (FIFO).

**Test Case:**
Train with 10 seats, all booked.
- Pending order 1: needs 5 seats (timestamp 100)
- Pending order 2: needs 3 seats (timestamp 101)
- Refund: 4 seats released

Expected behavior:
- Check order 1: needs 5 seats, only 4 available → skip
- Check order 2: needs 3 seats, 4 available → fulfill!

### 5.4 Double Refund Prevention

**CRITICAL:** Cannot refund an already refunded order.

**Test Case:**
```
query_order -u Bob
1
[success] TRAIN1 A 06-01 10:00 -> B 06-01 12:00 100 2

refund_ticket -u Bob -n 1
# Output: 0 (success)

refund_ticket -u Bob -n 1
# Output: -1 (already refunded)
```

---

## 6. Edge Case Test Scenarios

### 6.1 Boundary Cases

1. **User with Zero Orders**
   ```
   query_order -u NewUser
   0
   ```

2. **User with One Order**
   ```
   refund_ticket -u Alice -n 1  # Valid
   refund_ticket -u Alice -n 2  # Invalid (n > count)
   ```

3. **Large Order Number**
   ```
   refund_ticket -u Alice -n 999999
   # Output: -1 (exceeds order count)
   ```

4. **Default Refund Parameter**
   ```
   refund_ticket -u Alice
   # Should refund n=1 (newest order)
   ```

### 6.2 Status Transition Cases

**Case 1: Refund Success Order**
```
Initial: [success] ... (status='s')
After:   [refunded] ... (status='r')
Side effect: seats released, standby queue processed
```

**Case 2: Refund Pending Order**
```
Initial: [pending] ... (status='p')
After:   [refunded] ... (status='r')
Side effect: none (no seats to release)
```

**Case 3: Refund Already Refunded**
```
Initial: [refunded] ... (status='r')
After:   [refunded] ... (status='r')
Result: -1 (operation fails)
```

### 6.3 Date Boundary Cases

**Case 1: Journey Across Months**
```
Departure: 06-30 23:00
Journey: 2 hours
Arrival: 07-01 01:00
```

**Case 2: Multi-Day Journey**
```
Departure: 06-01 10:00
Journey: 50 hours
Arrival: 06-03 12:00
```

---

## 7. Performance Considerations

### 7.1 Frequency Analysis

- **query_order:** F (~100,000 operations)
  - Must be fast for users with many orders
  - Optimize: minimize train lookups

- **refund_ticket:** N (~10,000 operations)
  - Less frequent, can afford more computation
  - Complex logic (standby processing) is acceptable

### 7.2 Optimization Opportunities

**For query_order:**
1. Cache train data during iteration (avoid repeated lookups)
2. Reuse date/time calculation logic from query_ticket
3. Consider pre-allocating result array

**For refund_ticket:**
1. Early exit if order already refunded
2. Skip standby processing for pending order refunds
3. Limit standby queue size (currently 1000, seems reasonable)

---

## 8. Implementation Recommendations

### 8.1 Development Order

1. **Implement query_order first**
   - Simpler logic
   - Needed to test refund_ticket (verify order numbering)
   - Establishes order collection pattern

2. **Implement refund_ticket second**
   - Reuse order collection from query_order
   - Leverage existing seat management functions

### 8.2 Code Reuse

**Create helper function:**
```cpp
// Collect all user orders sorted by timestamp DESC
void collectUserOrders(const char* username, Order* result_array, int& count);
```

**Both commands use this:**
- query_order: collect and display all
- refund_ticket: collect, select n-th, process

### 8.3 Testing Strategy

**Unit Tests:**
1. query_order with 0 orders
2. query_order with mixed statuses
3. refund_ticket with valid n
4. refund_ticket with invalid n
5. Double refund attempt
6. Refund triggering standby fulfillment

**Integration Tests:**
1. Buy ticket → query → refund → query (verify status change)
2. Buy with queue → refund other ticket → verify fulfillment
3. Multiple refunds in sequence

---

## 9. Risk Assessment

### 9.1 High Risk Areas

1. **Order Numbering Consistency** ⚠️ CRITICAL
   - Risk: query_order and refund_ticket use different ordering
   - Mitigation: Use shared collection function

2. **Double Refund** ⚠️ CRITICAL
   - Risk: Refunding already-refunded order releases seats twice
   - Mitigation: Check status before processing

3. **Standby Queue Processing** ⚠️ MEDIUM
   - Risk: Complex logic, easy to get wrong
   - Mitigation: Thoroughly test edge cases

4. **Date Calculation** ⚠️ MEDIUM
   - Risk: Off-by-one errors in date/time arithmetic
   - Mitigation: Reuse existing tested code from query_ticket/buy_ticket

### 9.2 Low Risk Areas

1. **Data Structure:** All needed structures exist
2. **Persistence:** B+ tree handles automatically
3. **User Authentication:** Pattern already established

---

## 10. Open Questions / Ambiguities

### 10.1 Resolved (from FAQ)

✅ **Q:** Does refund numbering include refunded orders?
**A:** Yes (README line 475)

✅ **Q:** Can you refund an order that's already refunded?
**A:** Not explicitly stated, but logically should fail (prevent double-release)

✅ **Q:** Standby queue priority?
**A:** FIFO by timestamp (README line 462)

### 10.2 Implementation Decisions Needed

1. **Should refund_ticket fail if order is already refunded?**
   - **Recommendation:** YES (return -1)
   - **Reasoning:** Prevents double-releasing seats, matches user expectation

2. **How many orders to support per user?**
   - **Recommendation:** 1000 order limit per query
   - **Reasoning:** Matches pattern in other commands (e.g., query_ticket)

3. **Should we optimize date calculation?**
   - **Recommendation:** Reuse calculation from query_ticket
   - **Reasoning:** Already tested, handles edge cases

---

## 11. Summary Checklist

### query_order Implementation Checklist

- [ ] Validate user is logged in
- [ ] Collect all user orders
- [ ] Sort by timestamp DESC (newest first)
- [ ] Look up train data for each order
- [ ] Calculate leaving time (departure from from_station)
- [ ] Calculate arriving time (arrival at to_station)
- [ ] Map status char to status string
- [ ] Format output correctly
- [ ] Handle zero orders case (output "0")
- [ ] Handle user not logged in (output "-1")

### refund_ticket Implementation Checklist

- [ ] Validate user is logged in
- [ ] Parse n parameter (default to 1 if missing)
- [ ] Collect all user orders (same as query_order)
- [ ] Validate n is in range [1, order_count]
- [ ] Select the n-th order
- [ ] Check if already refunded (status='r') → fail
- [ ] If status='s': release seats
- [ ] If status='s': process standby queue
- [ ] If status='p': no seat operations needed
- [ ] Update order status to 'r'
- [ ] Return 0 on success, -1 on failure

---

## Conclusion

Both `query_order` and `refund_ticket` are well-specified commands with all necessary data structures already in place. The main implementation challenges are:

1. **Correctness:** Ensuring order numbering matches between commands
2. **Status Management:** Handling all three status types correctly
3. **Standby Processing:** Properly triggering queue fulfillment after refunds

With careful attention to edge cases and reuse of existing helper functions, both commands should be straightforward to implement correctly.

**Estimated Complexity:**
- query_order: MEDIUM (date calculation complexity)
- refund_ticket: MEDIUM-HIGH (state transitions + standby queue)

**Critical Success Factors:**
- Shared order collection logic
- Thorough testing of refund → standby fulfillment chain
- Prevention of double-refund bug
