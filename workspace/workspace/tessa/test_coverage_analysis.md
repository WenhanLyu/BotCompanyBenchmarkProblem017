# Test Coverage Analysis Report
**Date**: 2026-02-26
**Analyst**: Tessa
**Scope**: All test files in data/017/data/

---

## Executive Summary

We have 6 test suites ranging from basic_1 (1,135 commands, user management only) to basic_6 (112,109 commands, full system stress test). **We can currently run basic_1 through basic_3**, but **basic_4-6 will fail** due to missing query_transfer implementation.

**Key Findings**:
- ✅ **basic_1**: 100% runnable (user management only, 0 query_transfer commands)
- ✅ **basic_2**: 100% runnable (+ ticket system, 4 query_transfer = 0.24% of commands)
- ✅ **basic_3**: 99.79% runnable (12 query_transfer = 0.21% of commands)
- ❌ **basic_4**: 99.65% runnable (73 query_transfer = 0.35% of commands)
- ❌ **basic_5**: 99.67% runnable (137 query_transfer = 0.33% of commands)
- ❌ **basic_6**: 99.65% runnable (392 query_transfer = 0.35% of commands)

**Testing Strategy**: Focus on basic_3 for M3.2.2 verification. Use basic_4 samples to test query_transfer implementation for M4.

---

## Detailed Test Breakdown

### basic_1: User Management Only
- **Total Commands**: 1,135
- **Test Files**: 1 (1.in)
- **Output Files**: None (no expected output)
- **query_transfer**: 0 (0%)
- **Status**: ✅ **Can run now** (M1 complete)

**Command Distribution**:
```
675 query_profile    (59.5%)
196 login            (17.3%)
132 add_user         (11.6%)
72  logout           (6.3%)
58  modify_profile   (5.1%)
```

**Testing Value**:
- Validates user management system
- No ticket operations
- No persistence testing across multiple files
- Already verified at 100% pass rate (M1 completion)

---

### basic_2: User + Ticket System
- **Total Commands**: 1,645
- **Test Files**: 1 (1.in)
- **Output Files**: None (no expected output)
- **query_transfer**: 4 (0.24%)
- **Status**: ✅ **Can run now** (with 4 failures)

**Command Distribution**:
```
470 query_ticket     (28.6%)
466 buy_ticket       (28.3%)
294 query_profile    (17.9%)
145 login            (8.8%)
123 add_user         (7.5%)
43  query_order      (2.6%)
34  modify_profile   (2.1%)
28  logout           (1.7%)
6   refund_ticket    (0.4%)
4   query_transfer   (0.24%) ❌
```

**Testing Value**:
- First test with buy_ticket and query_ticket
- Small scale, easy to debug
- **4 query_transfer failures** = 0.24% impact
- Good for testing M2-M3.2 commands
- Cannot verify without expected output file

---

### basic_3: Comprehensive Ticket System
- **Total Commands**: 5,651
- **Test Files**: 5 (1.in - 5.in)
- **Output Files**: 5 (1.out - 5.out) ✅
- **query_transfer**: 12 (0.21%)
- **Status**: ✅ **Can run now** (with 12 failures)

**Command Distribution**:
```
1,560 query_profile  (27.6%)
1,533 query_ticket   (27.1%)
1,505 buy_ticket     (26.6%)
440   login          (7.8%)
168   modify_profile (3.0%)
140   query_order    (2.5%)
134   add_user       (2.4%)
76    logout         (1.3%)
20    refund_ticket  (0.4%)
12    query_transfer (0.21%) ❌
```

**Testing Value**:
- **PRIMARY TEST for M3.2.2 verification**
- Has expected output files for all 5 tests
- Currently at 88.25% pass rate (M3.2.1 regression)
- Target: 95%+ overall, 98.8%+ on test 1
- **12 query_transfer failures** = 0.21% impact (negligible)
- Tests state persistence across 5 sequential runs
- Good balance of all commands

**Current Use**: Active testing for M3.2.2 (query_order fix)

---

### basic_4: High Volume Ticket Operations
- **Total Commands**: 20,651
- **Test Files**: 5 (1.in - 5.in)
- **Output Files**: None (no expected output yet)
- **query_transfer**: 73 (0.35%)
- **Status**: ❌ **Will fail** without query_transfer

**Command Distribution**:
```
6,989 buy_ticket      (33.8%)
6,930 query_ticket    (33.6%)
3,920 query_profile   (19.0%)
837   login           (4.1%)
702   query_order     (3.4%)
464   modify_profile  (2.2%)
291   logout          (1.4%)
134   add_user        (0.6%)
73    query_transfer  (0.35%) ❌
56    refund_ticket   (0.3%)
```

**Testing Value**:
- **4x larger** than basic_3
- Heavy focus on buy_ticket and query_ticket
- **73 query_transfer commands** = 0.35% impact
- Good for stress testing ticket operations
- Cannot fully verify without expected output

**Testing Strategy**:
- Extract first few query_transfer test cases for M4 development
- Use for smoke testing before M6 optimization

---

### basic_5: Large Scale System Test
- **Total Commands**: 41,265
- **Test Files**: 10 (1.in - 10.in)
- **Output Files**: None (no expected output yet)
- **query_transfer**: 137 (0.33%)
- **Status**: ❌ **Will fail** without query_transfer

**Command Distribution**:
```
13,919 buy_ticket      (33.7%)
13,905 query_ticket    (33.7%)
8,024  query_profile   (19.4%)
1,709  login           (4.1%)
1,364  query_order     (3.3%)
912    modify_profile  (2.2%)
425    logout          (1.0%)
238    add_user        (0.6%)
137    query_transfer  (0.33%) ❌
134    refund_ticket   (0.3%)
```

**Testing Value**:
- **7.3x larger** than basic_3
- 10 test files (vs 5 in basic_3-4)
- **137 query_transfer commands** = 0.33% impact
- Good for stress testing state persistence
- Tests memory efficiency at scale

**Testing Strategy**:
- Use after M4 (query_transfer) completion
- Run subset of tests initially (e.g., 1-5 only)
- Full run for M6 optimization verification

---

### basic_6: Maximum Scale Stress Test
- **Total Commands**: 112,109
- **Test Files**: 10 (1.in - 10.in)
- **Output Files**: None (no expected output yet)
- **Query_transfer**: 392 (0.35%)
- **Status**: ❌ **Will fail** without query_transfer

**Command Distribution**:
```
38,468 buy_ticket      (34.3%)
38,189 query_ticket    (34.1%)
20,492 query_profile   (18.3%)
4,533  login           (4.0%)
3,752  query_order     (3.3%)
2,292  modify_profile  (2.0%)
1,442  add_user        (1.3%)
881    logout          (0.8%)
392    query_transfer  (0.35%) ❌
372    refund_ticket   (0.3%)
```

**Testing Value**:
- **19.8x larger** than basic_3 (MASSIVE)
- **Maximum stress test** for memory and performance
- **392 query_transfer commands** = 0.35% impact
- Critical for M6 optimization
- Tests 42-47 MiB memory constraint

**Testing Strategy**:
- **DO NOT RUN** until M6 optimization phase
- May take 10+ minutes to execute
- Use for final pre-OJ submission validation
- Run in GitHub Actions, not locally

---

## Query_Transfer Impact Analysis

### Overall Impact
- **basic_1**: 0 commands (0%) → ✅ No impact
- **basic_2**: 4 commands (0.24%) → ✅ Minimal impact
- **basic_3**: 12 commands (0.21%) → ✅ Minimal impact
- **basic_4**: 73 commands (0.35%) → ❌ Blocks 0.35%
- **basic_5**: 137 commands (0.33%) → ❌ Blocks 0.33%
- **basic_6**: 392 commands (0.35%) → ❌ Blocks 0.35%

### Key Insight
**query_transfer represents <0.4% of all test commands**, but **blocks 64% of our test coverage** (basic_4-6 with 174,025 commands vs basic_1-3 with 8,431 commands).

**Ratio**: 20.6x more test commands locked behind query_transfer!

---

## Testing Strategy by Milestone

### M3.2.2 (Current - Fix query_order Regression)
**Primary Test**: basic_3
**Why**: Has expected output files, currently at 88.25% pass rate
**Target**: 95%+ overall, 98.8%+ on test 1
**Commands**: All tests runnable (ignore 12 query_transfer failures)
**Execution**: Sequential run with state persistence

**Test Command**:
```bash
rm -f *.dat  # Clean state
for i in 1 2 3 4 5; do
  timeout 300 ./code < data/017/data/basic_3/$i.in > test_$i.out
  diff -u data/017/data/basic_3/$i.out test_$i.out > diff_$i.txt
done
```

**Success Metrics**:
- Test 1: 98.8%+ match (1,531/1,551 lines)
- Test 2-5: Degradation <5% vs test 1
- Overall: 95%+ average pass rate
- No timestamp collision pattern (99.8% → 99.2% → 98.5% → 90% → 61%)

---

### M4 (Next - Implement query_transfer)
**Development Tests**: basic_2 (4 cases), basic_3 (12 cases)
**Why**: Small sample size, easy to debug, has expected outputs
**Approach**: Extract individual query_transfer test cases, verify correctness

**Verification Test**: basic_4 test 1
**Why**: 73 query_transfer cases, enough to test edge cases
**Target**: 98%+ pass rate on test 1

**Test Command**:
```bash
# Development - extract query_transfer cases
grep -n "query_transfer" data/017/data/basic_2/1.in
grep -n "query_transfer" data/017/data/basic_3/in.txt

# Verification - run basic_4 test 1
rm -f *.dat
timeout 600 ./code < data/017/data/basic_4/1.in > basic_4_test_1.out
# Manual verification (no expected output file)
```

---

### M5 (Future - Optimize query_transfer)
**Primary Test**: basic_5
**Why**: 137 query_transfer cases, 10 test files
**Target**: 95%+ pass rate

**Test Command**:
```bash
rm -f *.dat
for i in 1 2 3 4 5; do  # Run first 5 tests only
  timeout 600 ./code < data/017/data/basic_5/$i.in > basic_5_test_$i.out
done
# Manual verification
```

---

### M6 (Final - Performance & Memory Optimization)
**Primary Test**: basic_6
**Why**: 112,109 commands, maximum stress test
**Target**: 98%+ pass rate, <47 MiB memory, <time limit

**Test Command** (GitHub Actions only):
```bash
rm -f *.dat
for i in 1 2 3 4 5 6 7 8 9 10; do
  timeout 1200 ./code < data/017/data/basic_6/$i.in > basic_6_test_$i.out
done
# Measure memory usage with /usr/bin/time -v
```

⚠️ **WARNING**: basic_6 may take 20+ minutes. Do NOT run locally.

---

### M7 (Pre-OJ - Final Verification)
**Full Test Suite**: All basic_1 through basic_6
**Why**: Comprehensive coverage before OJ submission
**Execution**: GitHub Actions CI pipeline

**Test Matrix**:
- basic_1: 100% match (verified in M1)
- basic_2: Manual verification (no expected output)
- basic_3: 98%+ match (5 tests)
- basic_4: 95%+ estimated (manual verification)
- basic_5: 95%+ estimated (manual verification)
- basic_6: 95%+ estimated (manual verification)

---

## Test Data Observations

### Files Available
- **basic_3**: Has both in.txt and split *.in files + expected *.out files ✅
- **basic_4-6**: Have split *.in files and combined in.txt, but **NO expected *.out files** ❌

### Implications
1. **basic_3 is our only fully verifiable test suite**
2. **basic_4-6 require manual verification** or output file generation
3. **OJ submission is the ultimate verification** for basic_4-6

### Split Output Files
- basic_3 has `split_output.py` (282 bytes) - used to generate individual *.out files
- Could potentially generate expected outputs for basic_4-6, but:
  - Requires correct implementation (chicken-and-egg problem)
  - Risk of baking in bugs
  - Better to verify against OJ system

---

## Risk Analysis

### Current Risks (M3.2.2)
- ✅ **Low Risk**: basic_3 fully testable with expected outputs
- ✅ **Low Risk**: Only 12 query_transfer failures (0.21% impact)
- ⚠️ **Medium Risk**: Current regression at 88.25% vs 95.05% baseline
- ✅ **Low Risk**: Root cause identified (missing saveOrderCounter)

### Future Risks (M4+)
- ⚠️ **High Risk**: No expected outputs for basic_4-6
- ⚠️ **High Risk**: Cannot verify query_transfer correctness before OJ
- ⚠️ **Medium Risk**: basic_6 may exceed time/memory limits
- ⚠️ **Medium Risk**: Limited feedback loop for optimization

### Mitigation Strategies
1. **Use basic_2 + basic_3 query_transfer cases** for initial development
2. **Manually verify basic_4** test 1 output for sanity check
3. **Reserve OJ submissions** for post-M5 (need high confidence)
4. **Implement robust logging** for query_transfer debugging
5. **Profile memory usage early** (before basic_6 stress test)

---

## Recommendations

### Immediate Actions (M3.2.2)
1. ✅ Focus on basic_3 for verification
2. ✅ Run tests sequentially with state persistence
3. ✅ Target 95%+ overall, 98.8%+ test 1
4. ✅ Verify no timestamp collision pattern

### Next Milestone (M4)
1. 📋 Extract all 16 query_transfer test cases from basic_2-3
2. 📋 Implement query_transfer with basic_2 cases
3. 📋 Verify against basic_3 cases (12 tests)
4. 📋 Smoke test with basic_4 test 1 (73 cases)
5. 📋 Plan for expected output generation or manual verification

### Long-term Strategy
1. 📋 **M5**: Use basic_5 for optimization testing
2. 📋 **M6**: Run basic_6 in CI, not locally
3. 📋 **M7**: Full test suite before first OJ submission
4. 📋 Consider generating expected outputs for basic_4-6 (if confident)

---

## Summary

We have **robust testing for M3.2.2** with basic_3 (5,651 commands, full expected outputs). For **M4 onward**, we face a verification gap due to missing expected outputs for basic_4-6, representing 174,025 commands (95.4% of total test coverage).

**Critical Path**:
- M3.2.2: Fix regression using basic_3 → 95%+ pass rate
- M4: Implement query_transfer using basic_2-3 → verify with basic_4 samples
- M5-M6: Optimize using basic_5-6 → manual verification + profiling
- M7: Full suite verification → OJ submission

**Test Philosophy**: Use basic_3 as the "ground truth" for correctness. Use basic_4-6 for stress testing and optimization. Reserve OJ submissions for high-confidence milestones.

---

**Prepared by**: Tessa
**Date**: 2026-02-26
**Next Review**: After M3.2.2 completion
