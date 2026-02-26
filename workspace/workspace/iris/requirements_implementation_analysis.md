# Requirements vs Implementation Analysis
**Analyst**: Iris (Requirements Analyst)
**Date**: 2026-02-26
**Project**: Train Ticket Management System (ACMOJ #1170)

---

## Executive Summary

**Current State**: 9 of 14 commands implemented (64% complete)
**Milestone Progress**: M1 and M1.1 complete, M2 in progress (train commands done, but not all tested)
**Next Priority**: Complete M2 verification, then tackle M3 (Ticket Query System)

---

## Complete Command Inventory (All 14 Commands)

### ✅ User Management Commands (5/5 Implemented)

| # | Command | Frequency | Status | Implementation Location |
|---|---------|-----------|--------|------------------------|
| 1 | `add_user` | N (Normal) | ✅ IMPLEMENTED | main.cpp:185-251 |
| 2 | `login` | F (Frequent) | ✅ IMPLEMENTED | main.cpp:253-282 |
| 3 | `logout` | F (Frequent) | ✅ IMPLEMENTED | main.cpp:284-301 |
| 4 | `query_profile` | SF (Super Frequent) | ✅ IMPLEMENTED | main.cpp:303-341 |
| 5 | `modify_profile` | F (Frequent) | ✅ IMPLEMENTED | main.cpp:343-422 |

**Notes**: All user commands fully implemented with persistence. Uses StringHashTable for O(1) lookups. basic_1 test passes (1134/1134 lines).

---

### ✅ Train Management Commands (4/4 Implemented)

| # | Command | Frequency | Status | Implementation Location |
|---|---------|-----------|--------|------------------------|
| 6 | `add_train` | N (Normal) | ✅ IMPLEMENTED | main.cpp:440-520 |
| 7 | `delete_train` | N (Normal) | ✅ IMPLEMENTED | main.cpp:522-550 |
| 8 | `release_train` | N (Normal) | ✅ IMPLEMENTED | main.cpp:552-582 |
| 9 | `query_train` | N (Normal) | ✅ IMPLEMENTED | main.cpp:584-669 |

**Notes**: Train commands implemented using B+ tree storage. B+ tree bugs fixed in M1.1. **Verification needed** - no evidence of passing basic_2 or basic_3 tests yet.

---

### ❌ Ticket Management Commands (0/5 Implemented)

| # | Command | Frequency | Status | Implementation Location |
|---|---------|-----------|--------|------------------------|
| 10 | `query_ticket` | SF (Super Frequent) | ❌ NOT IMPLEMENTED | N/A |
| 11 | `query_transfer` | N (Normal) | ❌ NOT IMPLEMENTED | N/A |
| 12 | `buy_ticket` | SF (Super Frequent) | ❌ NOT IMPLEMENTED | N/A |
| 13 | `query_order` | F (Frequent) | ❌ NOT IMPLEMENTED | N/A |
| 14 | `refund_ticket` | N (Normal) | ❌ NOT IMPLEMENTED | N/A |

**Notes**: None of the ticket commands are implemented. This is expected per roadmap (M3-M5). **Critical**: `query_ticket` and `buy_ticket` are SF (Super Frequent) - must be highly optimized.

---

### ✅ System Commands (2/2 Implemented)

| Command | Frequency | Status | Implementation Location |
|---------|-----------|--------|------------------------|
| `clean` | R (Rare) | ✅ IMPLEMENTED | main.cpp:424-436 |
| `exit` | R (Rare) | ✅ IMPLEMENTED | main.cpp:682-684 |

**Notes**: Both system commands implemented correctly.

---

## Test Case Analysis

### Test Case Inventory (32 Total)

| Test Suite | Test Count | Focus Area | Milestone | Can Pass Now? |
|------------|-----------|------------|-----------|---------------|
| basic_1 | 1 | User management only | M1 | ✅ YES (verified) |
| basic_2 | 1 | User + more users | M1 | ✅ YES (likely) |
| basic_3 | 5 | Users + trains + tickets | M3/M4 | ❌ NO (tickets needed) |
| basic_4 | 5 | Users + trains + tickets + orders | M4 | ❌ NO (tickets + orders needed) |
| basic_5 | 10 | Full system incl. tickets + orders | M4/M5 | ❌ NO (tickets + orders needed) |
| basic_6 | 10 | Full system + transfers | M5 | ❌ NO (everything needed) |

**Total**: 32 test cases across 6 test suites

### Test Suite Command Coverage

#### basic_1 (1 test) - **CAN PASS** ✅
- Commands: add_user, login, logout, query_profile, modify_profile, exit
- **All commands implemented** ✅
- **Status**: VERIFIED PASSING (1134/1134 lines match per roadmap)

#### basic_2 (1 test) - **LIKELY CAN PASS** ✅
- Commands: add_user, login, logout, query_profile, modify_profile, clean, exit
- **All commands implemented** ✅
- **Status**: NOT VERIFIED (should be tested as part of M1 completion)

#### basic_3 (5 tests) - **CANNOT PASS** ❌
- Commands needed: add_train, release_train, query_ticket, buy_ticket, query_order
- **Missing**: query_ticket, buy_ticket, query_order (M3/M4 commands)
- Requires: M2 (trains) + M3 (query_ticket) + M4 (buy_ticket, query_order)

#### basic_4 (5 tests) - **CANNOT PASS** ❌
- Commands needed: add_train, release_train, query_ticket, query_train, buy_ticket, query_order
- **Missing**: query_ticket, buy_ticket, query_order (M3/M4 commands)
- Requires: M2 + M3 + M4

#### basic_5 (10 tests) - **CANNOT PASS** ❌
- Commands needed: Full system (all train and ticket commands)
- **Missing**: query_ticket, buy_ticket, query_order, query_transfer, refund_ticket
- Requires: M2 + M3 + M4 + M5 (query_transfer)

#### basic_6 (10 tests) - **CANNOT PASS** ❌
- Commands needed: Full system including query_transfer
- **Missing**: All ticket commands (M3/M4/M5)
- Requires: Complete implementation (M2-M5)

---

## Milestone Mapping

### M1: Foundation and User Management ✅ COMPLETE
- **Commands**: add_user, login, logout, query_profile, modify_profile, clean, exit (7 commands)
- **Test Coverage**: basic_1 (1 test case) - **VERIFIED PASSING** ✅
- **Status**: 100% complete, verified by Apollo

### M1.1: B+ Tree Bug Fixes ✅ COMPLETE
- **Purpose**: Fix critical B+ tree bugs before M2
- **Test Coverage**: 24 comprehensive B+ tree tests (by Magnus)
- **Status**: All bugs fixed, verified by Maya and team

### M2: Train Data Management 🔄 IN PROGRESS
- **Commands**: add_train, delete_train, release_train, query_train (4 commands)
- **Implementation**: ✅ All 4 commands coded in main.cpp
- **Test Coverage**: basic_2 (should cover more user tests)
- **Verification Status**: ⚠️ NOT VERIFIED - no test evidence yet
- **Next Step**: **Test against basic_2 to verify M2 is truly complete**

### M3: Ticket Query System ⏸️ NOT STARTED
- **Commands**: query_ticket (1 command)
- **Priority**: **HIGH** - query_ticket is SF (Super Frequent)
- **Test Coverage**: basic_3 (5 test cases require this)
- **Blocker**: M2 must be verified first

### M4: Ticket Purchasing and Order Management ⏸️ NOT STARTED
- **Commands**: buy_ticket, query_order, refund_ticket (3 commands)
- **Priority**: **CRITICAL** - buy_ticket is SF (Super Frequent)
- **Test Coverage**: basic_3, basic_4, basic_5 (20 test cases require this)
- **Blocker**: M3 must complete first

### M5: Transfer Query System ⏸️ NOT STARTED
- **Commands**: query_transfer (1 command)
- **Test Coverage**: basic_6 (10 test cases require this)
- **Blocker**: M4 must complete first

---

## Critical Implementation Gaps

### 1. **Ticket Command System (M3/M4/M5)** - CRITICAL GAP
**Impact**: Cannot pass 31 of 32 test cases (97% of tests)
- query_ticket (SF - Super Frequent) ❌
- buy_ticket (SF - Super Frequent) ❌
- query_order (F - Frequent) ❌
- refund_ticket (N - Normal) ❌
- query_transfer (N - Normal) ❌

**Recommendation**: This is the main implementation gap blocking progress.

### 2. **Seat Availability Tracking** - MISSING
**Required for**: buy_ticket, refund_ticket
**Current State**: Train struct has seatNum, but no per-date seat tracking
**Impact**: Cannot implement ticket purchasing without this

### 3. **Order Management System** - MISSING
**Required for**: query_order, refund_ticket, buy_ticket (standby queue)
**Current State**: No order storage structure exists
**Impact**: Cannot track user purchases or implement standby queue

### 4. **Station Index System** - MISSING
**Required for**: query_ticket, query_transfer (fast station-to-station search)
**Current State**: No index from station name to trains
**Impact**: query_ticket will require O(n) scan of all trains (too slow for SF operation)

### 5. **Standby Queue System** - MISSING
**Required for**: buy_ticket (-q true), refund_ticket (must process standby)
**Current State**: No queue structure or logic
**Impact**: Cannot handle pending orders or fulfill them on refund

---

## Recommended Next Steps

### Immediate Priority: Verify M2 Completion
1. **Test train commands against basic_2** to ensure M2 is truly complete
2. If basic_2 passes → Mark M2 complete
3. If basic_2 fails → Identify and fix train command bugs

### Next Milestone: M3 - Ticket Query System
**Why M3 Next?**
- Unblocks 20+ test cases (basic_3, basic_4, basic_5)
- query_ticket is SF (Super Frequent) - critical for performance scoring
- Provides foundation for M4 (buy_ticket needs query_ticket logic)
- Simpler than M4 (no order management complexity)

**M3 Requirements**:
1. **Station-to-train index**: Map station names → trains passing through
2. **Fast station-to-station search**: Find trains from station A to station B
3. **Sorting logic**: By time or cost (with trainID as tiebreaker)
4. **Date arithmetic**: Handle boarding date vs departure date
5. **Seat availability tracking**: Need to know remaining seats per segment

**Estimated Complexity**: Medium (12 cycles per roadmap)

---

## Risk Assessment

### High Risk Items

1. **Station Index Performance** ⚠️
   - query_ticket is SF (~1,000,000 operations)
   - Naive O(n) scan of all trains will timeout
   - **Mitigation**: Must build efficient station → train index

2. **Seat Availability Data Structure** ⚠️
   - Each train has 92 days × 99 segments = 9,108 seat counts
   - Memory constraint: 42-47 MiB total
   - **Mitigation**: Need compact storage (perhaps bit packing or run-length encoding)

3. **Standby Queue Complexity** ⚠️
   - Must check entire queue on each refund
   - Queue can be large (many pending orders)
   - **Mitigation**: Need efficient queue structure and fulfillment algorithm

4. **Query Transfer Optimization** ⚠️
   - Must find optimal single-transfer route
   - Large search space (train1 × transfer_station × train2)
   - **Mitigation**: Need smart pruning and optimization strategy

---

## Performance Priorities

Per README frequency annotations, optimize in this order:

### Super Frequent (SF) - ~1,000,000 operations
1. **query_profile** ✅ (O(1) hash table lookup - done)
2. **query_ticket** ❌ (NOT IMPLEMENTED - CRITICAL)
3. **buy_ticket** ❌ (NOT IMPLEMENTED - CRITICAL)

### Frequent (F) - ~100,000 operations
4. **login** ✅ (O(1) hash table lookup - done)
5. **logout** ✅ (O(1) hash table lookup - done)
6. **modify_profile** ✅ (O(1) hash table lookup - done)
7. **query_order** ❌ (NOT IMPLEMENTED)

### Normal (N) - ~10,000 operations
8-14. All other commands (add_user, add_train, query_train, etc.)

**Key Insight**: 3 of 4 SF commands are not implemented. This represents the highest performance risk.

---

## Edge Cases to Consider (From README Q&A)

### Date Handling
- ✅ Trains depart on different days
- ✅ Sale date = departure from starting station
- ✅ Query/buy date = departure from boarding station
- ⚠️ Train journey ≤ 3 days (must handle date arithmetic)

### Standby Queue Logic
- ✅ FIFO order (first in, first satisfied)
- ✅ Must fully satisfy order (all N tickets or none)
- ✅ If 49 seats left and want 50 → all 50 enter standby

### Privilege Checks
- ✅ Users with same privilege CANNOT modify each other
- ✅ New user privilege must be LOWER than creator's (not equal)

### Refund Logic
- ✅ Refund n-th order includes already-refunded orders
- ✅ Must process standby queue after refund

### Train Constraints
- ✅ No train passes through same station twice
- ✅ 2-100 stations per train
- ✅ Can query train before release (tickets show as available)
- ✅ Cannot delete train after release

### Query Behavior
- ✅ query_ticket returns 0 (not -1) if no station exists
- ✅ Show trains even if no tickets available
- ✅ Tie-breaking: trainID lexicographic order

---

## Conclusion

**Current Implementation Status**: 64% complete (9/14 commands)
**Test Pass Capability**: 6% (2/32 test cases)
**Blocker**: Ticket management system (M3/M4/M5)

**Recommended Action**:
1. ✅ Verify M2 by testing against basic_2
2. 🎯 **START M3** - Implement query_ticket (enables 20 more test cases)
3. Proceed sequentially: M3 → M4 → M5 → M6

**Timeline Estimate** (per roadmap):
- M2 verification: 1 cycle
- M3 (query_ticket): 12 cycles
- M4 (buy/query/refund): 14 cycles
- M5 (query_transfer): 10 cycles
- M6 (optimization): 12 cycles
- M7 (testing): 8 cycles
- **Total remaining**: ~57 cycles

**Success Probability**: High if we follow incremental approach and verify each milestone before proceeding.
