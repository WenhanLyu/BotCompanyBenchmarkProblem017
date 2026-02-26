# Apollo - Verification Plan for M3.1: query_order Implementation

## Milestone Requirements

Implement ONLY the query_order command to retrieve user order history.
- Infrastructure exists (Order struct, OrderKey, queryUserOrders helper)
- Return orders sorted by timestamp with format: [TIMESTAMP] [TRAINID] [FROM] [TO] [STATUS] [PRICE] [COUNT]
- Handle edge cases (no orders, invalid user)
- Do NOT implement other features
- Do NOT fix query_ticket bugs
- Do NOT implement refund_ticket
- This is F frequency command blocking basic_3 test (140+ commands)

## Implementation Review

**Commit:** 0bb63f6 [Leo] Implement query_order command
**Files changed:** main.cpp only (67 lines added)
**Implementation:** Added cmd_query_order() function and main loop handler

## Verification Team

1. **Blake** - Implementation Scope Verifier
   - Task: Verify ONLY query_order was implemented, no unintended changes

2. **Quinn** - Code Quality Verifier
   - Task: Review cmd_query_order implementation for correctness

3. **Ursula** - Format Compliance Verifier
   - Task: Verify output format matches specification exactly

4. **Trina** - Test Results Verifier
   - Task: Verify Leo's manual test results

5. **Nina** - Basic_3 Test Runner
   - Task: Run basic_3 and verify all 140 query_order commands work

## Success Criteria

ALL of the following must pass:
- ✓ Only query_order implemented (no other features/fixes)
- ✓ Code is correct and uses existing infrastructure
- ✓ Output format exactly matches specification
- ✓ Edge cases handled correctly
- ✓ Leo's manual tests pass
- ✓ All 140 query_order commands in basic_3 pass

If ANY criterion fails, the milestone FAILS.
