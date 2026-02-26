# Maya's Work Summary - Issue #40: query_order Sorting Bug

## Assignment
Fix query_order sorting bug causing orders to appear in wrong sequence. Progressive degradation: 0 → 15 → 42 → 108 → 189 instances across tests 1-5. Target: 98.8%+ pass rate.

## Investigation

### Tests Conducted
1. ✅ B+ tree unit tests (simple, stress, persistence, multi-user) - ALL PASSED
2. ✅ Direct orders.dat file inspection - All users have perfectly sorted orders
3. ❌ Actual system test (basic_3) - Shows ordering discrepancies

### Critical Finding
**Data is stored correctly** - Direct B+ tree file inspection shows all 120 users have orders in perfect timestamp sequence.

**Data retrieval is inconsistent** - query_order produces different order than expected, despite:
- Data being correctly stored
- forEach appearing to work in synthetic tests
- Adding explicit sorting to query_order

### Fix Attempted
Added explicit sorting by timestamp in `main.cpp`:
```cpp
// Line ~1145 in cmd_query_order
std::sort(order_list.begin(), order_list.end(), [](const Order& a, const Order& b) {
    return a.timestamp < b.timestamp;
});
std::reverse(order_list.begin(), order_list.end());

// Line ~1268 in cmd_refund_ticket (same fix)
```

**Result**: Ordering discrepancies persist. Test shows 30 order-related differences vs expected output.

### Analysis
Since explicit sorting didn't fix the issue, the root cause is likely:

1. **B+ tree forEach non-determinism** - Returns different order on different invocations
2. **Delete-then-insert corruption** - updateOrderStatus pattern degrades leaf chain integrity over time
3. **Cache interference** - Mutable `find_result_node` cache may corrupt iteration state

### Evidence
After sorting BOTH actual and expected outputs alphabetically:
- Only 4 real differences remain (price calculation bugs, not ordering)
- This confirms ordering is the primary issue
- Example: Same 3 orders appear as {July 7, Aug 3, July 29} vs {July 29, July 7, Aug 3}

## Root Cause Hypothesis
B+ tree forEach has subtle bug not caught by synthetic tests. Likely scenarios:
1. Leaf chain (`next_leaf` pointers) corrupted after many delete-then-insert operations
2. forEach sometimes starts from wrong leaf node
3. Memory corruption/uninitialized reads in specific conditions

## Files Modified
- `main.cpp`:
  - cmd_query_order (~line 1145): Added timestamp sorting
  - cmd_refund_ticket (~line 1268): Added timestamp sorting

## Files Created
- `test_bptree_ordering.cpp`: Simple delete-then-insert test (PASSES)
- `test_bptree_stress.cpp`: 150 orders with updates (PASSES)
- `test_bptree_persistence.cpp`: Sequential runs with persistence (PASSES)
- `test_bptree_multiuser.cpp`: 5 users, 250 orders (PASSES)
- `check_actual_orders.cpp`: Direct file inspection (shows perfect ordering)
- `investigation_report.md`: Detailed analysis

## Status
**INCOMPLETE** - Sorting fix implemented but ineffective. Issue requires deeper B+ tree debugging.

### Why Sorting Didn't Work
If forEach returns items in consistent order, sorting would normalize it. Since sorting failed to fix the issue, forEach must be returning DIFFERENT orders on different calls - a serious non-determinism bug.

## Recommendations for Next Cycle

### Immediate Actions
1. Add debug logging to B+ tree forEach to trace traversal path
2. Log the exact sequence of next_leaf pointers followed
3. Compare forEach results before/after updateOrderStatus operations
4. Identify where leaf chain corruption occurs

### Alternative Fixes
1. **Eliminate delete-then-insert**: Modify updateOrderStatus to update in-place
2. **Rebuild iteration**: Don't rely on forEach - collect all matching keys, sort, then retrieve
3. **Fix forEach**: Debug and fix the root cause in bptree.hpp

### Critical Files to Investigate
- `bptree.hpp:561-588` - forEach implementation
- `bptree.hpp:419-490` - insert with split logic
- `bptree.hpp:515-540` - remove logic
- `order_system.hpp:232-249` - updateOrderStatus delete-then-insert pattern

## Time Spent
Approximately 1 hour on investigation and attempted fix.

## Escalation Note
This issue is beyond a simple fix. The B+ tree implementation has a subtle bug that:
- Doesn't appear in unit tests
- Only manifests after specific operation sequences
- Causes progressive degradation (gets worse over time)
- Is masked by correct data storage (bug is in retrieval, not storage)

Recommend: Assign to data structures specialist or schedule deeper debugging session.
