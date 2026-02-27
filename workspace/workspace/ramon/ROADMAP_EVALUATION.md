# Roadmap Evaluation - Milestone Sequence Analysis

**Evaluator**: Ramon
**Date**: 2026-02-26
**Current Status**: M3.2.2 in progress, 15/16 commands complete

---

## Executive Summary

The roadmap has **critical issues** that need immediate correction:

1. **DUPLICATE MILESTONES**: M4 and M5 are both about `query_transfer` (redundant)
2. **INCORRECT COMMAND COUNT**: Claims "14/20 commands" but actually "15/16 commands"
3. **MISLEADING NAMING**: M3 "Ticket Query System" included multiple non-query commands
4. **INEFFICIENT SEQUENCING**: M3.2.1 → M3.2.2 created a two-milestone fix cycle for one bug

**Recommendation**: Consolidate remaining milestones into 3 clear phases: Fix (M3.2.2) → Implement (M4) → Optimize (M5) → Submit (M6).

---

## 1. Command Implementation Status

### Actual Status: 15/16 Commands Complete

**Implemented Commands (verified in main.cpp):**
1. ✅ add_user (N frequency)
2. ✅ login (F frequency)
3. ✅ logout (F frequency)
4. ✅ query_profile (SF frequency)
5. ✅ modify_profile (F frequency)
6. ✅ add_train (N frequency)
7. ✅ delete_train (N frequency)
8. ✅ release_train (N frequency)
9. ✅ query_train (N frequency)
10. ✅ query_ticket (SF frequency)
11. ✅ buy_ticket (SF frequency)
12. ✅ query_order (F frequency)
13. ✅ refund_ticket (N frequency)
14. ✅ clean (R frequency)
15. ✅ exit (R frequency)

**Missing Commands:**
16. ❌ query_transfer (N frequency) - **ONLY REMAINING COMMAND**

### Issue: Roadmap Says "14/20 Commands"

**Line 15 of roadmap.md states:**
> Code Status: 14/20 commands complete (5 user + 4 train + query_ticket + buy_ticket + query_order + refund_ticket), 6 commands remaining

**Reality:**
- Total commands in README: **16** (not 20)
- Commands complete: **15** (not 14)
- Commands remaining: **1** (not 6) - only query_transfer

**Root Cause**: Counting error. Someone miscounted total commands or didn't include clean/exit.

**Impact**: Misleading progress tracking. Team thinks more work remains than actually does.

---

## 2. Duplicate Milestone Analysis

### Critical Duplication: M4 and M5

**M4: Query Transfer System** (line 292-302)
- Status: Not Started
- Estimated Cycles: 10
- Description: "Implement query_transfer to find optimal single-transfer routes"
- Success Criteria: "Finds optimal single-transfer routes (time or cost)"

**M5: Transfer Query System** (line 306-317)
- Status: Not Started
- Estimated Cycles: 10
- Description: "Implement query_transfer with optimization"
- Success Criteria: "Finds optimal single-transfer routes"

### Analysis

These are **THE SAME MILESTONE**. Both aim to implement `query_transfer`.

The distinction between "implement" (M4) and "implement with optimization" (M5) is artificial:
1. Any implementation MUST handle optimization (time vs cost via `-p` parameter)
2. The README spec requires both time and cost optimization in ONE command
3. Cannot implement query_transfer without optimization logic

**This is not "implement then optimize" - it's "implement once, correctly".**

### Root Cause

Likely copied from a template that separated "basic implementation" from "optimization", but query_transfer doesn't allow this separation - the `-p` parameter IS the core feature.

### Recommendation

**MERGE M4 and M5 into a single milestone: M4: Implement query_transfer**
- Budget: 12 cycles (reasonable for complex pathfinding)
- Success criteria: All query_transfer requirements from README

---

## 3. Milestone Naming and Scope Issues

### M3: "Ticket Query System" (line 106-141)

**Original Scope (implied by name):**
- Implement ticket querying (query_ticket)

**Actual Scope (what was attempted):**
- ✅ query_ticket implemented
- ✅ buy_ticket implemented ← **SCOPE CREEP** (acknowledged in roadmap line 114)
- ❌ query_order NOT implemented ← **WRONG PRIORITY**

**Problem:**
- Milestone name says "Query System" but included purchase system (buy_ticket)
- Implemented M4-level command (buy_ticket) before finishing query_order
- query_order is **F frequency** but was skipped for buy_ticket (**SF frequency**)

### M3.1, M3.2, M3.2.1, M3.2.2 - Over-Fragmentation

After M3 failed, it was broken into:
- **M3.1**: Implement query_order (2 cycles estimated, 3 actual) ✅
- **M3.2**: Implement refund_ticket (3 cycles estimated, 3 actual) ✅
- **M3.2.1**: Fix timestamp bug (2 cycles estimated, 2 actual) ❌ FAILED
- **M3.2.2**: Complete the fix (2 cycles estimated, in progress)

**Issue**: M3.2.1 and M3.2.2 are fixing **THE SAME BUG**
- M3.2.1 added initOrderSystem() but forgot saveOrderCounter()
- M3.2.2 adds saveOrderCounter() to complete the fix
- This is **one bug requiring one fix**, artificially split into two milestones

**Root Cause**: M3.2.1 was incomplete (half-fix), required continuation

**Better Approach**: M3.2.1 should have been scoped correctly to include BOTH load and save operations from the start. Current M3.2.2 is damage control.

---

## 4. Milestone Sequencing Logic

### Current Sequence Problems

**What Happened:**
1. M1: User management ✅ (GOOD)
2. M1.1: Fix B+ tree ✅ (GOOD - proactive bug fix)
3. M2: Train management ✅ (GOOD)
4. M3: Mixed query/purchase ❌ (SCOPE CREEP)
5. M3.1: query_order ✅ (REACTIVE - should have been in M3)
6. M3.2: refund_ticket ✅ (OUT OF ORDER)
7. M3.2.1: Fix bug ❌ (INCOMPLETE)
8. M3.2.2: Fix bug again (CONTINUATION)

**Optimal Sequence Should Have Been:**

By command frequency (SF > F > N):
1. M1: Foundation + User Management (5 user commands) ✅
2. M2: Train Management (4 train commands) ✅
3. **M3: query_ticket (SF)** ✅
4. **M4: buy_ticket (SF)** ✅ (done early, but as scope creep)
5. **M5: query_order (F)** ✅ (done as M3.1)
6. **M6: refund_ticket (N)** ✅ (done as M3.2)
7. **M7: query_transfer (N)** ❌ NOT DONE
8. **M8: Optimization**
9. **M9: Submission**

### Why Current Sequence Caused Problems

1. **Mixed priorities**: M3 tried to do multiple frequency levels
2. **Incomplete planning**: Didn't identify all commands before starting M3
3. **Reactive breakdown**: M3 → M3.1 → M3.2 → M3.2.1 → M3.2.2 shows poor upfront planning
4. **Quality issues**: Bug fixes (M3.2.1, M3.2.2) shouldn't be separate milestones

---

## 5. Optimal Path to Completion

### Current Position

- **Completed**: 15/16 commands, basic functionality working at ~88% pass rate (M3.2.2 not complete)
- **In Progress**: M3.2.2 (fixing timestamp persistence bug)
- **Remaining**: query_transfer + optimization + testing

### Recommended Milestone Structure

**CONSOLIDATE into 4 clean milestones:**

#### M3.2.2: Complete Timestamp Fix (Current, ~0.5 cycles remaining)
- **Status**: IN PROGRESS
- **Scope**: Add saveOrderCounter() calls in buy_ticket (3 locations)
- **Success**: 95%+ pass rate on basic_3, degradation pattern eliminated
- **Budget**: Already allocated (2 cycles)

#### M4: Implement query_transfer (12 cycles)
- **Status**: NOT STARTED
- **Scope**: Implement query_transfer command (N frequency)
- **Features**:
  - Find single-transfer routes between stations
  - Support time optimization (`-p time`)
  - Support cost optimization (`-p cost`)
  - Handle connection timing constraints
  - Tie-breaking: minimize train 1 riding time
- **Success Criteria**:
  - query_transfer returns valid transfer routes
  - Handles "no transfer found" correctly (return `0`)
  - Optimization logic works for both time and cost
  - Can pass basic_5 test case (if available)
- **Risk**: Complex pathfinding, requires station indexing and efficient search

#### M5: Performance Optimization and Memory Tuning (10 cycles)
- **Status**: NOT STARTED
- **Scope**: Optimize for OJ constraints
- **Focus Areas**:
  - Memory: Stay within 42-47 MiB limit
  - Performance: SF/F commands highly optimized
  - Data structures: Review B+ tree block sizes, hash table sizing
  - Disk I/O: Minimize file operations
- **Success Criteria**:
  - Memory usage verified under 47 MiB
  - All SF commands (query_profile, query_ticket, buy_ticket) meet performance targets
  - basic_6 stress test passes (if available)
  - No timeouts on large test cases

#### M6: Final Testing and OJ Submission (8 cycles)
- **Status**: NOT STARTED
- **Scope**: Comprehensive testing and submission
- **Activities**:
  - Run all local test cases (basic_1 through basic_6)
  - Edge case testing (empty queries, boundary dates, max stations)
  - Code review (readability, comments for complex algorithms)
  - First OJ submission
  - Fix issues from OJ feedback (reserve 2-3 submissions for fixes)
- **Success Criteria**:
  - All local tests pass at 98%+ accuracy
  - Code review complete (no obvious bugs or style issues)
  - First OJ submission returns actionable feedback or passes
  - Final OJ submission passes all test cases

### Cycle Budget Analysis

**Remaining Work:**
- M3.2.2: ~0.5 cycles (almost done)
- M4: 12 cycles
- M5: 10 cycles
- M6: 8 cycles
- **Total: ~30.5 cycles**

**Cycles Used So Far:**
- M1: 1 cycle
- M1.1: 1 cycle
- M2: 1 cycle
- M3: 12 cycles
- M3.1: 3 cycles
- M3.2: 3 cycles
- M3.2.1: 2 cycles
- M3.2.2: 2 cycles (in progress)
- **Total: ~25 cycles**

**Total Project Budget: ~55-60 cycles** (much less than original 78 estimate due to team efficiency)

### Risk Analysis

**Low Risk:**
- M3.2.2 completion (fix is well-understood, 3 lines of code)
- M5 optimization (infrastructure already efficient)
- M6 testing (process is clear)

**Medium Risk:**
- M4 query_transfer implementation (complex algorithm, new functionality)
  - Mitigation: Break into sub-tasks (indexing, search, optimization)
  - Reserve 2 extra cycles for debugging

**High Risk:**
- OJ submission failures due to unknown edge cases
  - Mitigation: Thorough local testing, conservative estimation (8 cycles for M6)
  - Use submission budget carefully (6 attempts total)

---

## 6. Specific Recommendations

### Immediate Actions

1. **Update roadmap.md command count**: Change "14/20" to "15/16" (line 15)
2. **Merge M4 and M5**: Combine duplicate query_transfer milestones
3. **Rename merged milestone**: "M4: Implement query_transfer Command"
4. **Update cycle estimates**: Adjust total from 78 to ~55-60 cycles

### Process Improvements

1. **No more sub-sub-milestones**: M3.2.1 → M3.2.2 pattern is fragmentation
   - If a fix fails, debug and retry within the same milestone
   - Don't create new milestone numbers for bug fixes

2. **Clearer success criteria**: Each milestone needs measurable goals
   - GOOD: "95%+ pass rate on basic_3"
   - BAD: "Implement with optimization" (vague)

3. **Upfront command inventory**: Before starting a milestone, list all commands it covers
   - M3 should have explicitly listed: query_ticket, query_order (NOT buy_ticket)

4. **Independent verification**: Continue the pattern from M1.1
   - M3.2.1 claimed 98.13% but verification showed 88.25% (huge gap)
   - Multiple verifiers caught the discrepancy

### Long-Term Improvements

1. **Frequency-based prioritization**: Strictly follow SF > F > N ordering
2. **One feature per milestone**: Prevent scope creep (M3 lesson)
3. **Test-driven milestones**: Define test pass criteria before implementation
4. **Budget 20% contingency**: Complex features (query_transfer) may exceed estimates

---

## 7. Conclusion

### Roadmap Quality: GOOD with Critical Corrections Needed

**Strengths:**
- ✅ Detailed lessons learned from failures (M3, M3.2.1)
- ✅ Clear success criteria for most milestones
- ✅ Adaptive breakdown when M3 failed
- ✅ Strong documentation of root causes

**Critical Issues:**
- ❌ Duplicate milestones (M4/M5)
- ❌ Incorrect command count (14/20 vs 15/16)
- ❌ Over-fragmentation (M3.2.1 → M3.2.2)

**Recommended Fixes:**
1. Merge M4 and M5 into single query_transfer milestone
2. Update command count to 15/16
3. Consolidate remaining work into 4 milestones (M3.2.2, M4, M5, M6)
4. Total project budget: ~55-60 cycles (not 78)

### Path to Completion: CLEAR

With corrections:
- **Next milestone**: Complete M3.2.2 (0.5 cycles) → 95%+ pass rate
- **Core implementation**: M4 query_transfer (12 cycles) → 16/16 commands complete
- **Optimization**: M5 performance tuning (10 cycles) → OJ-ready
- **Submission**: M6 testing and OJ (8 cycles) → Project complete

**Estimated completion**: 30-35 cycles from now, ~55-60 cycles total.

---

**End of Evaluation**
