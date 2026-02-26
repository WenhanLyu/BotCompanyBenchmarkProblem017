#!/usr/bin/env python3
"""Calculate exact pass rates for basic_3 sequential tests - Issue #39"""

import os

# Test results
results = [
    {"test": 1, "matching": 1548, "total": 1551},
    {"test": 2, "matching": 1504, "total": 1522},
    {"test": 3, "matching": 1547, "total": 1601},
    {"test": 4, "matching": 1684, "total": 1801},
    {"test": 5, "matching": 1836, "total": 2067},
]

print("=" * 60)
print("BASIC_3 SEQUENTIAL TEST RESULTS - ISSUE #39")
print("After Alex's Fix (commit 97f1a05)")
print("=" * 60)
print()

# Calculate individual test pass rates
total_matching = 0
total_lines = 0

for r in results:
    matching = r["matching"]
    total = r["total"]
    pass_rate = (matching / total * 100) if total > 0 else 0

    status = "✅" if pass_rate >= 95 else "⚠️" if pass_rate >= 90 else "❌"

    print(f"Test {r['test']}: {matching:4d} / {total:4d} lines  ({pass_rate:6.2f}%)  {status}")

    total_matching += matching
    total_lines += total

print("-" * 60)

# Calculate overall pass rate
overall_pass_rate = (total_matching / total_lines * 100) if total_lines > 0 else 0

status = "✅" if overall_pass_rate >= 98 else "⚠️" if overall_pass_rate >= 95 else "❌"

print(f"Overall: {total_matching:4d} / {total_lines:4d} lines  ({overall_pass_rate:6.2f}%)  {status}")
print("=" * 60)
print()

# Check for progressive degradation
print("PROGRESSIVE DEGRADATION ANALYSIS:")
print("-" * 60)
prev_rate = 100.0
degraded = False

for r in results:
    matching = r["matching"]
    total = r["total"]
    pass_rate = (matching / total * 100) if total > 0 else 0

    drop = prev_rate - pass_rate
    if drop > 2.0:  # More than 2% drop
        print(f"Test {r['test']}: {pass_rate:.2f}% (↓ {drop:.2f}% from previous)")
        degraded = True
    else:
        print(f"Test {r['test']}: {pass_rate:.2f}% (stable)")

    prev_rate = pass_rate

print("-" * 60)

if degraded:
    print("⚠️  PROGRESSIVE DEGRADATION DETECTED")
else:
    print("✅ NO SIGNIFICANT PROGRESSIVE DEGRADATION")

print()

# Compare to target
print("TARGET COMPARISON:")
print("-" * 60)
target = 98.8
diff = overall_pass_rate - target

if overall_pass_rate >= target:
    print(f"✅ PASS - Meets target of {target}%")
    print(f"   Actual: {overall_pass_rate:.2f}% (+{diff:.2f}%)")
else:
    print(f"❌ FAIL - Below target of {target}%")
    print(f"   Actual: {overall_pass_rate:.2f}% ({diff:.2f}%)")

print("=" * 60)
