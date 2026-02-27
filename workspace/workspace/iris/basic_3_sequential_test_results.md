# Basic_3 Sequential Test Results with State Persistence

**Test Date:** 2026-02-26
**Test Configuration:** Clean *.dat once before test 1, then run tests 1-5 continuously with state persistence
**Executable:** code (compiled on Feb 26 19:58)

## Test Execution Summary

| Test | Status | Diff Lines | Key Issues |
|------|--------|------------|------------|
| 1    | FAIL   | 12         | Multiple commands returning -1 instead of 0 |
| 2    | FAIL   | 4          | One command returning -1 instead of 0 |
| 3    | FAIL   | 20         | Multiple commands returning -1 instead of 0 |
| 4    | FAIL   | 8          | One -1 error + order status incorrect (pending vs success) |
| 5    | FAIL   | 13         | One -1 error + missing query results + incorrect output |

## Pass Rate Analysis

### Overall Results
- **Total Tests Run:** 5
- **Tests Passed:** 0
- **Tests Failed:** 5
- **Overall Pass Rate:** 0.00%

### Individual Test Pass Rates
(Each test run once in sequential mode)
- Test 1: 0.00% (0/1)
- Test 2: 0.00% (0/1)
- Test 3: 0.00% (0/1)
- Test 4: 0.00% (0/1)
- Test 5: 0.00% (0/1)

## State Persistence Verification

The tests successfully maintained state across runs:

**After Test 1:**
- order_counter.dat: 8B
- orders.dat: 159K
- seats.dat: 120K
- trains.dat: 72K
- users.dat: 16K

**After Test 2:**
- order_counter.dat: 8B (unchanged)
- orders.dat: 307K (grew by 148K)
- seats.dat: 306K (grew by 186K)
- trains.dat: 72K (unchanged)
- users.dat: 18K (grew by 2K)

**After Test 3:**
- order_counter.dat: 8B (unchanged)
- orders.dat: 444K (grew by 137K)
- seats.dat: 583K (grew by 277K)
- trains.dat: 72K (unchanged)
- users.dat: 18K (unchanged)

**After Test 4:**
- order_counter.dat: 8B (unchanged)
- orders.dat: 593K (grew by 149K)
- seats.dat: 824K (grew by 241K)
- trains.dat: 72K (unchanged)
- users.dat: 18K (unchanged)

**After Test 5:**
- order_counter.dat: 8B (unchanged)
- orders.dat: 762K (grew by 169K)
- seats.dat: 1.2M (grew by ~400K)
- trains.dat: 217K (grew by 145K - new trains added)
- users.dat: 18K (unchanged)

State persistence is working correctly - data accumulates across tests as expected.

## Failure Analysis

### Common Pattern: Return Code -1 Instead of 0

The most frequent issue across all tests is commands returning -1 (error) when they should return 0 (success). This suggests:

1. **Validation errors** - Commands are failing validation checks that should pass
2. **State issues** - Commands cannot find or access required data
3. **Logic errors** - Incorrect error handling in command implementation

### Test-Specific Issues

**Test 4 (Line 1418):**
```
Expected: [success] LeavesofGrass 河南省灵宝市 06-28 23:45 -> 河南省偃师市 06-29 03:28 346 15759
Actual:   [pending] LeavesofGrass 河南省灵宝市 06-28 23:45 -> 河南省偃师市 06-29 03:28 346 15759
```
Order status is incorrect - shows [pending] when it should be [success].

**Test 5 (Lines 1424-1425, 1559):**
- Missing two query results for trains
- Line 1559 shows "15169000" instead of "queue"
- Suggests query_order or related command has incorrect output

## Critical Issues Identified

1. **Primary Issue:** Commands returning -1 when they should succeed (affects all tests)
2. **Order Status:** Order status not updating correctly (test 4)
3. **Query Results:** Missing or incorrect query output (test 5)

## Recommendations

1. Investigate why valid commands are returning -1 error codes
2. Review order status update logic (buy_ticket command)
3. Debug query_order and query_ticket commands for missing/incorrect output
4. Add logging to identify which specific commands are failing

## Test Artifacts

All test results saved in: `/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/workspace/workspace/iris/`

Files:
- `basic_3_1_actual.txt` through `basic_3_5_actual.txt` - Actual outputs
- `basic_3_1_diff.txt` through `basic_3_5_diff.txt` - Diff results
- `run_basic_3_sequential.sh` - Test execution script
