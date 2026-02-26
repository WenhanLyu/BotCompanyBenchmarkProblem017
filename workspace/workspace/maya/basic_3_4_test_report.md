# Test Report: query_ticket Implementation - basic_3 and basic_4

**Test Date:** 2026-02-26
**Tester:** Maya
**Component:** query_ticket command implementation

---

## Executive Summary

Both basic_3 and basic_4 test suites **FAIL** with critical bugs in query_ticket and query_order implementations.

| Test Suite | Status | Actual Lines | Expected Lines | Success Rate |
|------------|--------|--------------|----------------|--------------|
| basic_3    | ❌ FAIL | 1,522 | 8,541 | ~18% |
| basic_4    | ❌ FAIL | 5,679 | 46,121 | ~12% |

---

## Critical Issues Found

### 1. query_ticket Returning -1 (Most Common)

**Severity:** CRITICAL
**Frequency:** Hundreds of failures per test suite

Many query_ticket commands that should return valid results are returning -1 (failure).

**Example from basic_3 (around line 161):**
```
Expected: 7142328
Actual:   -1

Expected: 53744575
Actual:   -1
```

**Example Commands:**
- `query_ticket -s 广东省四会市 -t 新疆伊宁市 -d 08-02 -p cost` → Expected price, got -1
- `query_ticket -s 河北省河间市 -t 河南省灵宝市 -d 08-04 -p time` → Expected price, got -1
- `query_ticket -s 广东省花都市 -t 浙江省慈溪市 -d 07-03 -p cost` → Expected price, got -1

### 2. Incorrect Ticket Prices

**Severity:** CRITICAL
**Frequency:** ALL query_ticket results that don't fail

Ticket prices are calculated incorrectly. The program uses the full journey price instead of segment-specific prices.

**Examples from basic_3:**
```
Command: query_ticket (浙江省杭州市 -> 河南省灵宝市)
Expected: LeavesofGrass ... 2295 75921
Actual:   LeavesofGrass ... 2295 98306
Issue: Wrong total price (98306 instead of 75921)

Command: query_ticket (新疆塔城市 -> 黑龙江省海林市)
Expected: LeavesofGrass ... 2486 91170
Actual:   LeavesofGrass ... 2486 98306
Issue: Wrong total price (98306 instead of 91170)
```

**Examples from basic_4:**
```
Command: query_ticket (河北省霸州市 -> 广东省兴宁市)
Expected: LeavesofGrass ... 1203 38482
Actual:   LeavesofGrass ... 1203 45430
Issue: Wrong total price (45430 instead of 38482)

Command: query_ticket (河北省霸州市 -> 黑龙江省齐齐哈尔市)
Expected: LeavesofGrass ... 716 38482
Actual:   LeavesofGrass ... 716 45430
Issue: Wrong total price (45430 instead of 38482)
```

**Pattern:** The actual price (e.g., 98306, 45430) appears to be the full train journey price, not the specific segment price.

### 3. query_order Returning -1

**Severity:** CRITICAL
**Frequency:** All query_order commands fail

All query_order commands are returning -1 instead of showing order history.

**Example from basic_4 (line 166-168):**
```
Expected:
2
[success] LeavesofGrass 黑龙江省齐齐哈尔市 06-18 06:26 -> 四川省成都市 06-18 14:46 1475 8719
[success] LeavesofGrass 湖南省张家界市 06-20 02:32 -> 黑龙江省齐齐哈尔市 06-20 06:17 318 2587

Actual:
-1
```

**Note:** query_order is not being tested in this assignment but the bug is present.

---

## Bug Analysis

### Root Cause 1: query_ticket Logic Errors

Possible causes for -1 returns:
1. **Route finding failure**: Algorithm fails to find valid routes between stations
2. **Date handling**: Date parsing or validation issues
3. **Train filtering**: Released trains not being properly queried from B+ tree
4. **Transfer logic**: Multi-train journeys not being properly handled

### Root Cause 2: Price Calculation Bug

The price calculation is using the wrong formula:
- **Current behavior**: Using full train journey price (station[0] to station[n-1])
- **Expected behavior**: Sum of segment prices only for the queried route

**Formula should be:**
```cpp
// For journey from station i to station j:
total_price = sum of train.prices[k] for k in [i, j-1]
```

**Current implementation likely does:**
```cpp
// Wrong - using full journey price
total_price = train.prices[n-1]  // or sum of all prices
```

### Root Cause 3: query_order Not Implemented

The query_order command is likely not implemented or has critical bugs in order retrieval from the B+ tree.

---

## Impact Assessment

### Data Integrity
- ✅ User management works correctly (add_user, login, query_profile)
- ✅ Train management works correctly (add_train, delete_train, release_train)
- ❌ Ticket queries completely broken
- ❌ Order queries completely broken

### Test Coverage
- **basic_3**: ~82% of output is missing or incorrect
- **basic_4**: ~88% of output is missing or incorrect

Both test suites are dominated by query_ticket and query_order commands, which explains the low success rate.

---

## Recommendations

### Immediate Fixes Required

1. **Fix query_ticket Route Finding**
   - Debug the route-finding algorithm
   - Add logging to identify why valid routes return -1
   - Check train date range validation
   - Verify station name matching

2. **Fix Price Calculation**
   - Recalculate segment-based pricing
   - Ensure prices array is correctly indexed
   - Test with simple 2-station journeys first

3. **Implement/Fix query_order**
   - Complete the query_order implementation
   - Test order retrieval from B+ tree
   - Verify order status display format

### Testing Strategy

1. Create minimal test cases for each bug
2. Fix query_ticket route finding first
3. Fix price calculation second
4. Test with basic_3 and basic_4 incrementally

---

## Sample Correct Output vs Actual

### Basic Commands (Working ✅)
```
Command: add_user -c cur -u I_am_the_admin ...
Expected: 0
Actual:   0  ✅

Command: login -u I_am_the_admin -p awsl
Expected: 0
Actual:   0  ✅

Command: query_profile ...
Expected: I_am_the_admin 奥斯卡 foo@bar.com 10
Actual:   I_am_the_admin 奥斯卡 foo@bar.com 10  ✅
```

### query_ticket (Broken ❌)
```
Command: query_ticket -s 广东省四会市 -t 新疆伊宁市 -d 08-02 -p cost
Expected: 7142328 (valid price)
Actual:   -1  ❌

Command: query_ticket with result
Expected: LeavesofGrass 浙江省杭州市 ... 2295 75921
Actual:   LeavesofGrass 浙江省杭州市 ... 2295 98306  ❌
Issue:    Price 98306 should be 75921
```

### query_order (Broken ❌)
```
Command: query_order -u <username>
Expected: [Order list with count and details]
Actual:   -1  ❌
```

---

## Files Generated

- `basic_3_actual_output.txt` - Full output from basic_3 tests (1,522 lines)
- `basic_4_actual_output.txt` - Full output from basic_4 tests (5,679 lines)
- Expected outputs available at:
  - `./data/017/data/basic_3/out.txt` (8,541 lines)
  - `./data/017/data/basic_4/out.txt` (46,121 lines)

---

## Conclusion

The query_ticket implementation has critical bugs that cause:
1. ~80-90% of queries to fail completely (return -1)
2. 100% of successful queries to return incorrect prices
3. All order queries to fail

**Recommendation:** Focus on debugging query_ticket route finding and price calculation before proceeding with further testing.
