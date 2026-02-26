#!/usr/bin/env python3

def analyze_commands(input_file, diff_file):
    # Read the input commands
    with open(input_file, 'r') as f:
        commands = [line.strip() for line in f.readlines()]
    
    # Read the diff to identify failing lines
    with open(diff_file, 'r') as f:
        diff_lines = f.readlines()
    
    # Track which commands have failures
    failures_by_cmd = {}
    
    # Parse diff to get line numbers that differ
    current_line = 0
    for line in diff_lines:
        if line.startswith('@@'):
            # Extract line range from diff header
            parts = line.split()
            if len(parts) >= 3:
                # Parse the expected line range (e.g., @@ -1,441 +1,447 @@)
                expected_range = parts[1].split(',')
                if len(expected_range) == 2:
                    current_line = int(expected_range[0][1:])
        elif line.startswith('-') and not line.startswith('---'):
            # A line that differs (expected but not actual)
            current_line += 1
        elif line.startswith('+') and not line.startswith('+++'):
            # Extra line in actual (or replacement)
            pass
        elif not line.startswith('\\'):
            # Context line
            current_line += 1
    
    # Count commands
    cmd_counts = {}
    for cmd in commands:
        cmd_type = cmd.split()[0] if cmd else 'empty'
        cmd_counts[cmd_type] = cmd_counts.get(cmd_type, 0) + 1
    
    return cmd_counts

if __name__ == '__main__':
    for test_num in range(1, 6):
        input_file = f'data/017/data/basic_3/{test_num}.in'
        print(f"\nTest {test_num} command breakdown:")
        cmd_counts = analyze_commands(input_file, f'workspace/workspace/tessa/test_{test_num}_diff.txt')
        for cmd, count in sorted(cmd_counts.items(), key=lambda x: -x[1]):
            print(f"  {cmd}: {count}")
