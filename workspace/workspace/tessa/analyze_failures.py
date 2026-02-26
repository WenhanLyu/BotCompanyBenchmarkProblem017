#!/usr/bin/env python3
import sys

def analyze_diff(diff_file):
    with open(diff_file, 'r') as f:
        lines = f.readlines()
    
    # Count different types of failures
    extra_minus_one = 0  # Lines with -1 instead of 0
    wrong_values = 0  # Lines with different values
    extra_lines = 0  # Extra lines in actual output
    missing_lines = 0  # Missing lines in actual output
    wrong_user_data = 0  # Wrong user information
    
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        
        if line.startswith('-') and not line.startswith('---'):
            # Expected line that's different
            if i+1 < len(lines) and lines[i+1].startswith('+'):
                # Replacement
                expected = line[1:].strip()
                actual = lines[i+1][1:].strip()
                
                if expected == '0' and actual == '-1':
                    extra_minus_one += 1
                elif ' ' in expected and ' ' in actual:  # User data
                    wrong_user_data += 1
                else:
                    wrong_values += 1
                i += 2
            else:
                missing_lines += 1
                i += 1
        elif line.startswith('+') and not line.startswith('+++'):
            # Extra line in actual
            if lines[i][1:].strip() == '-1':
                extra_minus_one += 1
            extra_lines += 1
            i += 1
        else:
            i += 1
    
    return {
        'extra_minus_one': extra_minus_one,
        'wrong_values': wrong_values,
        'extra_lines': extra_lines,
        'missing_lines': missing_lines,
        'wrong_user_data': wrong_user_data
    }

if __name__ == '__main__':
    for test_num in range(1, 6):
        diff_file = f'workspace/workspace/tessa/test_{test_num}_diff.txt'
        results = analyze_diff(diff_file)
        print(f"Test {test_num}:")
        print(f"  Extra -1 outputs: {results['extra_minus_one']}")
        print(f"  Wrong values: {results['wrong_values']}")
        print(f"  Wrong user data: {results['wrong_user_data']}")
        print(f"  Extra lines: {results['extra_lines']}")
        print(f"  Missing lines: {results['missing_lines']}")
        print()
