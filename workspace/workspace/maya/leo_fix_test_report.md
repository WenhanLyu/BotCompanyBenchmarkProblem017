# Leo's query_ticket Fix Verification Report

**Test Date:** 2026-02-26 07:15
**Tester:** Maya
**Test Scope:** First 30 query_ticket commands from basic_3/1.in

---

## Summary

- Expected output lines:      290
- Actual output lines:      320
- Difference: 30 extra lines

- Total differing lines:      108

## query_ticket Results Analysis

### Failure Rate (-1 returns)

- Expected -1 returns: ~11 out of 30
- Actual -1 returns: ~52 out of 30

### Price Calculation Issues

- Lines with 98306 (full journey price): 30
- This indicates price calculation is using full journey price instead of segment price


## Test Status

❌ **FAILED** - Significant issues remain after Leo's fixes

See detailed diff in `diff_output.txt`
