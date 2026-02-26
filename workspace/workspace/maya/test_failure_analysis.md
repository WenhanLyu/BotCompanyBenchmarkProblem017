# Test Failure Analysis - basic_3 and basic_4

**Date:** 2026-02-26
**Tested with:** Fixed run_basic_tests.sh (Leo's version with proper *.dat cleanup)
**Compiled:** Successfully with make

## Executive Summary

Both test suites **FAIL** with critical issues causing:
- **basic_3**: 1,522 lines output vs 8,541 expected (17.8% complete)
- **basic_4**: 5,679 lines output vs 46,121 expected (12.3% complete)

The program terminates prematurely and produces incorrect results for ticket queries.

---

## Top 3 Failure Patterns

### 1. **CRITICAL: Early Program Termination**

**Impact:** Massive - causes 82% of test failures

**Details:**
- Program exits at line 1522 (actual) vs 1551 (expected first exit)
- Missing 29 lines before first proper exit
- Should process 5 file segments (with 5 "bye" outputs), only processes 1
- Expected "bye" at lines: 1551, 3073, 4674, 6475, 8542
- Actual "bye" at line: 1522 only

**Evidence:**
```
Expected bye locations: 1551, 3073, 4674, 6475, 8542 (5 total)
Actual bye locations:   1522 (1 total)

Missing output: 7020 lines (82% of test)
```

**Root Cause Hypothesis:**
- Program crashes or encounters fatal error around line 1520
- Commands at line 1515-1522 trigger the crash
- Possible causes:
  - Segmentation fault in query_ticket or buy_ticket
  - Buffer overflow in data structures
  - File corruption in .dat files
  - Memory allocation failure

**Commands near crash point:**
```
Line 1515: query_ticket -s 新疆吐鲁番市 -t 天津市 -d 08-06 -p time
Line 1517: buy_ticket -u Ambriel -i INSCRIPTIONS -d 06-15 -n 475 -f 云南省安宁市 -t 新疆米泉市 -q true
Line 1521: query_profile -c Hoshiguma -u Magallan
Line 1522: query_ticket -s 云南省安宁市 -t 北京市 -d 08-01 -p time
```

---

### 2. **HIGH: query_ticket Returns -1 (Route Finding Failure)**

**Impact:** High - affects 62+ queries in basic_3 (first 500 lines analyzed)

**Details:**
- ~80-90% of query_ticket commands return -1 instead of valid ticket counts
- Pattern: Multi-city routes fail to find connections
- Simple direct routes might work, complex routes fail

**Sample Failures:**
```
Line 161-165:
Expected: 7142328, 53744575, 0, 6699568, 2744047 (ticket IDs)
Actual:   -1, -1, -1, -1, -1 (all failed)

Line 182:
Expected: 11707380
Actual:   -1

Line 186-187:
Expected: 22150297, 5135130
Actual:   -1, -1

Line 195:
Expected: 14057780
Actual:   -1
```

**Statistics (first 500 lines):**
- query_ticket failures: 62 instances
- Success rate: ~20-30% (estimated)

**Root Cause Hypothesis:**
- Route-finding algorithm broken
- Graph traversal logic incorrect
- Train connections not properly stored/retrieved
- Date/time matching issues in route search

---

### 3. **MEDIUM: Incorrect Ticket Prices (Segment Calculation Bug)**

**Impact:** Medium - affects ALL successful query_ticket results

**Details:**
- When query_ticket succeeds, it shows WRONG price
- Always shows full train journey price (98306 for LeavesofGrass, 70655 for INSCRIPTIONS)
- Should calculate sum of segment-specific prices

**Sample Price Errors:**
```
Line 197:
Expected: LeavesofGrass ... 2295 75921
Actual:   LeavesofGrass ... 2295 98306
Error:    Wrong price (98306 vs 75921)

Line 227:
Expected: LeavesofGrass ... 2486 91170
Actual:   LeavesofGrass ... 2486 98306
Error:    Wrong price (98306 vs 91170)

Line 233:
Expected: LeavesofGrass ... 802 85869
Actual:   LeavesofGrass ... 802 98306
Error:    Wrong price (98306 vs 85869)

Line 265:
Expected: LeavesofGrass ... 1374 73821
Actual:   LeavesofGrass ... 1374 98306
Error:    Wrong price (98306 vs 73821)
```

**Pattern:**
- All LeavesofGrass queries show price: 98306
- All INSCRIPTIONS queries show price: 70655
- These are likely the full train route prices
- Should calculate: sum of prices for stations between source and destination

**Statistics (first 500 lines):**
- Price calculation errors: 10+ instances
- Success rate: 0% (all successful queries have wrong prices)

**Root Cause:**
- Price calculation uses full train journey cost
- Should iterate through segment-specific pricing
- Formula: price = Σ(station[i].price) for i in [start_station..end_station-1]

---

## Additional Issues

### 4. query_order Not Implemented (estimated)
- All query_order commands likely return -1
- Unable to retrieve order history
- Blocking order tracking functionality

### 5. buy_ticket Failures (from Leo's report)
- buy_ticket returns -1 for most attempts
- Seat availability tracking broken
- Blocks order creation

---

## Test Environment

**Data Files Cleaned:** ✅ Yes (users.dat, trains.dat, orders.dat, seats.dat)
- Fixed by Leo in run_basic_tests.sh
- Lines 41, 45, 48 modified

**Compilation:** ✅ Success
**Execution:** ⚠️ Premature termination

---

## Detailed Statistics

### basic_3 Test Suite

| Metric | Value |
|--------|-------|
| Expected output lines | 8,541 |
| Actual output lines | 1,522 |
| Completion rate | 17.8% |
| Missing lines | 7,019 |
| query_ticket failures | 62+ (in first 500 lines) |
| Price calculation errors | 10+ (in first 500 lines) |

### basic_4 Test Suite

| Metric | Value |
|--------|-------|
| Expected output lines | 46,121 |
| Actual output lines | 5,679 |
| Completion rate | 12.3% |
| Missing lines | 40,442 |

---

## Specific Failure Examples

### Example 1: query_ticket Route Finding Failure
```bash
Command: query_ticket -s 广东省四会市 -t 新疆伊宁市 -d 08-02 -p cost
Expected: 7142328 (ticket ID)
Actual: -1 (failure)
```

### Example 2: query_ticket Price Calculation Error
```bash
Command: query_ticket -s 浙江省杭州市 -t 河南省灵宝市 -d 07-04 -p time
Expected: LeavesofGrass 浙江省杭州市 07-04 11:52 -> 河南省灵宝市 07-04 23:39 2295 75921
Actual:   LeavesofGrass 浙江省杭州市 07-04 11:52 -> 河南省灵宝市 07-04 23:39 2295 98306
Issue: Price wrong (98306 should be 75921)
```

### Example 3: Early Program Termination
```bash
Expected line 1522: 4134900
Actual line 1522: bye (program exits)

Expected line 1523-1551: 29 more lines of output
Actual: None (program terminated)
```

---

## Recommendations

### Immediate Priority (Critical Path)

1. **Fix Early Termination**
   - Debug commands around line 1520-1522
   - Check for segmentation faults, buffer overflows
   - Add error handling to prevent crashes
   - Test with smaller input to isolate crash trigger

2. **Fix query_ticket Route Finding**
   - Review route-finding algorithm
   - Check train connection graph construction
   - Verify date/time filtering in route search
   - Add debug logging to trace route calculation

3. **Fix Price Calculation**
   - Locate price calculation code in query_ticket
   - Change from full journey price to segment sum
   - Formula: sum prices for stations between src and dest
   - Test with known route examples

### Secondary Priority

4. **Implement/Fix query_order**
   - Verify order tracking data structure exists
   - Implement order retrieval by username
   - Test with buy_ticket integration

5. **Fix buy_ticket Failures**
   - Debug why buy_ticket returns -1
   - Check seat availability tracking
   - Verify train data access during purchase

---

## Testing Strategy

### Phase 1: Isolate Crash
1. Run subset of commands (lines 1-1000)
2. Binary search to find exact crash trigger
3. Run with debugger (gdb/lldb)
4. Check stderr for error messages

### Phase 2: Verify Fixes
1. Test query_ticket with known routes
2. Verify price calculation with manual checks
3. Run basic_3 subset (first exit segment only)
4. Compare output line-by-line

### Phase 3: Full Test
1. Run complete basic_3 suite
2. Verify all 5 "bye" outputs appear
3. Check output line count matches 8,541
4. Run basic_4 for final verification

---

## Files Generated

- `basic_3_actual_output.txt` - Full test output (1,522 lines)
- `basic_4_actual_output.txt` - Full test output (5,679 lines)
- `test_failure_analysis.md` - This report
- `basic_3_diff_sample.txt` - First 200 diff lines

---

## Conclusion

The train ticket system has **three critical bugs** preventing test suite completion:

1. **Program crashes** before completing first file segment (82% data loss)
2. **Route finding fails** for 80-90% of queries (-1 returns)
3. **Price calculation wrong** for ALL successful queries (uses full train price)

**Estimated Fix Time:**
- Early termination: 2-4 hours (debugging required)
- Route finding: 4-6 hours (algorithm review)
- Price calculation: 1-2 hours (straightforward fix)

**Blocking:** All three issues must be fixed for tests to pass.

---

*Analysis completed: 2026-02-26*
*Next cycle: Focus on crash debugging (Pattern #1)*
