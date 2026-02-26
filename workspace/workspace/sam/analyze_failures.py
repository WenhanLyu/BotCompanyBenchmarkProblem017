#!/usr/bin/env python3
"""Analyze failure patterns in basic_3 test results - Issue #39"""

import re
from pathlib import Path

def analyze_diff(test_num):
    """Analyze differences for a specific test"""
    diff_file = Path(f"workspace/workspace/sam/basic3_results/test_{test_num}_diff.txt")

    if not diff_file.exists():
        return None

    with open(diff_file, 'r') as f:
        content = f.read()

    # Count different types of differences
    removed_lines = re.findall(r'^-(.+)$', content, re.MULTILINE)
    added_lines = re.findall(r'^\+(.+)$', content, re.MULTILINE)

    # Filter out diff metadata
    removed_lines = [l for l in removed_lines if not l.startswith('---')]
    added_lines = [l for l in added_lines if not l.startswith('+++')]

    # Count specific patterns
    zero_to_minus_one = 0
    minus_one_to_zero = 0
    order_list_changes = 0

    for i in range(min(len(removed_lines), len(added_lines))):
        rem = removed_lines[i].strip()
        add = added_lines[i].strip()

        if rem == "0" and add == "-1":
            zero_to_minus_one += 1
        elif rem == "-1" and add == "0":
            minus_one_to_zero += 1
        elif rem.startswith("[") and add.startswith("["):
            order_list_changes += 1

    return {
        "removed": len(removed_lines),
        "added": len(added_lines),
        "zero_to_minus_one": zero_to_minus_one,
        "minus_one_to_zero": minus_one_to_zero,
        "order_list_changes": order_list_changes
    }

print("=" * 70)
print("FAILURE PATTERN ANALYSIS - ISSUE #39")
print("=" * 70)
print()

for test_num in range(1, 6):
    analysis = analyze_diff(test_num)
    if analysis:
        print(f"Test {test_num}:")
        print(f"  Lines removed: {analysis['removed']}")
        print(f"  Lines added: {analysis['added']}")
        print(f"  0 → -1 changes: {analysis['zero_to_minus_one']}")
        print(f"  -1 → 0 changes: {analysis['minus_one_to_zero']}")
        print(f"  Order list reordering: {analysis['order_list_changes']}")
        print()

print("=" * 70)
print("PATTERN SUMMARY:")
print("-" * 70)
print()
print("1. PROGRESSIVE DEGRADATION:")
print("   - Failures increase with each test (31→144→343→549→1077 diff lines)")
print("   - Corresponds to increased query_order usage (15→21→23→34→47)")
print()
print("2. MAIN FAILURE TYPES:")
print("   a) Query order sorting issues - orders appear in wrong sequence")
print("   b) Return value discrepancies (0 vs -1)")
print()
print("3. ROOT CAUSE HYPOTHESIS:")
print("   - query_order sorting may be unstable or incorrect")
print("   - Possible B+ tree traversal issue")
print("   - State accumulation causing corruption over time")
print()
print("=" * 70)
