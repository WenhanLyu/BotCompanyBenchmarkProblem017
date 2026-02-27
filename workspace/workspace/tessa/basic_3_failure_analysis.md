# Basic_3 Tests - Failure Pattern Analysis
**Analyst:** Tessa
**Date:** 2026-02-26
**Test Suite:** basic_3 (tests 1-5)

---

## Executive Summary

**CRITICAL FINDING:** The catastrophic test failures (6-8% pass rates) are caused by an incorrect test script, NOT code bugs. When tests are run with proper state persistence, the real pass rates are much higher (88.25% overall).

**REAL CODE ISSUE:** Tests 4 and 5 have ordering bugs in `query_order` command output, causing progressive degradation from 90.39% → 61.68% pass rate.

---

## Test Results Comparison

### With Incorrect Script (deletes .dat between tests)
- Test 1: 99.80% ✅ PASS
- Test 2: 6.63% ❌ CATASTROPHIC FAIL
- Test 3: 8.61% ❌ CATASTROPHIC FAIL
- Test 4: 6.16% ❌ CATASTROPHIC FAIL
- Test 5: 6.62% ❌ CATASTROPHIC FAIL
- **Overall: 23.82%**

### With Correct Script (preserves state)
- Test 1: 99.80% ✅ PASS
- Test 2: 99.21% ✅ NEAR PASS
- Test 3: 98.56% ✅ PASS
- Test 4: 90.39% ⚠️ DEGRADED
- Test 5: 61.68% ❌ FAIL
- **Overall: 88.25%**

---

## Root Cause Analysis

### Issue #1: Test Script Bug (BLOCKER)

**File:** `workspace/workspace/iris/run_sequential_tests.sh` (line 23)

**Problem:**
```bash
# Clean .dat files before each test
rm -f *.dat
```

This deletes persistence files before EACH test, but tests 2-5 are SEQUENTIAL tests that depend on data created in previous tests.

**Impact:** Catastrophic - causes 93% of test 2-5 commands to return -1 (not found)

**Solution:** Use `run_sequential_tests_v2.sh` which only cleans .dat files ONCE at the start

**Status:** CRITICAL - test infrastructure bug, not code bug

---

### Issue #2: query_order Sorting Bug (HIGH PRIORITY)

**Affected Commands:** `query_order` (34 instances in test 4, 47 in test 5)

**Symptom:** Results returned in wrong order

**Example from test 4 (lines 161-163):**

Expected order:
```
[success] LeavesofGrass 浙江省平湖市 07-23 08:59 -> 福建省建阳市 07-23 21:14 2181 19980
[success] puzzletheNewWorld 湖南省岳阳市 07-20 17:01 -> 黑龙江省尚志市 07-20 20:04 426 8140
[success] LeavesofGrass 陕西省安康市 07-12 17:25 -> 广东省花都市 07-13 01:39 1274 9890
```

Actual order:
```
[success] puzzletheNewWorld 湖南省岳阳市 07-20 17:01 -> 黑龙江省尚志市 07-20 20:04 426 8140
[success] LeavesofGrass 陕西省安康市 07-12 17:25 -> 广东省花都市 07-13 01:39 1274 9890
[success] LeavesofGrass 浙江省平湖市 07-23 08:59 -> 福建省建阳市 07-23 21:14 2181 19980
```

**Analysis:** All data is correct, but ordering is wrong. This is a stable_sort issue in the order query system.

**Impact:**
- Test 4: 173 line mismatches (9.6% failure rate)
- Test 5: 792 line mismatches (38.3% failure rate)
- Compounds as more orders accumulate

**Root Cause:** Likely `stable_sort` not being used correctly in order system, or incorrect comparator for multi-field sorting

---

### Issue #3: Minor query_profile Failures (LOW PRIORITY)

**Affected Tests:** Tests 1-3

**Symptom:** Occasional `-1` instead of `0` return values

**Example from test 1:**
- Line 280: Expected `0`, got `-1` (3 instances total)

**Impact:**
- Test 1: 3 failures (0.19%)
- Test 2: 12 failures (0.79%)
- Test 3: 23 failures (1.44%)

**Status:** Minor - likely edge case in permission checks or deleted users

---

## Failure Categorization by Command Type

### Test 4 Command Distribution (1101 commands)
| Command | Count | Failure Rate |
|---------|-------|--------------|
| query_profile | 338 | Low (~0%) |
| query_ticket | 327 | Low (~0%) |
| buy_ticket | 279 | Low (~0%) |
| login | 65 | None |
| modify_profile | 38 | Low (~0%) |
| **query_order** | **34** | **HIGH (~50%)** |
| Other | 20 | Mixed |

### Test 5 Command Distribution (1103 commands)
| Command | Count | Failure Rate |
|---------|-------|--------------|
| query_profile | 317 | Low (~0%) |
| buy_ticket | 317 | Low (~0%) |
| query_ticket | 290 | Low (~0%) |
| login | 68 | None |
| **query_order** | **47** | **VERY HIGH (~85%)** |
| modify_profile | 32 | Low (~0%) |
| Other | 32 | Mixed |

---

## Failure Pattern Summary

### Pattern Type: ORDERING ONLY
- **NOT** missing data errors
- **NOT** permission errors
- **NOT** calculation errors
- **ONLY** wrong order of correct results

### Affected Commands:
1. **query_order** - PRIMARY FAILURE (sorting bug)
2. **query_profile** - MINOR FAILURES (edge cases)

### Progression:
- Tests accumulate more data over time
- Test 5 has most accumulated orders
- Ordering bugs compound with data volume
- Test 5 shows worst failures (61.68% pass)

---

## Priority Rankings

### HIGH PRIORITY (Blockers)
1. **Fix test script** - Use run_sequential_tests_v2.sh instead of run_sequential_tests.sh
   - **Impact:** Fixes 70% of reported failures instantly
   - **Effort:** Zero code changes needed
   - **Quick Win:** ✅ YES

2. **Fix query_order sorting** - Stable sort issue in order system
   - **Impact:** Fixes remaining 9% of failures
   - **Effort:** Code fix required in order query logic
   - **Location:** Order system, likely `stable_sort` comparator

### MEDIUM PRIORITY
3. **Investigate query_profile -1 returns** - Edge case handling
   - **Impact:** Fixes final 1% of failures
   - **Effort:** Debug permission or deletion logic

---

## Recommended Actions

### Immediate (Today)
1. **Switch to correct test script** - Run run_sequential_tests_v2.sh
2. **Document test script requirements** - Prevent future misuse
3. **Verify current pass rate** - Should jump to 88.25%

### Short-term (This Sprint)
4. **Fix query_order sorting bug**
   - Review stable_sort usage in order query code
   - Check multi-field sort comparators
   - Ensure deterministic ordering with ties
   - Target: 98%+ pass rate

### Long-term (Next Sprint)
5. **Fix query_profile edge cases**
   - Debug -1 return conditions
   - Target: 99.5%+ pass rate

---

## Systemic vs Isolated Assessment

**VERDICT: ISOLATED TO SPECIFIC OPERATIONS**

This is NOT a systemic failure. Analysis shows:

✅ **Working correctly:**
- User management (add_user, login, logout)
- Profile queries (99%+ accurate)
- Ticket operations (buy_ticket, query_ticket)
- Train management (add_train, release_train)
- Modifications (modify_profile)

❌ **Isolated failures:**
- query_order sorting only (affects 2 tests, 1 command type)
- Minor edge cases in query_profile (<1% of calls)

**Conclusion:** This is a targeted bug in the order query sorting logic, not a system-wide architecture problem. The fix should be localized to the order system's result sorting code.

---

## Test Environment Notes

- Tests are sequential and cumulative
- State MUST persist between tests
- Test data builds: T1 creates users → T2-3 query/modify → T4-5 heavy order operations
- Incorrect test setup causes false negatives

---

## Files Referenced

### Test Scripts
- `workspace/workspace/iris/run_sequential_tests.sh` - INCORRECT (deletes state)
- `workspace/workspace/iris/run_sequential_tests_v2.sh` - CORRECT (preserves state)
- `workspace/workspace/iris/run_fresh_tests.sh` - For independent testing only

### Test Data
- `data/017/data/basic_3/1.in` through `5.in` - Input commands
- `data/017/data/basic_3/1.out` through `5.out` - Expected outputs

### Test Results
- `workspace/workspace/iris/test_results.txt` - Incorrect script results (23.82%)
- `workspace/workspace/iris/test_results_v2.txt` - Correct script results (88.25%)
- `workspace/workspace/iris/test_*_persist_diff.txt` - Individual test diffs

---

## Next Steps

1. Alert team to use correct test script
2. Assign query_order sorting bug to developer
3. Provide diff examples for debugging
4. Re-test after fix with target 95%+ pass rate
