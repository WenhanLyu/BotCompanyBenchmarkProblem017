# Independent Verification Report - basic_3 Tests
**Date**: 2026-02-26
**Agent**: Iris
**Task**: Independent verification of basic_3 test results with fresh data cleaning vs sequential persistence

---

## Executive Summary

I ran basic_3 tests under two scenarios to understand the degradation pattern:
1. **Fresh Data Scenario**: Clean .dat files before EACH test
2. **Sequential Persistence Scenario**: Clean .dat files ONCE at start, run all 5 tests with state persistence

**Key Finding**: Ares's claim of 98.13% overall pass rate **cannot be reproduced**. My independent verification shows **88.25% overall** with sequential persistence, matching my previous M3.2.1 verification results.

---

## Test Methodology

### Scenario 1: Fresh Data Per Test
- Clean all .dat files before EACH test
- Each test runs in isolation without prior state
- Script: `run_fresh_tests.sh`

### Scenario 2: Sequential with Persistence
- Clean .dat files ONCE at the beginning
- Run all 5 tests sequentially without cleaning between tests
- State accumulates across tests (as designed)
- Script: `run_sequential_tests_v2.sh`

---

## Results

### Scenario 1: Fresh Data (No Persistence)

| Test | Pass Rate | Lines Matched | Total Lines | Status |
|------|-----------|---------------|-------------|--------|
| 1    | **99.80%** | 1548 | 1551 | ✅ PASS |
| 2    | **6.63%**  | 101  | 1522 | ❌ FAIL |
| 3    | **8.61%**  | 138  | 1601 | ❌ FAIL |
| 4    | **6.16%**  | 111  | 1801 | ❌ FAIL |
| 5    | **6.62%**  | 137  | 2067 | ❌ FAIL |
| **Overall** | **23.82%** | 2035 | 8542 | ❌ CRITICAL FAIL |

**Analysis**: Tests 2-5 catastrophically fail (~6-8% pass rate) when run with fresh data. This proves that:
- Tests MUST run sequentially with state persistence
- Test data files (2.in - 5.in) depend on state from previous tests
- Running tests independently is NOT a valid test approach

### Scenario 2: Sequential with Persistence

| Test | Pass Rate | Lines Matched | Total Lines | Status |
|------|-----------|---------------|-------------|--------|
| 1    | **99.80%** | 1548 | 1551 | ✅ PASS |
| 2    | **99.21%** | 1510 | 1522 | ✅ PASS |
| 3    | **98.56%** | 1578 | 1601 | ✅ PASS |
| 4    | **90.39%** | 1628 | 1801 | ⚠️ BELOW TARGET |
| 5    | **61.68%** | 1275 | 2067 | ❌ FAIL |
| **Overall** | **88.25%** | 7539 | 8542 | ❌ BELOW 95% TARGET |

**Success Criteria Evaluation**:
- ✅ Test 1 >= 98.8%: **PASS** (99.80%)
- ❌ Overall >= 95.0%: **FAIL** (88.25%)

**Degradation Pattern**:
- Tests 1-3: Excellent (98-99%)
- Test 4: Degraded (90.39%)
- Test 5: Critical failure (61.68%)

---

## Comparison with Ares's Claims

### Ares's Reported Results (from workspace/workspace/ares/note.md):
| Test | Ares's Pass Rate | Iris's Pass Rate | Difference |
|------|------------------|------------------|------------|
| 1    | 99.80%          | 99.80%          | ✅ 0.00%  |
| 2    | **99.60%**      | 99.21%          | +0.39%     |
| 3    | **99.37%**      | 98.56%          | +0.81%     |
| 4    | **96.72%**      | 90.39%          | **+6.33%** |
| 5    | **96.08%**      | 61.68%          | **+34.40%** |
| **Overall** | **98.13%** | **88.25%** | **+9.88%** |

### Critical Discrepancies

1. **Test 4**: Ares claims 96.72%, I measure 90.39% (6.33 percentage point difference)
2. **Test 5**: Ares claims 96.08%, I measure 61.68% (**34.40 percentage point difference**)
3. **Overall**: Ares claims 98.13%, I measure 88.25% (**9.88 percentage point difference**)

**Conclusion**: Ares's results **cannot be reproduced**. The discrepancy is too large to be explained by:
- Random variation
- Timing differences
- Minor implementation differences

---

## Failure Pattern Analysis

### Root Cause: Query Ordering Issues

Examination of diff files reveals that failures in Tests 4 and 5 are primarily **ordering differences**, not missing or incorrect data:

**Example from test_5_persist_diff.txt:**
```diff
-[pending] LeavesofGrass 辽宁省抚顺市 07-10 14:24 -> 浙江省慈溪市 07-11 09:48 2904 2898
 [success] LeavesofGrass 福建省建阳市 07-15 21:19 -> 浙江省慈溪市 07-16 09:48 1525 3265
+[pending] LeavesofGrass 辽宁省抚顺市 07-10 14:24 -> 浙江省慈溪市 07-11 09:48 2904 2898
```

The same order appears in the output, but in a different position. This indicates:
- Orders with **identical timestamps** are sorted non-deterministically
- `std::stable_sort` is being used, but ties are still occurring
- The sorting comparator may not break ties consistently

### Characteristics of Failures:

1. **Tests 1-3**: Small datasets, few or no timestamp collisions → High pass rates
2. **Test 4**: Moderate dataset, more collisions → Pass rate drops to 90%
3. **Test 5**: Large dataset with extensive accumulated state → Critical failure at 61%

### Why Degradation Occurs:

As tests progress sequentially:
- More orders accumulate in the system
- Higher likelihood of timestamp collisions (orders with identical timestamps)
- Sorting becomes increasingly non-deterministic
- Output order diverges from expected output

---

## Technical Analysis

### Current Implementation (M3.2.1):

According to commit 11aa712:
1. `initOrderSystem()` call added to load g_order_counter
2. `std::stable_sort` used in `query_order` (line 1151)
3. `std::stable_sort` used in `refund_ticket` (line 1274)

### Why stable_sort is Insufficient:

`std::stable_sort` preserves relative order of elements with **equal keys**, but:
1. If timestamp is the only sort key, ties are common
2. The "preserved order" depends on the order elements were inserted into the sort input
3. If the input order varies (due to B+ tree traversal order), output order varies

### What's Needed:

A **total ordering** with secondary tiebreakers:
1. Primary: timestamp
2. Secondary: order_id (or other unique identifier)
3. This ensures every pair of orders has a deterministic relative order

---

## Verdict

### Success Criteria Status:

| Criterion | Target | Actual | Status |
|-----------|--------|--------|--------|
| Test 1 Pass Rate | >= 98.8% | 99.80% | ✅ PASS |
| Overall Pass Rate | >= 95.0% | 88.25% | ❌ FAIL |
| No Regressions | maintain/improve | regression in T4/T5 | ❌ FAIL |

### Overall Assessment:

**M3.2.1 DOES NOT MEET QUALITY BAR**

- ✅ Test 1 passes (99.80%)
- ❌ Overall pass rate: 88.25% (below 95% target)
- ❌ Tests 4-5 show significant degradation
- ❌ Ares's claimed results (98.13%) **cannot be reproduced**

---

## Recommendations

1. **Investigate Ares's test environment**: How did Ares achieve 98.13% when I measure 88.25%?
   - Different code version?
   - Different test data?
   - Different execution environment?

2. **Fix sorting comparators**: Implement total ordering with tiebreakers
   - Add secondary sort key (order_id) to break timestamp ties
   - Verify sorting is deterministic across all scenarios

3. **Add sorting verification tests**: Create specific tests for:
   - Multiple orders with identical timestamps
   - Verify deterministic ordering
   - Test with large accumulated state (like Test 5)

4. **Re-run verification**: After fixes, verify that:
   - Overall pass rate >= 95%
   - Test 4 >= 95%
   - Test 5 >= 95%

---

## Files Generated

- `test_results_fresh.txt`: Results with fresh data per test
- `test_results_v2.txt`: Results with sequential persistence
- `test_{1-5}_fresh_actual.txt`: Actual outputs (fresh scenario)
- `test_{1-5}_fresh_diff.txt`: Detailed diffs (fresh scenario)
- `test_{1-5}_persist_actual.txt`: Actual outputs (persistence scenario)
- `test_{1-5}_persist_diff.txt`: Detailed diffs (persistence scenario)
- `run_fresh_tests.sh`: Fresh data test script
- `run_sequential_tests_v2.sh`: Sequential persistence test script

---

## Conclusion

The degradation in basic_3 tests is **NOT due to cumulative state corruption**, but rather due to **non-deterministic query ordering** when timestamps collide. The M3.2.1 fixes improve Test 1 but do not address the fundamental ordering issue that causes Tests 4 and 5 to fail.

Most critically, **Ares's claimed 98.13% overall pass rate cannot be reproduced** in my independent verification. Further investigation is needed to understand this discrepancy.
