#!/usr/bin/env python3
"""Extract and compare query_ticket results specifically"""

import re
import sys

def extract_query_ticket_results(input_file, output_file):
    """Extract lines corresponding to query_ticket commands"""
    with open(input_file, 'r') as f:
        input_lines = f.readlines()

    with open(output_file, 'r') as f:
        output_lines = f.readlines()

    results = []
    output_idx = 0

    for i, line in enumerate(input_lines):
        if line.startswith('query_ticket'):
            # Extract the query_ticket command
            cmd = line.strip()

            # The output for this command is at output_idx
            # It could be:
            # - "0" (no trains found)
            # - "-1" (error)
            # - A number N followed by N lines of train info
            if output_idx < len(output_lines):
                result_line = output_lines[output_idx].strip()
                output_idx += 1

                # Check if it's a count
                try:
                    count = int(result_line)
                    train_lines = []
                    if count > 0 and count != -1:
                        # Read the next 'count' lines
                        for _ in range(count):
                            if output_idx < len(output_lines):
                                train_lines.append(output_lines[output_idx].strip())
                                output_idx += 1

                    results.append({
                        'command': cmd,
                        'count': count,
                        'trains': train_lines
                    })
                except ValueError:
                    results.append({
                        'command': cmd,
                        'count': None,
                        'trains': []
                    })
        else:
            # Non-query_ticket command, advance output
            # Most commands produce 1 line of output
            if not line.startswith('#') and line.strip():
                output_idx += 1

    return results

if __name__ == '__main__':
    input_file = 'workspace/workspace/maya/query_ticket_test_30.in'
    expected_file = 'workspace/workspace/maya/query_ticket_test_30_expected.txt'
    actual_file = 'workspace/workspace/maya/query_ticket_test_30_actual.txt'

    print("Extracting expected results...")
    expected_results = extract_query_ticket_results(input_file, expected_file)

    print("Extracting actual results...")
    actual_results = extract_query_ticket_results(input_file, actual_file)

    print(f"\n# query_ticket Specific Analysis\n")
    print(f"Total query_ticket commands: {len(expected_results)}\n")

    # Compare results
    correct = 0
    wrong_price = 0
    wrong_count = 0

    for i, (exp, act) in enumerate(zip(expected_results, actual_results)):
        if exp['count'] == act['count']:
            if exp['count'] > 0:
                # Check if prices match
                exp_price = re.search(r'(\d+)$', exp['trains'][0]) if exp['trains'] else None
                act_price = re.search(r'(\d+)$', act['trains'][0]) if act['trains'] else None

                if exp_price and act_price:
                    if exp_price.group(1) == act_price.group(1):
                        correct += 1
                    else:
                        wrong_price += 1
                        if i < 5:  # Show first 5 examples
                            print(f"Wrong price #{i+1}:")
                            print(f"  Expected: {exp['trains'][0]}")
                            print(f"  Actual:   {act['trains'][0]}")
                            print()
                else:
                    correct += 1
            else:
                correct += 1
        else:
            wrong_count += 1
            if i < 5:  # Show first 5 examples
                print(f"Wrong count #{i+1}:")
                print(f"  Command: {exp['command']}")
                print(f"  Expected: {exp['count']} trains")
                print(f"  Actual:   {act['count']} trains")
                print()

    print(f"## Summary")
    print(f"- Correct results: {correct}/{len(expected_results)}")
    print(f"- Wrong prices: {wrong_price}/{len(expected_results)}")
    print(f"- Wrong counts: {wrong_count}/{len(expected_results)}")
