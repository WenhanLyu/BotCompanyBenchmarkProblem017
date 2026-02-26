#!/usr/bin/env python3
"""Calculate pass rate for basic_3 test suite (sequential runs)"""

import sys

def calculate_pass_rate(expected_file, actual_file):
    """Calculate line-by-line pass rate"""
    with open(expected_file, 'r') as f:
        expected = f.readlines()
    with open(actual_file, 'r') as f:
        actual = f.readlines()

    total_lines = len(expected)
    matching_lines = 0

    # Compare line by line
    max_len = max(len(expected), len(actual))
    for i in range(max_len):
        exp_line = expected[i].strip() if i < len(expected) else None
        act_line = actual[i].strip() if i < len(actual) else None

        if exp_line == act_line:
            matching_lines += 1

    pass_rate = (matching_lines / total_lines * 100) if total_lines > 0 else 0

    return {
        'total': total_lines,
        'matching': matching_lines,
        'pass_rate': pass_rate,
        'expected_lines': len(expected),
        'actual_lines': len(actual)
    }

# Run for all 5 tests
total_lines = 0
total_matching = 0

print("Basic_3 Test Suite Results (SEQUENTIAL RUNS)")
print("=" * 60)

for i in range(1, 6):
    expected = f'/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/data/017/data/basic_3/{i}.out'
    actual = f'basic_3_{i}_sequential.txt'

    result = calculate_pass_rate(expected, actual)
    total_lines += result['total']
    total_matching += result['matching']

    print(f"\nTest {i}:")
    print(f"  Expected lines: {result['expected_lines']}")
    print(f"  Actual lines:   {result['actual_lines']}")
    print(f"  Matching:       {result['matching']}/{result['total']}")
    print(f"  Pass rate:      {result['pass_rate']:.2f}%")

overall_pass_rate = (total_matching / total_lines * 100) if total_lines > 0 else 0
print(f"\n{'=' * 60}")
print(f"Overall Pass Rate: {total_matching}/{total_lines} = {overall_pass_rate:.2f}%")
print(f"{'=' * 60}")
print(f"\nBaseline (before refund_ticket): 98.8%")
print(f"Current (with refund_ticket):    {overall_pass_rate:.2f}%")
print(f"Change:                          {overall_pass_rate - 98.8:+.2f}%")
