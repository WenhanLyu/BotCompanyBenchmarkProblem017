#!/usr/bin/env python3
"""
Comprehensive query_ticket analysis tool
Extracts and compares query_ticket results from test outputs
"""

import sys
import re
from pathlib import Path
from collections import defaultdict

def extract_query_ticket_results(input_file, output_file):
    """
    Extract query_ticket commands and their corresponding results
    Returns: [(command, result_lines), ...]
    """
    # Read commands
    with open(input_file, 'r') as f:
        commands = [line.strip() for line in f if line.strip().startswith('query_ticket')]

    # Read output
    with open(output_file, 'r') as f:
        output_lines = [line.strip() for line in f]

    # Match commands with results
    results = []
    output_idx = 0

    for cmd in commands:
        # Find the corresponding output
        # query_ticket results are either:
        # - "0" (no results)
        # - Multiple lines of "TrainID SeatCount Price"
        result_lines = []

        # Skip to the right position in output (matching command execution order)
        while output_idx < len(output_lines):
            line = output_lines[output_idx]
            output_idx += 1

            # Check if this is a query_ticket result
            if line == "0":
                result_lines.append(line)
                break
            elif re.match(r'^[A-Za-z0-9_-]+ \d+ \d+$', line):
                # This looks like a train result line
                result_lines.append(line)
                # Keep reading until we hit a non-result line
                while output_idx < len(output_lines):
                    next_line = output_lines[output_idx]
                    if re.match(r'^[A-Za-z0-9_-]+ \d+ \d+$', next_line):
                        result_lines.append(next_line)
                        output_idx += 1
                    else:
                        break
                break

        results.append((cmd, result_lines))

    return results

def compare_results(expected_results, actual_results):
    """
    Compare expected and actual query_ticket results
    Returns: (total, passed, failed, details)
    """
    total = len(expected_results)
    passed = 0
    failed = 0
    details = []

    for i, ((exp_cmd, exp_lines), (act_cmd, act_lines)) in enumerate(zip(expected_results, actual_results)):
        if exp_lines == act_lines:
            passed += 1
        else:
            failed += 1
            details.append({
                'index': i + 1,
                'command': exp_cmd,
                'expected': exp_lines,
                'actual': act_lines
            })

    return total, passed, failed, details

def format_result_lines(lines):
    """Format result lines for display"""
    if not lines:
        return "  (no output)"
    return "\n".join(f"  {line}" for line in lines)

def main():
    workspace = Path(__file__).parent
    # Navigate up: sam -> workspace -> workspace -> project_root
    project_root = workspace.parent.parent.parent
    data_dir = project_root / "data" / "017" / "data"

    # Test suites
    suites = ['basic_3', 'basic_4']

    total_tests = 0
    total_passed = 0
    total_failed = 0
    all_failures = []

    print("=" * 60)
    print("COMPREHENSIVE QUERY_TICKET ANALYSIS")
    print("=" * 60)
    print()

    for suite in suites:
        print(f"\n{'=' * 60}")
        print(f"Testing {suite}")
        print(f"{'=' * 60}\n")

        # Prepare combined input file
        suite_dir = data_dir / suite
        combined_input = workspace / f"{suite}_combined.in"

        with open(combined_input, 'w') as out:
            for i in range(1, 6):
                with open(suite_dir / f"{i}.in", 'r') as f:
                    out.write(f.read())

        expected_output = suite_dir / "out.txt"
        actual_output = project_root / f"{suite}_actual_output.txt"

        # Check if actual output exists
        if not actual_output.exists():
            print(f"⚠️  {suite}: Actual output not found at {actual_output}")
            print(f"   Run the basic tests first to generate output files.")
            continue

        # Extract results
        print(f"Extracting query_ticket results...")
        expected_results = extract_query_ticket_results(combined_input, expected_output)
        actual_results = extract_query_ticket_results(combined_input, actual_output)

        print(f"Commands found: {len(expected_results)}")

        # Compare
        total, passed, failed, failures = compare_results(expected_results, actual_results)

        total_tests += total
        total_passed += passed
        total_failed += failed

        # Report
        print(f"\n{suite} Results:")
        print(f"  Total:  {total}")
        print(f"  Passed: {passed} ({passed*100//total if total > 0 else 0}%)")
        print(f"  Failed: {failed}")

        if failures:
            print(f"\n  First 3 failures:")
            for failure in failures[:3]:
                print(f"\n  [{failure['index']}] {failure['command']}")
                print(f"    Expected:")
                print(format_result_lines(failure['expected']))
                print(f"    Actual:")
                print(format_result_lines(failure['actual']))

            if len(failures) > 3:
                print(f"\n  ... and {len(failures) - 3} more failures")

            all_failures.extend([(suite, f) for f in failures])

    # Overall summary
    print(f"\n{'=' * 60}")
    print("OVERALL SUMMARY")
    print(f"{'=' * 60}")
    print(f"Total query_ticket commands tested: {total_tests}")
    print(f"Passed: {total_passed} ({total_passed*100//total_tests if total_tests > 0 else 0}%)")
    print(f"Failed: {total_failed}")
    print(f"{'=' * 60}")

    if total_failed == 0:
        print("✅ ALL TESTS PASSED!")
        return 0
    else:
        print(f"❌ {total_failed} TESTS FAILED")
        return 1

if __name__ == "__main__":
    sys.exit(main())
