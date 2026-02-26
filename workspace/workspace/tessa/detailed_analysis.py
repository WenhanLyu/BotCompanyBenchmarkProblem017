#!/usr/bin/env python3

def analyze_failures_with_context(test_num):
    input_file = f'data/017/data/basic_3/{test_num}.in'
    expected_file = f'data/017/data/basic_3/{test_num}.out'
    actual_file = f'workspace/workspace/tessa/test_{test_num}_actual.out'
    
    with open(input_file, 'r') as f:
        commands = [line.strip() for line in f.readlines()]
    
    with open(expected_file, 'r') as f:
        expected = [line.strip() for line in f.readlines()]
    
    with open(actual_file, 'r') as f:
        actual = [line.strip() for line in f.readlines()]
    
    # Find mismatches
    failures = []
    cmd_idx = 0
    exp_idx = 0
    act_idx = 0
    
    while exp_idx < len(expected) and act_idx < len(actual):
        if expected[exp_idx] == actual[act_idx]:
            exp_idx += 1
            act_idx += 1
            cmd_idx += 1
        else:
            # Found a mismatch
            if cmd_idx < len(commands):
                failures.append({
                    'cmd': commands[cmd_idx],
                    'expected': expected[exp_idx] if exp_idx < len(expected) else 'EOF',
                    'actual': actual[act_idx] if act_idx < len(actual) else 'EOF'
                })
            
            # Try to align
            if exp_idx + 1 < len(expected) and expected[exp_idx + 1] == actual[act_idx]:
                exp_idx += 1
            elif act_idx + 1 < len(actual) and expected[exp_idx] == actual[act_idx + 1]:
                act_idx += 1
            else:
                exp_idx += 1
                act_idx += 1
            
            cmd_idx += 1
            
            if len(failures) >= 20:  # Limit to first 20 failures
                break
    
    return failures

# Analyze all tests
for test_num in range(1, 6):
    print(f"\n=== Test {test_num} - First 10 Failures ===")
    failures = analyze_failures_with_context(test_num)
    
    for i, failure in enumerate(failures[:10]):
        cmd_type = failure['cmd'].split()[0] if failure['cmd'] else 'unknown'
        print(f"\n{i+1}. Command: {cmd_type}")
        print(f"   Full: {failure['cmd'][:80]}")
        print(f"   Expected: {failure['expected']}")
        print(f"   Actual:   {failure['actual']}")

