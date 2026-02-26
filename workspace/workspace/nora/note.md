# Edge Case Verification Report for query_order

## Assignment
Verify that query_order handles all 4 edge cases correctly:
1. No orders (user exists, logged in, but has no orders) → output "0"
2. Invalid user (user doesn't exist) → output "-1"
3. User not logged in → output "-1"
4. Missing username parameter → output "-1"

## Code Review

### Location
`main.cpp` lines 1107-1164: `cmd_query_order` implementation

### Edge Case Handling in Code

#### Edge Case 4: Missing username parameter
**Code**: Lines 1110-1112
```cpp
if (!username) {
    return -1;
}
```
**Code Logic**: ✅ CORRECT - Returns -1 when username is missing
**Main handling**: Lines 1262-1263 print "-1" when result is -1

#### Edge Case 3: User not logged in
**Code**: Lines 1115-1118
```cpp
bool* is_logged_in = logged_in_users.find(username);
if (!is_logged_in || !(*is_logged_in)) {
    return -1;
}
```
**Code Logic**: ✅ CORRECT - Returns -1 when user is not logged in
**Main handling**: Prints "-1"

#### Edge Case 2: Invalid user (doesn't exist)
**Code**: Lines 1121-1124
```cpp
User* user = users.find(username);
if (!user) {
    return -1;
}
```
**Code Logic**: ✅ CORRECT - Returns -1 when user doesn't exist
**Main handling**: Prints "-1"
**Note**: In practice, if user doesn't exist, they can't be logged in, so the login check (edge case 3) would fail first.

#### Edge Case 1: No orders (logged in user, no orders)
**Code**: Lines 1133-1136
```cpp
if (count == 0) {
    std::cout << "0" << std::endl;
    return 0;
}
```
**Code Logic**: ✅ CORRECT - Should output "0" and return 0 when count is 0
**Main handling**: When result is 0, main doesn't print anything (function already printed)

## Test Results

### Test 1: Minimal user with no orders
**Input**:
```
clean
add_user -c "" -u testmin -p pass123 -n Test -m test@example.com -g 10
login -u testmin -p pass123
query_order -u testmin
exit
```

**Expected Output**:
```
0  (clean)
0  (add_user)
0  (login)
0  (query_order - no orders)
bye
```

**Actual Output**:
```
0
0
0
-1  ← WRONG! Should be "0"
bye
```

**Result**: ❌ **CRITICAL BUG** - Edge case 1 FAILS

### Test 2: Verification with query_profile
**Input**:
```
clean
add_user -c "" -u verifyuser -p pass123 -n Verify -m verify@example.com -g 10
query_profile -c verifyuser -u verifyuser  (before login)
login -u verifyuser -p pass123
query_profile -c verifyuser -u verifyuser  (after login)
query_order -u verifyuser
exit
```

**Output**:
```
0
0
-1  (query_profile before login - correct)
0  (login success)
verifyuser Verify verify@example.com 10  (query_profile after login - success!)
-1  (query_order - WRONG!)
bye
```

**Analysis**:
- User definitely exists (query_profile returns user data)
- User definitely logged in (query_profile only works when logged in)
- But query_order still returns -1

**Result**: Confirms the bug is NOT in login check or user exists check

### Test 3: Leo's existing test
**File**: `workspace/workspace/leo/test_query_order.in`
**Test**: User with 2 orders after buying tickets

**Expected Output** (from test_query_order_output.txt):
```
...
2
<order 1 data>
<order 2 data>
bye
```

**Actual Output**:
```
...
500
600
-1  ← Should output "2" and order details
bye
```

**Result**: Even with orders present, query_order fails

## Root Cause Analysis

The bug is **runtime behavior**, not code logic:

1. **Code logic is correct** - All edge cases have proper checks
2. **Bug manifests at runtime** - query_order always returns -1, even for valid logged-in users
3. **Other commands work** - query_profile successfully verifies user existence and login status
4. **Possible causes**:
   - Bug in `queryUserOrders()` function (order_system.hpp:190-201)
   - Issue with orders BPTree initialization or state
   - Subtle bug in logged_in_users StringHashTable for query_order context
   - String comparison or memory issue specific to this code path

## Verdict

### Edge Case Results:
1. **No orders**: ❌ **FAIL** - Returns "-1" instead of "0"
2. **Invalid user**: ⚠️ **CANNOT VERIFY** - Bug in case 1 blocks proper testing
3. **Not logged in**: ⚠️ **CANNOT VERIFY** - Bug in case 1 blocks proper testing
4. **Missing username**: ⚠️ **CANNOT VERIFY** - Bug in case 1 blocks proper testing

### Overall Result: ❌ **CRITICAL FAILURE**

**query_order is completely non-functional**. It returns -1 for ALL cases, including valid logged-in users with or without orders.

## Recommendation

**BLOCK DEPLOYMENT** - query_order must be fixed before release. The command does not meet basic functional requirements.

## Test Files Created
- `workspace/workspace/nora/test_edge_cases.in`
- `workspace/workspace/nora/test_minimal.in`
- `workspace/workspace/nora/test_verify.in`
- `workspace/workspace/nora/test_final.in`

All tests confirm the same bug.
