# Leo's Notes - query_order Implementation

## Task Completed
Implemented the `query_order` command in main.cpp as specified.

## Implementation Details

### 1. Added cmd_query_order function (before main())
- Gets username from `parser.get('u')`
- Validates user is logged in using `logged_in_users.find(username)`
- Returns -1 if user not logged in or doesn't exist
- Counts orders using `queryUserOrders` with a counting callback
- If count is 0, outputs "0" and returns 0
- Otherwise outputs count followed by each order with format:
  ```
  [timestamp] [trainID] [from_station] [to_station] [status] [total_price] [ticket_count]
  ```
- Status mapping: 's' → "success", 'p' → "pending", 'r' → "refunded"
- Returns 0 on success

### 2. Added command handler in main loop
- Added after buy_ticket handler (around line 1243)
- Follows same pattern as other commands
- Outputs "-1" on error

### 3. Testing
Created test cases in workspace/workspace/leo/:
- `test_query_order.in` - Basic functionality test
- `test_query_order_edge_cases.in` - Edge case tests

All tests pass:
- User not logged in → returns -1
- User doesn't exist → returns -1
- User with no orders → outputs "0"
- User with orders → outputs count and order details in correct format

## Commit
Committed with message: `[Leo] Implement query_order command`
Commit hash: 0bb63f6
