#!/usr/bin/env python3
"""Analyze test failures to categorize ordering vs value errors"""

import sys
from collections import Counter

def analyze_diff_file(diff_path):
    """Parse diff file and categorize errors"""
    with open(diff_path, 'r') as f:
        lines = f.readlines()

    deletions = []
    additions = []

    i = 0
    while i < len(lines):
        line = lines[i].rstrip()
        if line.startswith('< '):
            deletions.append(line[2:])
        elif line.startswith('> '):
            additions.append(line[2:])
        i += 1

    # Find matching lines (ordering issues)
    deleted_set = set(deletions)
    added_set = set(additions)

    ordering_issues = deleted_set & added_set  # Lines that appear in both
    value_errors_deleted = deleted_set - ordering_issues
    value_errors_added = added_set - ordering_issues

    return {
        'total_deletions': len(deletions),
        'total_additions': len(additions),
        'ordering_issues': len(ordering_issues),
        'value_errors': len(value_errors_deleted) + len(value_errors_added),
        'query_order_ordering': sum(1 for line in ordering_issues if line.strip().startswith('[')),
    }

# Analyze all 5 tests
print("Basic_3 Failure Analysis")
print("=" * 70)

total_ordering = 0
total_value = 0
total_query_order_ordering = 0

for i in range(1, 6):
    diff_file = f'workspace/workspace/sam/basic3_results/test_{i}_diff.txt'
    try:
        result = analyze_diff_file(diff_file)
        total_ordering += result['ordering_issues']
        total_value += result['value_errors']
        total_query_order_ordering += result['query_order_ordering']

        print(f"\nTest {i}:")
        print(f"  Ordering issues: {result['ordering_issues']} lines")
        print(f"  Value errors:    {result['value_errors']} lines")
        print(f"  query_order ordering issues: {result['query_order_ordering']} lines")
    except FileNotFoundError:
        print(f"\nTest {i}: Diff file not found")

print(f"\n{'=' * 70}")
print(f"Total Ordering Issues: {total_ordering} lines")
print(f"  - query_order sorting: {total_query_order_ordering} lines")
print(f"Total Value Errors:    {total_value} lines")
print(f"{'=' * 70}")

# Calculate what pass rate would be if ordering was fixed
total_lines = 1551 + 1522 + 1601 + 1801 + 2067
current_matching = 1548 + 1504 + 1547 + 1684 + 1836
potential_matching = current_matching + total_ordering

print(f"\nCurrent pass rate:  {current_matching}/{total_lines} = {100*current_matching/total_lines:.2f}%")
print(f"If ordering fixed:  {potential_matching}/{total_lines} = {100*potential_matching/total_lines:.2f}%")
print(f"Target:             98.8%")
