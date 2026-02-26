# query_order Implementation Review

**Date:** 2026-02-26
**Commit:** 0bb63f6 ([Leo] Implement query_order command)
**Reviewer:** Kyra

---

## Verification Results

### 1. Implementation Exists ✅ PASS

**Function location:** main.cpp:1107-1164
**Main loop handler:** main.cpp:1257-1264

```cpp
int cmd_query_order(const CommandParser& parser) {
    // ... (57 lines of implementation)
}
```

Handler in main loop correctly processes "query_order" command and calls cmd_query_order.

---

### 2. Parameter Handling ✅ PASS

**Lines 1108-1124:**

```cpp
const char* username = parser.get('u');

if (!username) {
    return -1;  // Missing username
}

// Check if user is logged in
bool* is_logged_in = logged_in_users.find(username);
if (!is_logged_in || !(*is_logged_in)) {
    return -1;  // Not logged in
}

// Check if user exists
User* user = users.find(username);
if (!user) {
    return -1;  // User doesn't exist
}
```

✅ Extracts username with `parser.get('u')`
✅ Returns -1 if username is missing
✅ Checks if user is logged in
✅ Checks if user exists
✅ Returns -1 for invalid/not-logged-in users

---

### 3. Order Retrieval Logic ✅ PASS

**Lines 1126-1161:**

```cpp
// First count the orders
int count = 0;
queryUserOrders(username, [&](const Order& order) {
    count++;
});

// If no orders, output 0 and return
if (count == 0) {
    std::cout << "0" << std::endl;
    return 0;
}

// Output count
std::cout << count << std::endl;

// Output each order
queryUserOrders(username, [&](const Order& order) {
    // ... format and output each order
});
```

✅ Uses `queryUserOrders` helper from order_system.hpp:190
✅ Counts orders first (two-pass approach)
✅ Special case: outputs "0" if no orders
✅ Outputs order count on first line

---

### 4. Output Format ✅ PASS

**Lines 1142-1161:**

```cpp
queryUserOrders(username, [&](const Order& order) {
    // Format: [timestamp] [trainID] [from_station] [to_station] [status] [total_price] [ticket_count]
    // Status: s=success, p=pending, r=refunded
    const char* status_str;
    if (order.status == 's') {
        status_str = "success";
    } else if (order.status == 'p') {
        status_str = "pending";
    } else {
        status_str = "refunded";
    }

    std::cout << order.timestamp << " "
              << order.trainID << " "
              << order.from_station << " "
              << order.to_station << " "
              << status_str << " "
              << order.total_price << " "
              << order.ticket_count << std::endl;
});
```

✅ Format matches: `[timestamp] [trainID] [from] [to] [status] [price] [count]`
✅ Status mapping correct: 's'→"success", 'p'→"pending", 'r'→"refunded"
✅ Fields in correct order: timestamp, trainID, from_station, to_station, status, total_price, ticket_count
✅ Each order on separate line

---

### 5. No Unintended Changes ✅ PASS

**Git diff analysis:**

```bash
$ git show --stat 0bb63f6
 main.cpp | 67 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 1 file changed, 67 insertions(+)
```

✅ Only main.cpp modified
✅ Only 67 lines added (no deletions or modifications)
✅ Changes limited to:
  - cmd_query_order function (lines 1107-1164)
  - query_order handler in main loop (lines 1257-1264)

✅ No query_ticket bug fixes
✅ No refund_ticket implementation
✅ No other feature additions

---

## Summary

### ✅ **OVERALL: PASS**

The query_order implementation is **complete and correct**:

- Implementation exists at the correct location
- Parameter handling is robust and validates all required conditions
- Uses queryUserOrders helper correctly with two-pass approach
- Output format matches specification exactly
- ONLY query_order functionality was added (clean, focused commit)

**No issues found.** The implementation is ready for production.
