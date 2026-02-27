# Ares - M4 Implementation

**Date**: 2026-02-26
**Milestone**: M4 - Implement query_transfer command
**Cycles**: 2 of 12

---

## Objective

Implement the final command (query_transfer) to achieve 100% feature completeness (16/16 commands). Must find optimal single-transfer routes between stations, handle time/cost optimization, validate transfer timing with multi-day date arithmetic, check seat availability, and pass all 12 query_transfer test cases in basic_3 without regressing the current 99.78% pass rate.

## Command Specification

From README.md:
- **Format**: Same parameters as query_ticket (-s -t -d -p)
- **Purpose**: Find optimal route with exactly ONE transfer between two trains
- **Output**:
  - `0` if no valid transfer exists
  - 2 lines (same format as query_ticket) if successful
- **Optimization**:
  - `-p time`: minimize total travel time
  - `-p cost`: minimize total price
- **Tie-breaking**:
  1. Lexicographic trainID order
  2. Prefer less riding time on train 1

## Implementation Strategy

### Algorithm
1. Nested iteration over all released train pairs (Train1, Train2)
2. For each pair, find common stations (potential transfer points)
3. For each transfer point, validate:
   - Train1: from_station → transfer_station
   - Train2: transfer_station → to_station
   - Date/time compatibility (Train1 arrival at transfer < Train2 departure)
   - Account for stopover time at transfer station
   - Seat availability on both segments
4. Calculate total time and cost for each valid route
5. Sort by optimization criterion (time or cost), then apply tie-breaking
6. Output optimal solution (first result after sorting)

### Key Challenges
- **Date arithmetic**: Reverse-engineer start dates for both trains
- **Multi-day journeys**: Trains can span 2-3 days
- **Transfer timing**: Ensure proper stopover/wait time at transfer station
- **Seat availability**: Check seats on both segments with correct date ranges
- **Tie-breaking**: Lexicographic trainID comparison, then less Train1 time

## Test Strategy
- basic_3 has 12 query_transfer commands (3+1+5+1+2 across tests 1-5)
- Must not regress current 99.78% pass rate
- Target: All query_transfer commands pass

## Team Status

**My team** (reporting to Ares):
- Alex
- Diana
- Leo (assigned issue #46)
- Maya
- Sam

---

## Cycle 2 Progress

### Issues Managed
- ✅ Closed issue #5 (persistence) - working correctly
- ✅ Closed issue #30 (buy_ticket queue) - Diana fixed it
- ✅ Closed issue #40 (query_order sorting) - minor issues remain but not blocking (99.78% pass rate)
- ✅ Closed issue #41 (final verification) - superseded by #45
- ✅ Closed issue #45 (M3.2.2 verification) - Sam verified 99.78% pass rate
- ✅ Created issue #46 (implement query_transfer) - assigned to Leo

### Current Status
- M3.2.2 complete with 99.78% pass rate (verified by Sam)
- query_transfer is the final missing command (15/16 implemented)
- Scheduling Leo to implement query_transfer

### Next Steps
1. Leo implements query_transfer (issue #46)
2. Maya or Sam tests implementation
3. Verify no regression in pass rate
4. Claim complete once all tests pass
