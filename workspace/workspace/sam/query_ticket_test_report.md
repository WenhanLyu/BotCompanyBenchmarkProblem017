# Query_Ticket Test Report

**Date:** 2026-02-26
**Tester:** Sam
**Issue:** #24 - Test query_ticket implementation

## Executive Summary

Tested query_ticket implementation against basic_3 (5 tests) and basic_4 (5 tests). **All tests FAIL** due to critical price calculation bug.

### Critical Bug Found

**Bug:** query_ticket always returns base train price (98306) instead of cumulative price based on seat availability

**Impact:**
- basic_3: Massive test failures with incorrect ticket prices
- basic_4: Similar price calculation errors
- This affects ALL query_ticket operations

## Test Environment

- **Build:** ✅ Successful (cmake + make)
- **Executable:** `code` (80KB)
- **Test Data:**
  - basic_3: 5 test files (85K-75K each)
  - basic_4: 5 test files (317K-302K each)

## Test Results

### basic_3 Tests (query_ticket focus)

| Test | Lines | Status | Key Issues |
|------|-------|--------|------------|
| 1.in | 1522  | ❌ FAIL | Price calculation incorrect |
| 2.in | 1422  | ❌ FAIL | Price calculation incorrect |
| 3.in | 1420  | ❌ FAIL | Price calculation incorrect |
| 4.in | 1445  | ❌ FAIL | Price calculation incorrect |
| 5.in | 1415  | ❌ FAIL | Price calculation incorrect |

**Overall:** 0/5 tests pass

### basic_4 Tests

| Test | Lines | Status | Key Issues |
|------|-------|--------|------------|
| 1.in | 4911  | ❌ FAIL | Multiple issues including price calc |
| 2.in | 5405  | ❌ FAIL | Multiple issues |
| 3.in | 5392  | ❌ FAIL | Multiple issues |
| 4.in | 5397  | ❌ FAIL | Multiple issues |
| 5.in | 5501  | ❌ FAIL | Multiple issues |

**Overall:** 0/5 tests pass

## Detailed Analysis

### Price Calculation Bug

**Example from basic_3/1.in:**

```
Command: query_ticket -s 浙江省杭州市 -t 河南省灵宝市 -d 07-04 -p time
Expected: LeavesofGrass 浙江省杭州市 07-04 11:52 -> 河南省灵宝市 07-04 23:39 2295 75921
Actual:   LeavesofGrass 浙江省杭州市 07-04 11:52 -> 河南省灵宝市 07-04 23:39 2295 98306
```

**Pattern:**
- Expected prices vary: 75921, 91170, 85869, 73821, 77723, 61404, etc.
- Actual prices are ALWAYS: 98306 (the base train price)

**Root Cause:**
The implementation returns the base train price for the entire route instead of calculating the cumulative cost based on:
1. Number of seats requested
2. Seats already purchased on each segment
3. Progressive pricing based on seat numbers

### Sample Differences

From basic_3/1.in comparison:

```
< LeavesofGrass 浙江省杭州市 07-04 11:52 -> 河南省灵宝市 07-04 23:39 2295 75921
> LeavesofGrass 浙江省杭州市 07-04 11:52 -> 河南省灵宝市 07-04 23:39 2295 98306

< LeavesofGrass 新疆塔城市 08-04 07:38 -> 黑龙江省海林市 08-04 22:46 2486 91170
> LeavesofGrass 新疆塔城市 08-04 07:38 -> 黑龙江省海林市 08-04 22:46 2486 98306

< LeavesofGrass 浙江省平湖市 07-13 08:59 -> 辽宁省抚顺市 07-13 14:21 802 85869
> LeavesofGrass 浙江省平湖市 07-13 08:59 -> 辽宁省抚顺市 07-13 14:21 802 98306
```

**All prices are 98306 instead of varying based on seat availability**

### Additional Issues (basic_4)

basic_4 tests show additional problems:
- Extra output line at start (line 1: "-1")
- Misaligned output (suggests query_order issues)
- Profile information shows wrong privilege levels

## Technical Details

### Query Format Tested

```
query_ticket -s <from> -t <to> -d <date> -p <cost|time>
```

### Expected Output Format

```
<count>
<train_id> <from> <depart_date> <depart_time> -> <to> <arrive_date> <arrive_time> <cost> <cumulative_price>
[... more trains if count > 1 ...]
```

### Key: The Last Field

The last field in each train line should be the **cumulative price** calculated based on:
- Starting seat number (based on tickets already sold)
- Number of seats requested
- Progressive pricing: seats cost 1, 2, 3, ... i.e., seat i costs i yuan
- Sum across all segments from source to destination

## Build Issues Fixed

**Issue:** CMakeLists.txt included test files with duplicate main() functions
**Fix:** Modified CMakeLists.txt to exclude test_*.cpp files using list(FILTER)

```cmake
# Exclude test files from the main build
list(FILTER SOURCES EXCLUDE REGEX ".*test_.*\\.cpp$")
```

## Files Generated

All test outputs saved to `/workspace/workspace/sam/`:

**basic_3:**
- basic_3_1_actual.txt through basic_3_5_actual.txt
- basic_3_1_expected.txt

**basic_4:**
- basic_4_1_actual.txt through basic_4_5_actual.txt
- basic_4_1_expected.txt

**Analysis:**
- analyze_query_ticket.sh

## Recommendations

### Immediate Actions Required

1. **Fix cumulative price calculation in query_ticket**
   - Implement proper seat-based pricing
   - Calculate price as sum of seat costs across all segments
   - Formula: for seat range [start, start+num), price = sum(start to start+num-1)

2. **Review BPTree ticket storage**
   - Verify seat count tracking per segment
   - Ensure proper seat number calculation

3. **Fix query_order** (secondary issue)
   - Many query_order commands return -1 instead of prices
   - Affects basic_3 and basic_4 tests

4. **Re-test after fixes**
   - Run all 10 tests again (basic_3 + basic_4)
   - Verify price calculations match expected output exactly

### Testing Strategy

After fixes, verify:
- ✅ Price calculation matches spec
- ✅ Seat availability tracked correctly
- ✅ Multi-segment routes priced correctly
- ✅ Time/cost sorting works
- ✅ query_order returns correct prices

## Conclusion

**Status:** ❌ **FAILING**
**Blocker:** Critical price calculation bug in query_ticket implementation

The query_ticket command is implemented but has a fundamental bug where it returns the base train price instead of calculating the cumulative price based on seat availability. This causes 100% test failure rate.

**Next Steps:** Assign bug to development team for immediate fix.
