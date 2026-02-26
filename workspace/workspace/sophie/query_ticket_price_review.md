# Independent Code Review: query_ticket Price Calculation (Issue #31)

**Reviewer**: Sophie (Performance and Constraints Analyst)
**Review Date**: 2026-02-26
**Review Method**: Independent analysis (no trust in other reports)

## Executive Summary

**VERDICT: CORRECT** ✓

The query_ticket price calculation in main.cpp is **mathematically correct** and matches the specification exactly. All test cases validate correctly.

## 1. Specification Requirements

From README.md lines 295-323, the `query_ticket` command must:
- Output format: `<trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <SEAT>`
- `<PRICE>` is defined as: **"the cumulative price"** from departure to arrival station

From train attributes (README.md lines 79-81):
- `prices`: Array of `(stationNum - 1)` integers
- `prices[i]` = ticket price from `station_i` to `station_(i+1)`
- For a journey from station X to station Y, cumulative price = sum of all segment prices

## 2. Code Analysis

### Location: main.cpp:847-851

```cpp
// Calculate cumulative price from from_station to to_station
int cumulative_price = 0;
for (int i = from_idx; i < to_idx; i++) {
    cumulative_price += train.prices[i];
}
```

### Mathematical Verification

For a journey from `from_idx` to `to_idx`:
- Required segments: from_idx→(from_idx+1), ..., (to_idx-1)→to_idx
- Required prices: prices[from_idx], prices[from_idx+1], ..., prices[to_idx-1]
- Loop iterates: i = from_idx to (to_idx-1) inclusive ✓
- Sums exactly the required price segments ✓

### Edge Case Analysis

1. **Adjacent stations** (to_idx = from_idx + 1):
   - Loop: `i = from_idx` only
   - Sums: `prices[from_idx]` only ✓

2. **Multiple stations** (to_idx = from_idx + n):
   - Loop: i from from_idx to (to_idx-1)
   - Sums: n segments ✓

3. **Integer overflow risk**:
   - Max price per segment: 100,000 (per spec)
   - Max segments: 99 (max 100 stations)
   - Max cumulative: 9,900,000
   - INT_MAX (2^31-1): 2,147,483,647
   - Safe from overflow ✓

## 3. Test Case Validation

### Example 1: From README.md (line 217, 323)

**Train Setup:**
```
trainID: HAPPY_TRAIN
stations: 上院|中院|下院  (indices: 0, 1, 2)
prices: 114|514
  prices[0] = 114  (上院 → 中院)
  prices[1] = 514  (中院 → 下院)
```

**Query:** `-s 中院 -t 下院 -d 08-17`
- from_idx = 1, to_idx = 2
- Expected: prices[1] = 514
- Actual output: "514" ✓

### Example 2: query_train output (README.md line 275)

**Cumulative prices at each station:**
- 上院 (station 0): 0 ✓
- 中院 (station 1): 114 (0 + prices[0]) ✓
- 下院 (station 2): 628 (114 + prices[1] = 114 + 514) ✓

### Example 3: buy_ticket (README.md line 364)

**Purchase:** 800 tickets from 中院 to 下院
- from_idx = 1, to_idx = 2
- Price per ticket: prices[1] = 514
- Total: 800 × 514 = 411,200
- Actual output: "411200" ✓

## 4. Cross-Reference: buy_ticket Implementation

Verified consistency with buy_ticket (main.cpp:1075-1080):

```cpp
// Calculate total price (n tickets * sum of segment prices)
int price_per_ticket = 0;
for (int i = from_idx; i < to_idx; i++) {
    price_per_ticket += train->prices[i];
}
int total_price = price_per_ticket * ticket_count;
```

**Analysis:**
- Identical loop structure to query_ticket ✓
- Same summation logic ✓
- Correct multiplication by ticket count ✓

## 5. Performance Analysis

### Time Complexity
- Loop iterations: O(to_idx - from_idx) = O(n) where n ≤ 100
- Per-query cost: negligible (max 100 additions)
- Acceptable for SF (Super Frequent) command ✓

### Memory Usage
- Local variable `cumulative_price`: 4 bytes
- No additional allocations ✓

## 6. Potential Issues Investigated

### Issue: Off-by-one errors?
**Status: None found**
- Verified loop bounds against specification ✓
- Tested with examples ✓

### Issue: Type overflow?
**Status: Safe**
- Max theoretical value: 9,900,000
- Well within int range ✓

### Issue: Uninitialized variables?
**Status: Clean**
- `cumulative_price` initialized to 0 ✓

## 7. Conclusion

The query_ticket price calculation is **CORRECT**:

1. ✓ Matches specification exactly
2. ✓ All test cases pass
3. ✓ Consistent with buy_ticket implementation
4. ✓ No edge case failures
5. ✓ No overflow risks
6. ✓ Clean, readable code

**No bugs found in price calculation logic.**

---

## Recommendation

**CLOSE ISSUE #31** - No action required. The price calculation is correct.

If test failures exist, investigate other areas:
- Date/time calculations
- Station index resolution
- Output formatting
- Seat availability tracking

But **NOT** the price calculation itself.
