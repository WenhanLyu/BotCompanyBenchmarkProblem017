# Iris - Cycle Notes

## Most Recent Task (2026-02-25)
**Assignment:** Task #13 - Independently analyze requirements and current implementation

## What I Did This Cycle
1. Read complete specification (README.md, spec.md, DATA_STRUCTURES.md, roadmap.md)
2. Analyzed current implementation (main.cpp, types.hpp, data structures)
3. Identified all 20 commands and their requirements
4. Analyzed critical constraints (memory, time, performance)
5. Evaluated edge cases and corner cases
6. Assessed implementation status (35% complete)
7. Identified critical risks and dependencies
8. Created comprehensive 850-line requirements analysis document

## Key Findings

### Critical Discovery: Memory Constraint Violation Risk 🔴
**Current approach will NOT scale:**
- Projected seat data: 10K trains × 36 KB = **360 MB** (exceeds 42-47 MiB limit)
- Order data: 100K orders × 200 bytes = **20 MB** (close to limit)
- **Solution Required**: Disk-based B+ tree (already implemented but has bugs)

### Implementation Status: 35% Complete
**Working (M1):**
- ✅ 7/20 commands (all user management)
- ✅ Hash table working perfectly
- ✅ Persistence working
- ✅ basic_1 test passing (100%)

**Not Implemented:**
- ❌ 4 train management commands
- ❌ 2 ticket query commands (including 2 SF operations)
- ❌ 3 order management commands (including 1 SF operation)

### Critical Requirements Identified

**Command Frequency (Performance Priority):**
- **SF (Super Frequent)**: query_profile ✅, query_ticket ❌, buy_ticket ❌
- **F (Frequent)**: login ✅, logout ✅, modify_profile ✅, query_order ❌
- **N (Normal)**: 7 commands (trains, transfer, refund) - all ❌
- **R (Rare)**: clean ✅, exit ✅

**Constraints:**
- Memory: 42-47 MiB (CRITICAL)
- Time: 5-40 seconds per test
- Files: Maximum 20
- STL: Only std::string allowed
- Persistence: Multi-run tests

### Edge Cases Identified

**Train Management (Not Yet Tested):**
1. Two-station trains (stopoverTimes = "_")
2. Multi-day journeys (up to 3 days)
3. Month boundary crossings (06-30 → 07-02)
4. Midnight crossings (23:59 + 2 min = 00:01 next day)
5. 100-station trains (maximum complexity)
6. Release state management (before: can delete, after: cannot)

**Ticket Operations (Not Yet Implemented):**
1. Station pair matching (order matters: A→B→C, query C→A = no match)
2. Date calculations (boarding date, not start station date)
3. Seat calculations (minimum across all segments)
4. Sorting with tie-breaking (time/cost primary, trainID secondary)
5. Zero seats available (still show train)

**Order Management (Not Yet Implemented):**
1. Standby queue (FIFO, all-or-nothing)
2. Buy 0 tickets (must fail)
3. Buy > train capacity (must fail, even with standby)
4. Refund triggering cascade (multiple standby orders satisfied)
5. Partial availability (if ANY segment lacks seats, entire order fails)

### Critical Risks

1. **B+ Tree Bugs** 🔴 HIGH
   - Known issues: #10, #11, #12
   - **Blocker for M2**
   - Must fix before train management

2. **Memory Limit** 🔴 HIGH
   - Seat data alone exceeds limit with naive approach
   - Must use disk-based storage
   - Requires working B+ tree

3. **Query Performance** 🟡 MEDIUM
   - query_ticket: O(n×m) naive = 1M operations per query
   - At 1M queries: 1 trillion comparisons
   - Must build station pair index

4. **Date/Time Calculations** 🟡 MEDIUM
   - Multi-day, cross-month, midnight crossings
   - Off-by-one errors common
   - Needs extensive testing

5. **Standby Queue Complexity** 🟡 MEDIUM
   - Cascade refund processing
   - FIFO ordering critical
   - Integration testing required

### Dependencies

**Critical Path:**
```
B+ Tree Fix → Train Management → Ticket Query → Order Management
```

**Implementation Order:**
1. **M1.5**: Fix B+ tree bugs (BLOCKER) - 3-4 cycles
2. **M2**: Train management (4 commands) - 10 cycles
3. **M3**: Ticket queries (2 commands, SF priority) - 12 cycles
4. **M4**: Order management (3 commands, SF priority) - 14 cycles
5. **M5**: Transfer query (1 command) - 10 cycles
6. **M6**: Optimization and stress testing - 12 cycles
7. **M7**: Final testing and OJ submission - 8 cycles

**Total Estimated**: 78 cycles

## Files Created This Cycle
- `workspace/iris/requirements_analysis.md` - Comprehensive 850-line analysis

## Recommendations

### Immediate Actions (Next Cycle)
1. **CRITICAL**: Fix B+ tree bugs #10, #11, #12
   - Review bug reports
   - Write comprehensive test suite
   - Validate memory usage
   - Estimated: 3-4 cycles

2. **After B+ Tree Fix**: Begin M2 train management
   - Implement Train struct
   - Parse pipe-separated values
   - Implement date/time calculations
   - Implement 4 train commands

### Success Criteria for Full System
- [ ] All 20 commands implemented
- [ ] All 6 basic tests passing
- [ ] Memory usage < 42 MiB
- [ ] Performance within time limits
- [ ] Code quality ready for review

## Analysis Summary

**System Complexity**: HIGH
- 20 commands across 4 functional areas
- Complex data structures (B+ tree, hash table, indices)
- Strict memory and performance constraints
- Multi-day date/time calculations
- Standby queue with cascade processing

**Current Quality**: EXCELLENT (M1 only)
- User management: 100% correct
- No bugs found in implemented features
- Persistence working perfectly
- Edge cases handled properly

**Next Steps**: Fix B+ tree, then proceed with M2

**Overall Assessment**: Project is on track but B+ tree fix is critical blocker for progress.

---

## Previous Cycles

### Cycle 2 (2026-02-25) - basic_1 Evaluation
- Ran complete basic_1 test suite (1,135 commands)
- Verified byte-for-byte correctness (MD5 match)
- Created and ran edge case tests (28 commands)
- Created and ran boundary tests (11 commands)
- Tested persistence across restarts
- Result: 100% PASS, no bugs found

### Cycle 1 (Prior) - Code Review
- Reviewed user management implementation
- Found excellent code quality
- Verified correctness

---

**Status**: Task #13 complete. Requirements analysis documented. Ready for next assignment.
