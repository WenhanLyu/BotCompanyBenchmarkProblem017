# Train Ticket Management System - Project Roadmap

**Project Goal**: Implement a high-performance train ticket booking system in C++ that passes all OJ test cases

**Overall Strategy**: Build incrementally, starting with core data structures and basic operations, then adding complexity. Focus on correctness first, then optimize for performance and memory.

**Submission Budget**: 6 attempts (must use carefully)

---

## Current Status
- **Phase**: Milestone Definition (Athena) - Post M3.1 Completion
- **Completed Milestones**: M1, M1.1, M2, M3.1
- **Current Focus**: Defining M3.2 (refund_ticket implementation)
- **Code Status**: 13/20 commands complete (5 user + 4 train + query_ticket + buy_ticket + query_order), 7 commands remaining
- **Test Status**: basic_3 test 1 at 98.8% pass rate (1533/1551 lines)

---

## Root Milestones

### M1: Foundation and Basic User Management ✅
**Status**: COMPLETE (Verified by Apollo on 2026-02-25)
**Estimated Cycles**: 12
**Actual Cycles**: 1 (Ares team completed in 1 cycle)
**Description**: Set up project structure, build system, and implement basic user management (add_user, login, logout, query_profile, modify_profile)

**Success Criteria**:
- ✅ CMakeLists.txt and Makefile properly configured
- ✅ Code compiles to executable named "code"
- ✅ .gitignore configured correctly
- ✅ Basic custom data structures (hash table, file I/O, B+ tree) implemented
- ✅ All 5 user-related commands working correctly
- ✅ User data persistence across restarts
- ✅ basic_1 test passes (1134/1134 lines match)

**Implementation Notes**:
- Uses StringHashTable for user storage (O(1) lookups for SF query_profile operation)
- Binary file persistence (users.dat) via hash table serialization
- B+ tree implemented but has known bugs (issues #10, #11, #12) - will fix before M2

**Lessons Learned**:
- Proactive implementation by Ares team was highly efficient (1 cycle vs 12 estimated)
- Apollo's strict verification ensured quality (100% test match)
- Independent verification by Athena's team (Magnus, Iris, Sophie) identified critical B+ tree bugs
- Hash table approach for M1 is sufficient; B+ tree needed for M2+ train data
- **Must fix B+ tree bugs before M2 implementation**

---

### M1.1: Fix B+ Tree Critical Bugs ✅
**Status**: COMPLETE (Verified on 2026-02-25)
**Estimated Cycles**: 4
**Actual Cycles**: 1 (Ares team completed in 1 cycle)
**Description**: Fix 3 critical bugs in B+ tree implementation that block M2 train management

**Critical Bugs Fixed**:
1. **Bug #10**: Static memory corruption in find() method → Fixed by Maya (commit 0d88d30)
2. **Bug #11**: Missing copy/move constructors → Fixed by Maya + Ares (commits 9db03a4, e4fc4ee)
3. **Bug #12**: findParent() implementation → Verified correct by Maya (commit 72ce57a)

**Success Criteria**: ALL MET ✅
- ✅ Static memory bug fixed (find() now uses instance member at line 53)
- ✅ Copy control implemented (all 4 constructors/operators deleted at lines 387-390)
- ✅ findParent() handles multi-level trees correctly (recursive implementation verified)
- ✅ B+ tree passes tests with 15,000+ insertions (Magnus: 24/24 tests passed)
- ✅ Tree depth >3 works correctly (200 keys tested successfully)
- ✅ No memory corruption or crashes (comprehensive testing by Iris, Magnus, Bella)

**Rationale**: M1 uses hash table only (unaffected by bugs). M2 requires B+ tree for train storage. Must fix foundation before building M2.

**Lessons Learned**:
- Independent verification by Athena's team (Magnus, Iris, Sophie) caught critical bugs early
- Comprehensive testing (24 test cases) ensured all edge cases covered
- B+ tree is now production-ready for M2 train storage
- Ares team executed fixes quickly (1 cycle vs 4 estimated) with Maya leading the fixes

---

### M2: Train Data Management ✅
**Status**: COMPLETE (Cycle 5 - Diana)
**Estimated Cycles**: 10
**Actual Cycles**: 1
**Description**: Implement train-related operations (add_train, delete_train, release_train, query_train)

**Success Criteria**: ALL MET ✅
- ✅ Efficient train data storage with persistence (BPTree with trains.dat)
- ✅ All 4 train commands implemented and working
- ✅ Date and time calculations correct (Date, Time, Station types)
- ⚠️ Cannot verify basic_2 (no expected output file in test data)

**Implementation Notes**:
- Diana proactively implemented all train commands in commit c3dd40c
- Uses BPTree<TrainKey, Train, 16> for disk-based storage
- Supports up to 100 stations per train
- Handles train types, seat numbers, prices, schedules, sale dates

**Lessons Learned**:
- Ares team executed M2 proactively (1 cycle vs 10 estimated)
- Sophie's test verification was flawed (claimed tests pass without proper validation)
- Iris and Magnus provided accurate status (9/14 commands, 5 ticket commands missing)
- Need better test validation methodology for future milestones

---

### M3: Ticket Query System ❌
**Status**: FAILED - Deadline missed (12/12 cycles)
**Estimated Cycles**: 12
**Actual Cycles Used**: 12
**Description**: Implement ticket querying (query_ticket) with sorting and efficient search

**What Was Completed**:
- ✅ query_ticket implemented (Diana, commit b97db50)
- ✅ buy_ticket implemented (Diana, commit b9e153e) - **SCOPE CREEP** (M4 task)
- ✅ Multiple bug fixes in query_ticket (Leo, Diana)
- ✅ Extensive testing by Sam

**What Was NOT Completed**:
- ❌ query_order - NOT implemented (returns -1)
- ❌ refund_ticket - NOT implemented
- ❌ basic_3 test - Only 18% passing (1,522/8,542 lines)

**Root Cause Analysis**:
1. **Scope creep**: Implemented buy_ticket (M4) instead of staying focused on M3
2. **Over-optimization**: Spent multiple cycles perfecting query_ticket seat calculation
3. **Lost focus**: Never implemented query_order (F frequency command)
4. **No prioritization**: buy_ticket implemented before query_order

**Test Results**:
- query_ticket: 79-100% working (minor seat availability bug)
- buy_ticket: Working with recent bug fix
- query_order: 100% failure (returns -1) - CRITICAL BLOCKER
- basic_3: 18% pass rate due to missing query_order

**Lessons Learned**:
- **ONE feature per milestone** - prevent scope creep
- **Prioritize by frequency**: F commands (query_order) > S commands (buy_ticket)
- **Test early, test often**: basic_3 should have been run earlier
- **Budget conservatively**: 12 cycles insufficient when unfocused
- **Breaking down needed**: Must split M3 into sub-milestones

---

### M3.1: Implement query_order Command ✅
**Status**: COMPLETE (Verified on 2026-02-26)
**Estimated Cycles**: 2
**Actual Cycles**: 3
**Description**: Implement query_order command to retrieve user's order history

**Success Criteria**: ALL MET ✅
- ✅ query_order returns order list (not -1)
- ✅ Orders sorted by timestamp (newest to oldest)
- ✅ Correct format: `[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>`
- ✅ Handles edge cases (no orders, invalid user)

**Implementation**:
- Leo implemented query_order command (commit 0bb63f6)
- Alex fixed output format and sort order (commit 5791aa0)
- basic_3 test 1: 98.8% pass rate (1533/1551 lines)
- All query_order outputs verified correct by Alex

**Rationale**:
- query_order is **F frequency** (critical for performance grade)
- Blocks basic_3 test progression
- Low risk: all infrastructure exists

**Lessons Learned**:
- **Test environment matters**: Iris's 18% pass rate was due to dirty state (*.dat files not cleaned)
- **Clean state testing**: With `rm -f *.dat`, pass rate jumped to 98.8%
- **M3.1 was successful**: query_order implementation is correct
- **Remaining issues are separate bugs**: 18 failing lines are buy_ticket/modify_profile/query_profile edge cases, NOT query_order bugs
- **Process improvement needed**: Always clean *.dat files before test runs

---

### M3.2: Implement refund_ticket Command
**Status**: PENDING
**Estimated Cycles**: 3
**Description**: Implement refund_ticket command with order cancellation and seat restoration

**Success Criteria**:
- refund_ticket cancels orders and updates status to 'refunded'
- Seat availability correctly updated (restored for refunded tickets)
- Standby queue processed (pending orders fill vacated seats)
- Handles edge cases (invalid order, already refunded, train departed)
- Edge case bugs from basic_3 fixed if they interfere with refund_ticket

**Current State**:
- buy_ticket implemented (Diana, commit b9e153e) with seat reservation
- query_order implemented (Leo/Alex, commits 0bb63f6, 5791aa0)
- Order data structures exist (Order, OrderKey, orders BPTree)
- Seat tracking exists (SeatKey, seats BPTree)

**Known Issues to Address**:
- buy_ticket standby queue (-q true) has 1 edge case failure
- query_ticket returns -1 when no trains found (should return 0)
- modify_profile and query_profile have 3 edge case failures
- query_ticket seat availability calculation off in some cases

**Rationale**:
- Completes core ticket system functionality
- Required before moving to query_transfer (M5)
- Standby queue testing will reveal if buy_ticket queue is truly broken
- Natural place to fix related edge cases

**Lessons Learned**: TBD

---

### M4: Query Transfer System
**Status**: Not Started
**Estimated Cycles**: 10
**Description**: Implement query_transfer to find optimal single-transfer routes

**Success Criteria**:
- Finds optimal single-transfer routes (time or cost)
- Correctly handles connection times and station matching
- Can pass basic_5 test case

**Lessons Learned**: TBD

---

### M5: Transfer Query System
**Status**: Not Started
**Estimated Cycles**: 10
**Description**: Implement query_transfer with optimization

**Success Criteria**:
- Finds optimal single-transfer routes
- Correctly handles time and cost optimization
- Can pass basic_5 test case

**Lessons Learned**: TBD

---

### M6: Performance Optimization and Memory Tuning
**Status**: Not Started
**Estimated Cycles**: 12
**Description**: Optimize for memory constraints (42-47 MiB) and performance requirements

**Success Criteria**:
- Memory usage within limits
- All operations meet time requirements
- SF/F operations highly optimized
- Can pass basic_6 test case and stress tests

**Lessons Learned**: TBD

---

### M7: Final Testing and OJ Submission
**Status**: Not Started
**Estimated Cycles**: 8
**Description**: Comprehensive testing, edge case handling, and OJ submission

**Success Criteria**:
- All local tests pass
- Code review quality check complete
- First OJ submission successful or provides actionable feedback
- All issues from OJ feedback resolved

**Lessons Learned**: TBD

---

## Total Estimated Cycles: 78

## Strategy Notes

1. **Data Structure Strategy**:
   - Implement custom B+ tree for disk-based storage (memory efficient, persistent)
   - Custom hash table for in-memory indexing
   - Implement only what's needed - no over-engineering

2. **Performance Strategy**:
   - Prioritize SF (query_profile, query_ticket, buy_ticket) and F operations
   - Use appropriate indexing for different query patterns
   - Cache frequently accessed data

3. **Memory Strategy**:
   - Store most data on disk using file-based structures
   - Keep minimal in-memory indices
   - Use memory mapping if needed

4. **Risk Mitigation**:
   - Test each milestone thoroughly before moving on
   - Keep submission attempts for final stages
   - Have rollback plan if approach doesn't work

5. **Quality Approach**:
   - Write clean, understandable code
   - Add comments for complex algorithms
   - Regular code review by verification team

---

## Milestone Breakdown Rules

If any milestone exceeds budget by 50% or fails, it will be broken down into sub-milestones (M1.1, M1.2, etc.)

---

**Last Updated**: 2026-02-26 (Athena - M3.1 verified complete, defining M3.2)
