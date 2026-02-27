#!/bin/bash

# Script to run basic_3 tests sequentially with state persistence
# IMPORTANT: .dat files are NOT cleaned between tests - state persists!

WORKSPACE_DIR="workspace/workspace/iris"
DATA_DIR="./data/017/data/basic_3"

# Initialize summary
echo "========================================" > "${WORKSPACE_DIR}/test_results_v2.txt"
echo "Basic_3 Tests - Sequential Verification" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "M3.2.1 Post-Fix Verification" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "WITH STATE PERSISTENCE" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "========================================" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "" >> "${WORKSPACE_DIR}/test_results_v2.txt"

# Clean .dat files ONCE at the start
echo "Cleaning .dat files..."
rm -f *.dat 2>/dev/null || true

total_pass=0
total_lines=0

# Run each test sequentially WITHOUT cleaning between tests
for i in 1 2 3 4 5; do
    echo "Running Test ${i}..."

    # Run test (NO cleaning between tests!)
    ./code < "${DATA_DIR}/${i}.in" > "${WORKSPACE_DIR}/test_${i}_persist_actual.txt" 2>&1

    # Compare with expected output
    expected="${DATA_DIR}/${i}.out"
    actual="${WORKSPACE_DIR}/test_${i}_persist_actual.txt"

    # Count total lines in expected output
    expected_lines=$(wc -l < "${expected}")

    # Count matching lines
    matching_lines=0

    # Line-by-line comparison
    while IFS= read -r expected_line && IFS= read -r actual_line <&3; do
        if [ "$expected_line" = "$actual_line" ]; then
            ((matching_lines++))
        fi
    done < "${expected}" 3< "${actual}"

    # Handle case where files have different number of lines
    actual_lines=$(wc -l < "${actual}")
    if [ "$actual_lines" -lt "$expected_lines" ]; then
        total_test_lines=$expected_lines
    else
        total_test_lines=$actual_lines
        # If actual has more lines, those are mismatches
        if [ "$actual_lines" -gt "$expected_lines" ]; then
            expected_lines=$actual_lines
        fi
    fi

    # Calculate pass rate
    pass_rate=$(echo "scale=2; $matching_lines * 100 / $expected_lines" | bc)

    # Update totals
    total_pass=$((total_pass + matching_lines))
    total_lines=$((total_lines + expected_lines))

    # Report
    echo "Test ${i}: ${matching_lines}/${expected_lines} lines match (${pass_rate}%)" >> "${WORKSPACE_DIR}/test_results_v2.txt"
    echo "Test ${i}: ${pass_rate}%"

    # Save diff for analysis
    diff -u "${expected}" "${actual}" > "${WORKSPACE_DIR}/test_${i}_persist_diff.txt" 2>&1
done

# Calculate overall pass rate
overall_pass_rate=$(echo "scale=2; $total_pass * 100 / $total_lines" | bc)

echo "" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "========================================" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "Overall: ${total_pass}/${total_lines} lines match (${overall_pass_rate}%)" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "========================================" >> "${WORKSPACE_DIR}/test_results_v2.txt"

# Check success criteria
echo "" >> "${WORKSPACE_DIR}/test_results_v2.txt"
echo "Success Criteria:" >> "${WORKSPACE_DIR}/test_results_v2.txt"

# Get Test 1 pass rate
test1_pass_rate=$(head -n 7 "${WORKSPACE_DIR}/test_results_v2.txt" | tail -n 1 | grep -oE '[0-9]+\.[0-9]+%' | grep -oE '[0-9]+\.[0-9]+')

if (( $(echo "$test1_pass_rate >= 98.8" | bc -l) )); then
    echo "✅ Test 1: ${test1_pass_rate}% >= 98.8% (PASS)" >> "${WORKSPACE_DIR}/test_results_v2.txt"
else
    echo "❌ Test 1: ${test1_pass_rate}% < 98.8% (FAIL)" >> "${WORKSPACE_DIR}/test_results_v2.txt"
fi

if (( $(echo "$overall_pass_rate >= 95.0" | bc -l) )); then
    echo "✅ Overall: ${overall_pass_rate}% >= 95.0% (PASS)" >> "${WORKSPACE_DIR}/test_results_v2.txt"
else
    echo "❌ Overall: ${overall_pass_rate}% < 95.0% (FAIL)" >> "${WORKSPACE_DIR}/test_results_v2.txt"
fi

echo ""
echo "Overall: ${overall_pass_rate}%"
echo ""
echo "Results saved to ${WORKSPACE_DIR}/test_results_v2.txt"

# Display the results
cat "${WORKSPACE_DIR}/test_results_v2.txt"
