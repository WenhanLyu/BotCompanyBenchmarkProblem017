# Query Order Code Verification Report
**Reviewer**: Zara
**Date**: 2026-02-26
**Commit**: 0bb63f6 [Leo] Implement query_order command

---

## 1. Format Verification ✅ PASS

**Requirement**: Output format must be exactly: `[TIMESTAMP] [TRAINID] [FROM] [TO] [STATUS] [PRICE] [COUNT]`

**Implementation** (main.cpp:1154-1160):
```cpp
std::cout << order.timestamp << " "
          << order.trainID << " "
          << order.from_station << " "
          << order.to_station << " "
          << status_str << " "
          << order.total_price << " "
          << order.ticket_count << std::endl;
```

**Status Conversion** (main.cpp:1145-1152):
- `s` → `"success"` ✓
- `p` → `"pending"` ✓
- `r` → `"refunded"` (else branch) ✓

**Result**: Format is exactly correct.

---

## 2. Edge Case Handling ✅ PASS

### 2.1 No Username Parameter
**Lines 1108-1112**:
```cpp
if (!username) {
    return -1;
}
```
✓ Returns -1 if -u flag missing

### 2.2 User Not Logged In
**Lines 1115-1118**:
```cpp
bool* is_logged_in = logged_in_users.find(username);
if (!is_logged_in || !(*is_logged_in)) {
    return -1;
}
```
✓ Returns -1 if user not logged in

### 2.3 User Doesn't Exist
**Lines 1121-1124**:
```cpp
User* user = users.find(username);
if (!user) {
    return -1;
}
```
✓ Returns -1 if user doesn't exist

### 2.4 No Orders for User
**Lines 1133-1136**:
```cpp
if (count == 0) {
    std::cout << "0" << std::endl;
    return 0;
}
```
✓ Outputs "0" and returns 0 when no orders exist

**Result**: All edge cases handled correctly.

---

## 3. Infrastructure Usage ✅ PASS

**Requirement**: Must use queryUserOrders helper from order_system.hpp

**Implementation**:
- Line 1128: `queryUserOrders(username, [&](const Order& order) { count++; });`
- Line 1142: `queryUserOrders(username, [&](const Order& order) { ... });`

**Helper Function** (order_system.hpp:189-201):
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

**Result**: Correctly uses existing infrastructure. Does NOT reimplement query logic.

---

## 4. Sorting Verification ✅ PASS

**Requirement**: Orders must be sorted by timestamp

**OrderKey Structure** (types.hpp:376-410):
```cpp
struct OrderKey {
    char username[25];
    long long timestamp;

    bool operator<(const OrderKey& other) const {
        int cmp = strcmp(username, other.username);
        if (cmp != 0) return cmp < 0;
        return timestamp < other.timestamp;  // Secondary sort by timestamp
    }
};
```

**Analysis**:
- B+ tree `orders` is indexed by `(username, timestamp)`
- Comparison operator first compares username, then timestamp
- forEach iterates in B+ tree order (sorted)
- All orders for a given user will be grouped together and sorted by timestamp ascending

**Result**: Orders are correctly sorted by timestamp.

---

## 5. Scope Compliance ✅ PASS

**Git Diff Analysis** (commit 0bb63f6):
- **Only file modified**: main.cpp
- **Lines added**: 67 total
  - Lines 1107-1164: `cmd_query_order` function (58 lines)
  - Lines 1257-1265: main() handler for "query_order" (9 lines)

**Verification**:
- ✓ ONLY cmd_query_order function added
- ✓ ONLY main() updated to handle "query_order" command
- ✓ NO other commands modified
- ✓ NO query_ticket bugs fixed
- ✓ NO refund_ticket implemented
- ✓ NO changes to other files

**Result**: Perfect scope compliance. Zero scope creep.

---

## FINAL VERDICT: ✅ PASS

All 5 requirements met:
1. ✅ Format matches exactly
2. ✅ All edge cases handled correctly
3. ✅ Uses existing infrastructure (queryUserOrders)
4. ✅ Orders sorted by timestamp via B+ tree
5. ✅ Perfect scope compliance

**Code Quality**: Clean, well-commented, follows existing patterns.

**Recommendation**: Implementation is production-ready. APPROVE.
