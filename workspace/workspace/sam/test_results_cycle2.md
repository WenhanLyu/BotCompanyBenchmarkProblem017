# Test Results - Cycle 2: query_ticket Comprehensive Testing

## Test Execution Summary

**Date:** 2026-02-26  
**Script:** run_basic_tests.sh (with .dat file cleaning)  
**Build:** Clean build completed successfully

## Overall Results

### basic_3 (Query Ticket Focus)
- **Status:** ❌ FAIL
- **Expected lines:** 8,541
- **Actual lines:** 1,522 (17.8% of expected)
- **Missing output:** 7,019 lines (82.2%)

### basic_4 (Comprehensive System Test)
- **Status:** ❌ FAIL
- **Expected lines:** 46,121
- **Actual lines:** 5,679 (12.3% of expected)
- **Missing output:** 40,442 lines (87.7%)

## Critical Bugs Identified

### Bug #1: query_ticket Price Calculation (CRITICAL)

**Issue:** Returns incorrect fixed prices instead of cumulative seat-based pricing

**Evidence from basic_3:**
```
Line 197:
Expected: LeavesofGrass ... 2295 75921
Actual:   LeavesofGrass ... 2295 98306  ← Wrong by 22,385 yuan

Line 227:
Expected: LeavesofGrass ... 2486 91170
Actual:   LeavesofGrass ... 2486 98306  ← Wrong by 7,136 yuan

Line 233:
Expected: LeavesofGrass ... 802 85869
Actual:   LeavesofGrass ... 802 98306   ← Wrong by 12,437 yuan

Line 265:
Expected: LeavesofGrass ... 1374 73821
Actual:   LeavesofGrass ... 1374 98306  ← Wrong by 24,485 yuan
```

**Evidence from basic_4:**
```
Line 162:
Expected: LeavesofGrass ... 3000 45430
Actual:   LeavesofGrass ... 3000 98306  ← Wrong by 52,876 yuan

Line 219:
Expected: LeavesofGrass ... 1203 38482
Actual:   LeavesofGrass ... 1203 45430  ← Wrong by 6,948 yuan
```

**Pattern:** Returns base train price (98306 or 45430) instead of calculating based on:
- Seats already sold (affects starting seat number)
- Progressive pricing formula: seat i costs i yuan
- Formula: price = sum(i=start to start+num-1) of i

### Bug #2: query_order Returns Garbage Instead of -1 (CRITICAL)

**Issue:** Returns random large numbers instead of -1 for non-existent orders

**Evidence:**
```
Lines 150-151:
Expected: -1, -1
Actual:   822666, 4802672  ← Garbage values

Lines 159-160:
Expected: -1, -1
Actual:   12860525, 6663132  ← Garbage values

Line 187:
Expected: -1
Actual:   16986411  ← Garbage value
```

**Root Cause:** Likely uninitialized variable or incorrect return path in query_order

### Bug #3: Missing query_order Details (CRITICAL)

**Issue:** query_order fails to return order details when orders exist

**Evidence from basic_4, lines 165-168:**
```
Expected:
LeavesofGrass 湖南省张家界市 07-19 02:32 -> 广东省兴宁市 07-19 08:09 805 45430
2
[success] LeavesofGrass 黑龙江省齐齐哈尔市 06-18 06:26 -> 四川省成都市 06-18 14:46 1475 8719
[success] LeavesofGrass 湖南省张家界市 06-20 02:32 -> 黑龙江省齐齐哈尔市 06-20 06:17 318 2587

Actual:
LeavesofGrass 湖南省张家界市 07-19 02:32 -> 广东省兴宁市 07-19 08:09 805 98306
-1
```

**Impact:** query_order not showing ticket details, just returning -1

## Impact Analysis

### Output Completeness
- **basic_3:** Missing 82% of expected output
- **basic_4:** Missing 88% of expected output

This suggests the program is crashing, hanging, or failing silently partway through execution.

### Test Case Progression
Both test suites show failure from early in execution, indicating fundamental issues with:
1. Price calculation logic in query_ticket
2. Order tracking/retrieval in query_order
3. Possibly data persistence or state management

## Root Cause Hypotheses

1. **query_ticket price bug:** Not tracking sold seats or using incorrect price calculation formula
2. **query_order garbage values:** Uninitialized return variable or memory corruption
3. **Missing output:** Possible crash/hang when processing certain commands, or early exit

## Recommended Next Steps

### Immediate Priority (P0)
1. Fix query_ticket cumulative price calculation
   - Track seats sold per train segment
   - Implement progressive pricing: seat i costs i yuan
   - Test: buy tickets, then query to verify price increases

2. Fix query_order return value handling
   - Initialize return variables properly
   - Return -1 for non-existent orders (not garbage)
   - Return proper order details when orders exist

### Secondary Priority (P1)
3. Investigate missing output issue
   - Check for crashes/exceptions in test execution
   - Verify all commands complete successfully
   - Add error logging to identify failure points

## Test Environment

- **OS:** macOS (Darwin 24.6.0)
- **Compiler:** Apple Clang via CMake
- **Note:** `timeout` command not available on macOS (replaced with manual execution)

## Files Generated

- `basic_3_actual_output.txt` (1,522 lines)
- `basic_4_actual_output.txt` (5,679 lines)
- `test_results_cycle2.md` (this file)

---
**Test Engineer:** Sam  
**Status:** Testing complete, critical bugs identified  
**Next:** Development team to implement fixes
