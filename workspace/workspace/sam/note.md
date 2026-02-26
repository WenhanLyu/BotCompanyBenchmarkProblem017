# Sam - Test Engineer - Cycle Report

## Current Assignment
Issue #24 - Test query_ticket implementation against basic_3 and basic_4

## Summary
✅ **Assignment Complete** - Tested all 10 test cases (basic_3: 5, basic_4: 5)
❌ **Result: ALL TESTS FAIL** - Critical bug found in query_ticket price calculation

## Critical Bug Found

**Bug:** query_ticket always returns base train price (98306) instead of cumulative price

**Evidence:**
- Expected prices vary: 75921, 91170, 85869, 73821, 77723, etc.
- Actual prices are ALWAYS: 98306

**Root Cause:**
The implementation returns the base train price instead of calculating cumulative cost based on:
1. Number of seats requested
2. Seats already purchased (affects starting seat number)
3. Progressive pricing: seat i costs i yuan

**Formula:** For seat range [start, start+num), price = sum(i=start to start+num-1) of i

## Test Results

### basic_3 (Query Ticket Focus)
- 5 tests run: all.in files (1.in through 5.in)
- Output: 1522, 1422, 1420, 1445, 1415 lines
- Status: 0/5 pass (100% failure rate)

### basic_4 (Comprehensive)
- 5 tests run: all.in files (1.in through 5.in)
- Output: 4911, 5405, 5392, 5397, 5501 lines
- Status: 0/5 pass (100% failure rate)
- Additional issues: query_order also failing, alignment problems

## Build Issues Fixed

**Issue:** CMakeLists.txt included test_*.cpp files causing duplicate main() errors

**Fix Applied:**
```cmake
list(FILTER SOURCES EXCLUDE REGEX ".*test_.*\\.cpp$")
```

Build now succeeds cleanly.

## Files Created

**Test Outputs:**
- basic_3_1_actual.txt through basic_3_5_actual.txt
- basic_4_1_actual.txt through basic_4_5_actual.txt
- basic_3_1_expected.txt, basic_4_1_expected.txt

**Reports:**
- query_ticket_test_report.md (comprehensive test report)
- analyze_query_ticket.sh (analysis script)

## Example Diff

```
Command: query_ticket -s 浙江省杭州市 -t 河南省灵宝市 -d 07-04 -p time
Expected: LeavesofGrass ... 2295 75921
Actual:   LeavesofGrass ... 2295 98306  ← WRONG! Should be 75921
```

## Actions Taken

1. ✅ Built project successfully
2. ✅ Ran all 10 tests (basic_3 and basic_4)
3. ✅ Analyzed outputs and identified bug pattern
4. ✅ Created comprehensive test report
5. ✅ Documented findings for development team

## Next Steps for Development Team

1. **Fix cumulative price calculation** in query_ticket
   - Implement seat-based progressive pricing
   - Sum seat costs across all segments
   - Test with: buy tickets, then query to verify price increases

2. **Fix query_order** (secondary issue)
   - Returns -1 when it should return prices

3. **Re-test** after fixes with all 10 test cases

## Status
✅ Testing complete
📝 Report submitted
🐛 Critical bug identified and documented
⏭️ Awaiting development team fix
