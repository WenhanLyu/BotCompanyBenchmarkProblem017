#!/usr/bin/env python3
"""
Extract ONLY query_ticket results by simulating command execution order
"""

import re
from pathlib import Path

def count_output_lines(command):
    """
    Predict how many output lines a command produces
    Note: This is a heuristic based on expected behavior
    """
    # Most commands produce 1 line of output
    # Exceptions:
    # - query_ticket with results: 1 + N train lines
    # - query_order with results: 1 + N order lines
    # - query_profile: 1 line (user info or -1)
    # We can't predict query results without running, so assume 1 line default
    return 1

def extract_query_ticket_results_precise(input_file, output_file):
    """
    Extract query_ticket results by tracking command execution order
    """
    # Read all commands
    with open(input_file, 'r') as f:
        commands = [(i, line.strip()) for i, line in enumerate(f, 1) if line.strip()]

    # Read all output lines
    with open(output_file, 'r') as f:
        output_lines = [line.strip() for line in f]

    # Match commands to outputs
    query_ticket_results = []
    output_idx = 0

    for line_num, cmd in commands:
        if cmd.startswith('query_ticket'):
            # This is a query_ticket command
            # The result starts at current output_idx
            result = []

            if output_idx >= len(output_lines):
                # No more output - program may have crashed
                result.append("(no output - program terminated?)")
            else:
                first_line = output_lines[output_idx]
                result.append(first_line)
                output_idx += 1

                # If first line is a number > 0, read that many train lines
                if first_line.isdigit() and int(first_line) > 0:
                    num_trains = int(first_line)
                    for _ in range(num_trains):
                        if output_idx < len(output_lines):
                            result.append(output_lines[output_idx])
                            output_idx += 1
                        else:
                            result.append("(missing train line)")

            query_ticket_results.append((line_num, cmd, result))

        elif cmd.startswith('query_order'):
            # query_order produces variable output
            # First line is count or -1
            if output_idx < len(output_lines):
                first_line = output_lines[output_idx]
                output_idx += 1

                if first_line.isdigit() and int(first_line) > 0:
                    num_orders = int(first_line)
                    # Skip order lines
                    for _ in range(num_orders):
                        if output_idx < len(output_lines):
                            output_idx += 1

        elif cmd.startswith('buy_ticket'):
            # buy_ticket produces 1 line
            if output_idx < len(output_lines):
                output_idx += 1

        else:
            # Most other commands produce 1 line
            if output_idx < len(output_lines):
                # Check if this is a multi-line result (like query_profile)
                line = output_lines[output_idx]
                output_idx += 1

    return query_ticket_results

def main():
    workspace = Path(__file__).parent
    project_root = workspace.parent.parent.parent
    data_dir = project_root / "data" / "017" / "data"

    print("=" * 70)
    print("PRECISE QUERY_TICKET EXTRACTION AND COMPARISON")
    print("=" * 70)
    print()

    for suite in ['basic_3', 'basic_4']:
        print(f"\n{'=' * 70}")
        print(f"Testing {suite}")
        print(f"{'=' * 70}\n")

        # Create combined input
        suite_dir = data_dir / suite
        combined_input = workspace / f"{suite}_combined.in"

        with open(combined_input, 'w') as out:
            for i in range(1, 6):
                with open(suite_dir / f"{i}.in", 'r') as f:
                    out.write(f.read())

        expected_output = suite_dir / "out.txt"
        actual_output = project_root / f"{suite}_actual_output.txt"

        if not actual_output.exists():
            print(f"⚠️  {suite}: Actual output not found")
            continue

        print("Extracting query_ticket results...")

        expected_results = extract_query_ticket_results_precise(combined_input, expected_output)
        actual_results = extract_query_ticket_results_precise(combined_input, actual_output)

        print(f"query_ticket commands found: {len(expected_results)}")
        print()

        # Compare
        passed = 0
        failed = 0
        failures = []

        for (exp_line, exp_cmd, exp_result), (act_line, act_cmd, act_result) in zip(expected_results, actual_results):
            if exp_result == act_result:
                passed += 1
            else:
                failed += 1
                failures.append({
                    'line': exp_line,
                    'command': exp_cmd,
                    'expected': exp_result,
                    'actual': act_result
                })

        # Report
        total = len(expected_results)
        print(f"Results:")
        print(f"  Total:  {total}")
        print(f"  Passed: {passed} ({passed*100//total if total > 0 else 0}%)")
        print(f"  Failed: {failed}")
        print()

        if failures:
            print(f"First 5 failures:")
            for f in failures[:5]:
                print(f"\n  Line {f['line']}: {f['command']}")
                print(f"    Expected: {f['expected'][:3]}")
                print(f"    Actual:   {f['actual'][:3]}")

if __name__ == "__main__":
    main()
