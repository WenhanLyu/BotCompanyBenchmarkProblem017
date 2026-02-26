# Cora's Notes - Issue #42 Analysis

## Task
Analyze query_order sorting implementation to identify why orders appear in wrong sequence.

## Findings

### Root Cause: Missing initOrderSystem() call
The `main()` function does NOT call `initOrderSystem()`, causing the order counter to reset to 0 on every program restart. This leads to timestamp collisions between old (persisted) and new orders.

### Secondary Issue: Non-stable sorting
Using `std::sort` instead of `std::stable_sort` causes non-deterministic ordering when timestamps collide.

## Bug Details

**Bug #1 (CRITICAL):** main.cpp:1314
- Missing `initOrderSystem();` call after `load_users()`
- Causes g_order_counter to reset to 0 on every restart
- Results in timestamp collisions

**Bug #2 (MEDIUM):** main.cpp:1151 and 1274
- Using `std::sort` instead of `std::stable_sort`
- Produces non-deterministic ordering on duplicate timestamps
- Amplifies the impact of Bug #1

## Why Previous Fix Failed

Maya added timestamp sorting (Issue #40), but it didn't work because:
- Sorting assumes unique timestamps
- With collisions, `std::sort` is non-deterministic
- Different runs produce different orderings

Maya suspected "B+ tree forEach non-determinism", but the actual issue is sorting non-determinism on duplicate keys.

## Recommended Fix

1. Add `initOrderSystem();` in main() after load_users()
2. Change `std::sort` to `std::stable_sort` in both query_order and refund_ticket
3. Same fix applies to cmd_refund_ticket (line 1274)

## Deliverable

Full analysis in: `workspace/workspace/cora/issue_42_analysis.md`

## Confidence

HIGH - Clear bug with predictable consequences that explains all observed symptoms.
