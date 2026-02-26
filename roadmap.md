# Train Ticket Management System - Project Roadmap

**Project Goal**: Implement a high-performance train ticket booking system in C++ that passes all OJ test cases

**Overall Strategy**: Build incrementally, starting with core data structures and basic operations, then adding complexity. Focus on correctness first, then optimize for performance and memory.

**Submission Budget**: 6 attempts (must use carefully)

---

## Current Status
- **Phase**: Milestone Definition (Athena) - Post M3 Deadline Miss
- **Completed Milestones**: M1 (User Management), M1.1 (B+ Tree Bug Fixes), M2 (Train Management)
- **Current Focus**: Defining M3.1 (query_order implementation)
- **Code Status**: 11/20 commands complete (5 user + 4 train + query_ticket + buy_ticket), 9 commands remaining
- **M3 Status**: FAILED - Deadline missed (12/12 cycles used)

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

### M3.1: Implement query_order Command
**Status**: PENDING
**Estimated Cycles**: 2
**Description**: Implement query_order command to retrieve user's order history

**Success Criteria**:
- query_order returns order list (not -1)
- Orders sorted by timestamp
- Correct format: [TIMESTAMP] [TRAINID] [FROM] [TO] [STATUS] [PRICE] [COUNT]
- Handles edge cases (no orders, invalid user)
- Infrastructure verified by Magnus: Order struct, OrderKey, queryUserOrders() all exist

**Rationale**:
- query_order is **F frequency** (critical for performance grade)
- Blocks basic_3 test progression (140+ commands return -1)
- Low risk: all infrastructure exists, ~1.5 hours implementation
- Focused scope: ONLY query_order, no other features

**Lessons Learned**: TBD

---

### M4: Ticket Purchasing and Order Management
**Status**: Not Started
**Estimated Cycles**: 14
**Description**: Implement buy_ticket, query_order, refund_ticket with standby queue

**Success Criteria**:
- Ticket purchase with seat availability checking
- Standby queue (pending orders) working correctly
- Refund properly updates available seats and processes standby queue
- Order history maintained correctly
- Can pass basic_4 test case

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

**Last Updated**: 2026-02-26 (Cycle 5 - Athena marking M2 complete, defining M3)
