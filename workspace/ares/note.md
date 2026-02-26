# Ares - Cycle 1 (M3: query_ticket)

## Milestone Status
**M3: Ticket Query System (query_ticket command)** - In Progress
- **Cycles Used**: 1 of 12
- **Status**: Bug identified, team assembled, fix in progress

## Critical Bug Identified

**Bug**: query_ticket returns `train.seatNum` (total capacity) instead of available seats

**Evidence** (from Sam's previous testing):
- Expected: `LeavesofGrass ... 2295 75921` (75921 available seats)
- Actual: `LeavesofGrass ... 1135 98306` (98306 is total capacity)
- Pattern: All outputs show 98306 instead of varying seat availability

**Root Cause** (main.cpp:887):
```cpp
result.seat = train.seatNum;  // ❌ Wrong - returns total capacity
```

**Should be**:
```cpp
result.seat = checkAvailableSeats(train.trainID, start_date, from_idx, to_idx);
```

## Analysis

**Files to modify**:
1. `main.cpp` - Include order_system.hpp and fix query_ticket
2. Need to declare global BPTree variables: `orders` and `seats`
3. Need to open B+ tree files in main()
4. Need to call `initializeSeatsForTrain()` when train is released

**Solution approach**:
- Use existing `checkAvailableSeats()` function from order_system.hpp
- Initialize seat availability when trains are released
- Handle edge case: if no seat data exists yet, return train.seatNum

## Team Hired

1. **Leo** - Implementation Engineer (reports_to: ares)
   - Skilled C++ developer for implementing fixes

2. **Maya** - Test Engineer (reports_to: ares)
   - Test verification specialist

## Schedule

**Cycle 1**:
- Leo: Fix query_ticket seat availability bug
- Maya: Test against basic_3 and basic_4 after Leo's fix

## Next Cycle

If tests pass, claim milestone complete.
If tests fail, analyze failures and iterate.
