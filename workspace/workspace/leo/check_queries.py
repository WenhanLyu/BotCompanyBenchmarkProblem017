#!/usr/bin/env python3import sys

# Read input commands
with open('../../../data/017/data/basic_3/1.in', 'r') as f:
    commands = [line.strip() for line in f if line.strip()]

# Read actual output
with open('basic_3_full_output.txt', 'r') as f:
    actual_output = [line.strip() for line in f]

# Read expected output
with open('../../../data/017/data/basic_3/out.txt', 'r') as f:
    expected_output = [line.strip() for line in f]

# Track output position
actual_pos = 0
expected_pos = 0

print("Query_ticket command comparison:")
print("=" * 80)

for cmd_num, cmd in enumerate(commands[:300], 1):
    if 'query_ticket' in cmd:
        # Get actual output
        if actual_pos < len(actual_output):
            actual_result_count = actual_output[actual_pos]
            actual_pos += 1
            
            # Get the results
            actual_results = []
            try:
                count = int(actual_result_count)
                if count > 0:
                    for i in range(count):
                        if actual_pos < len(actual_output):
                            actual_results.append(actual_output[actual_pos])
                            actual_pos += 1
            except:
                pass
        
        # Get expected output
        if expected_pos < len(expected_output):
            expected_result_count = expected_output[expected_pos]
            expected_pos += 1
            
            # Get the results
            expected_results = []
            try:
                count = int(expected_result_count)
                if count > 0:
                    for i in range(count):
                        if expected_pos < len(expected_output):
                            expected_results.append(expected_output[expected_pos])
                            expected_pos += 1
            except:
                pass
        
        # Compare
        if actual_result_count != expected_result_count:
            print(f"\nCommand {cmd_num}: {cmd}")
            print(f"  Actual:   {actual_result_count} results")
            if actual_results:
                for r in actual_results[:2]:
                    print(f"            {r}")
            print(f"  Expected: {expected_result_count} results")
            if expected_results:
                for r in expected_results[:2]:
                    print(f"            {r}")
    else:
        # Skip non-query commands - they produce varying output
        # For simplicity, just advance one line for most commands
        if actual_pos < len(actual_output):
            actual_pos += 1
        if expected_pos < len(expected_output):
            expected_pos += 1

