# Train Ticket Management System - Project Roadmap

**Project Goal**: Implement a high-performance train ticket booking system in C++ that passes all OJ test cases

**Overall Strategy**: Build incrementally, starting with core data structures and basic operations, then adding complexity. Focus on correctness first, then optimize for performance and memory.

**Submission Budget**: 6 attempts (must use carefully)

---

## Current Status
- **Phase**: Milestone Definition (Athena)
- **Completed Milestones**: None (M1 implementation complete, pending formal milestone)
- **Current Focus**: Formalizing M1 milestone for user management system
- **Code Status**: Build system + user management working, basic_1 test passes

---

## Root Milestones

### M1: Foundation and Basic User Management ⏳
**Status**: Implementation Complete (Pending Formal Milestone Verification)
**Estimated Cycles**: 12
**Actual Cycles**: ~6 (Ares team proactive work)
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
- Proactive implementation by Ares team saved cycles
- Apollo's strict verification caught B+ tree shortcuts early
- Independent verification (Athena's team) found subtle bugs Apollo missed
- Hash table approach for M1 is sufficient; B+ tree needed for M2+ train data

---

### M2: Train Data Management
**Status**: Not Started
**Estimated Cycles**: 10
**Description**: Implement train-related operations (add_train, delete_train, release_train, query_train)

**Success Criteria**:
- Efficient train data storage with persistence
- All train commands working correctly
- Date and time calculations correct
- Can pass basic_2 test case (train management focused)

**Lessons Learned**: TBD

---

### M3: Ticket Query System
**Status**: Not Started
**Estimated Cycles**: 12
**Description**: Implement ticket querying (query_ticket) with sorting and efficient search

**Success Criteria**:
- Fast station-to-station ticket search
- Correct sorting by time and cost
- Handles date calculations correctly
- Can pass basic_3 test case

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

**Last Updated**: 2026-02-25 (Cycle 0 - Athena Planning)
