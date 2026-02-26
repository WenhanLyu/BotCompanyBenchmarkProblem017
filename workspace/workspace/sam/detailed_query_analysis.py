#!/usr/bin/env python3
"""
Detailed analysis of query_ticket failures
"""

import re
from pathlib import Path

def analyze_failures():
    workspace = Path(__file__).parent
    project_root = workspace.parent.parent.parent
    data_dir = project_root / "data" / "017" / "data"

    # Analyze basic_3
    print("Analyzing basic_3 failures in detail...")
    print("=" * 60)

    # Get combined input
    combined_input = workspace / "basic_3_combined.in"
    if not combined_input.exists():
        # Create it
        with open(combined_input, 'w') as out:
            for i in range(1, 6):
                with open(data_dir / "basic_3" / f"{i}.in", 'r') as f:
                    out.write(f.read())

    # Read commands
    with open(combined_input, 'r') as f:
        commands = [line.strip() for line in f if line.strip().startswith('query_ticket')]

    # Read expected and actual outputs
    expected_output = data_dir / "basic_3" / "out.txt"
    actual_output = project_root / "basic_3_actual_output.txt"

    with open(expected_output, 'r') as f:
        expected_lines = [line.strip() for line in f]

    with open(actual_output, 'r') as f:
        actual_lines = [line.strip() for line in f]

    print(f"Commands: {len(commands)}")
    print(f"Expected lines: {len(expected_lines)}")
    print(f"Actual lines: {len(actual_lines)}")
    print()

    # Analyze the mismatch
    # The issue is that we're getting fewer output lines than expected
    # This suggests the program is missing some outputs

    # Count how many query_ticket commands there are
    query_count = len(commands)

    # Try to match outputs more carefully
    # Let's find patterns of failures

    # Read line by line and try to understand the structure
    exp_idx = 0
    act_idx = 0

    failures_by_type = {
        'missing_zero': 0,
        'missing_results': 0,
        'wrong_results': 0,
        'extra_output': 0
    }

    cmd_idx = 0
    failure_examples = []

    while exp_idx < len(expected_lines) and cmd_idx < len(commands):
        exp_line = expected_lines[exp_idx]

        # Skip non-query_ticket outputs in expected (from other commands)
        # We need to identify query_ticket results
        # Query results are: "0" or train results

        # This is getting complex. Let's take a simpler approach:
        # Compare line-by-line until we find mismatches

        exp_idx += 1
        cmd_idx += 1

    # Simpler approach: Just count the types of expected vs actual patterns
    zero_count_expected = expected_lines.count('0')
    zero_count_actual = actual_lines.count('0')

    # Count train result lines (format: "TrainID seats price")
    train_results_expected = len([l for l in expected_lines if re.match(r'^[A-Za-z0-9_-]+ \d+ \d+$', l)])
    train_results_actual = len([l for l in actual_lines if re.match(r'^[A-Za-z0-9_-]+ \d+ \d+$', l)])

    print(f"Expected '0' outputs: {zero_count_expected}")
    print(f"Actual '0' outputs: {zero_count_actual}")
    print(f"Missing zeros: {zero_count_expected - zero_count_actual}")
    print()

    print(f"Expected train result lines: {train_results_expected}")
    print(f"Actual train result lines: {train_results_actual}")
    print(f"Difference: {train_results_expected - train_results_actual}")
    print()

    # Check for "query_order" in commands (should be ignored per spec)
    with open(combined_input, 'r') as f:
        all_lines = [line.strip() for line in f]

    query_order_count = len([l for l in all_lines if l.startswith('query_order')])
    refund_ticket_count = len([l for l in all_lines if l.startswith('refund_ticket')])
    buy_ticket_count = len([l for l in all_lines if l.startswith('buy_ticket')])

    print(f"Other commands in test:")
    print(f"  query_order: {query_order_count} (should be ignored)")
    print(f"  refund_ticket: {refund_ticket_count} (should be ignored)")
    print(f"  buy_ticket: {buy_ticket_count}")
    print()

    # Sample some mismatches
    print("Sampling output comparison:")
    print("-" * 60)
    print("Expected output (first 50 lines):")
    for i, line in enumerate(expected_lines[:50], 1):
        print(f"{i:4d}: {line}")
    print()
    print("Actual output (first 50 lines):")
    for i, line in enumerate(actual_lines[:50], 1):
        print(f"{i:4d}: {line}")

if __name__ == "__main__":
    analyze_failures()
