# Detailed query_ticket Fix Verification

**Date:** 2026-02-26 07:16
**Test:** First 30 query_ticket commands from basic_3

---

## Test Results Summary

| Metric | Expected | Actual | Status |
|--------|----------|--------|--------|
| Output lines | 290 | 320 | ❌ +30 extra |
| query_ticket commands | 30 | 30 | ✅ |

## Critical Issues Found

### Issue 1: Excessive -1 Returns (Route Finding Failures)

Many query_ticket commands return -1 when they should return valid results.

**Examples:**

```
Command #1: query_ticket -s 广东省四会市 -t 新疆伊宁市 -d 08-02 -p cost
Expected: 7142328
Actual:   -1

Command #2: (line 155)
Expected: 53744575
Actual:   -1

Command #3-5: (lines 156-158)
Expected: 0, 6699568, 2744047
Actual:   -1, -1, -1
```

### Issue 2: Wrong Price Calculation

When query_ticket does succeed, it calculates the wrong price (uses full journey price).

**Examples:**

```
Command: query_ticket -s 浙江省杭州市 -t 河南省灵宝市 -d 07-04 -p time
Expected: LeavesofGrass ... 2295 75921
Actual:   LeavesofGrass ... 2295 98306
Issue:    Price should be 75921 (segment), not 98306 (full journey)

Command: query_ticket -s 新疆塔城市 -t 黑龙江省海林市 -d 08-04 -p time
Expected: LeavesofGrass ... 2486 91170
Actual:   LeavesofGrass ... 2486 98306
Issue:    Price should be 91170 (segment), not 98306 (full journey)
```

## Leo's Fixes - Impact Assessment

Leo made two fixes:
1. Date calculation boundary handling bug (commit 14cb541)
2. Seat availability calculation bug (commit 2f3d8cc by Diana)

**Result:** These fixes did NOT resolve the core query_ticket issues.

### Remaining Bugs

1. **Route Finding Algorithm:** Still fails for many valid routes
   - Possible causes: train filtering, date range validation, station matching

2. **Price Calculation:** Still using full journey price instead of segment price
   - Current: total_price = train.prices[n-1] (or sum of all)
   - Should be: total_price = sum(train.prices[from_idx..to_idx-1])

## Conclusion

❌ **Leo's fixes have NOT resolved the query_ticket issues.**

The test shows:
- ~60% of query_ticket commands still return -1 (should be ~30-40%)
- 100% of successful queries return wrong prices
- 108 lines differ from expected output

**Next Steps:**
1. Debug route finding algorithm - add logging to understand why valid routes fail
2. Fix price calculation to use segment-based pricing
3. Re-test with this same 30-command subset
