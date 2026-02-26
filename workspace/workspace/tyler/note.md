# Tyler's Workspace - Test Execution Notes

## Current Task: Query_Order Edge Case Testing (2026-02-26)

### Test Results Summary

All edge case tests for query_order command: ✅ **PASS**

#### Edge Cases Tested

1. **Invalid user (user doesn't exist)**: ✅ PASS
   - Returns -1 as expected

2. **User not logged in**: ✅ PASS
   - Returns -1 as expected

3. **User logged in but no orders**: ✅ PASS
   - Returns 0 as expected

4. **User with one order**: ✅ PASS
   - Displays count and order details correctly

5. **Multiple orders (sorting verification)**: ✅ PASS
   - Orders displayed in chronological order by timestamp

6. **Different order statuses**: ✅ PASS
   - Both "success" and "pending" statuses displayed correctly

#### Output Format: ✅ PASS
Format: [timestamp] [trainID] [from] [to] [status] [price] [count]

#### Issues Found
None. All edge cases pass successfully.

#### Overall Result
✅ **PASS** - query_order command handles all edge cases correctly

### Test Files Created
- `query_order_final_test.in` - Comprehensive edge case test
- `query_order_final_actual.txt` - Test output
- `analyze_output.sh` - Output analysis script

### Notes
- refund_ticket command not implemented, so "refunded" status not tested
- All three milestone-specified edge cases verified
- Additional testing confirmed sorting and status variations work correctly

---

## Previous Task: basic_2 Test Execution

### Summary
- Compiled project successfully
- Executed basic_2 test (data/017/data/basic_2/1.in)
- Captured full output in basic_2_test_results.txt
- Analyzed all train command results

### Key Findings
- 10/12 add_train succeeded (83.3%)
- 10/14 release_train succeeded (71.4%)
- 1/4 query_train succeeded (25%)
- 0/1 delete_train succeeded (0%)

### Files Created
- `train_commands_summary.txt` - Detailed analysis
- `basic_2_test_results.txt` - Full program output
- `analyze_train_output.sh` - Analysis script
