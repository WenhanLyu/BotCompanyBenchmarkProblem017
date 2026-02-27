#!/bin/bash
# M3.2.2 Independent Blind Verification
# Protocol: Clean start, run tests 1-5 sequentially without cleanup
# Target: test 1 >= 98.8%, overall >= 95%

WORKSPACE="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/workspace/workspace/sam"
CODE_DIR="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017"
DATA_DIR="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/data/017/data/basic_3"

cd "$CODE_DIR"

echo "=== M3.2.2 Independent Blind Verification ==="
echo "Date: $(date)"
echo ""

# Step 1: Clean start - remove all persistence files
echo "Step 1: Clean start - removing all .dat files"
rm -f *.dat data/*.dat
echo "Cleanup complete"
echo ""

# Function to run test and calculate pass rate
run_test() {
    local test_num=$1
    echo "=== Test $test_num ==="

    # Run test
    ./code < "$DATA_DIR/$test_num.in" > "$WORKSPACE/M3_2_2_test_${test_num}_actual.txt" 2>&1

    # Compare with expected output
    diff -u "$DATA_DIR/$test_num.out" "$WORKSPACE/M3_2_2_test_${test_num}_actual.txt" > "$WORKSPACE/M3_2_2_test_${test_num}_diff.txt"

    # Calculate pass rate
    local total_lines=$(wc -l < "$DATA_DIR/$test_num.out")
    local diff_lines=$(grep -c "^[-+]" "$WORKSPACE/M3_2_2_test_${test_num}_diff.txt" | grep -v "^[-+][-+][-+]" || echo "0")
    local matching_lines=$((total_lines - diff_lines / 2))
    local pass_rate=$(awk "BEGIN {printf \"%.2f\", ($matching_lines / $total_lines) * 100}")

    echo "Total lines: $total_lines"
    echo "Matching lines: $matching_lines"
    echo "Pass rate: $pass_rate%"

    if [ $test_num -eq 1 ]; then
        if (( $(echo "$pass_rate >= 98.8" | bc -l) )); then
            echo "✓ Test 1 PASSED (>= 98.8%)"
        else
            echo "✗ Test 1 FAILED (< 98.8%)"
        fi
    fi

    echo ""

    # Return pass rate for overall calculation
    echo "$total_lines $matching_lines"
}

# Run tests 1-5 sequentially without cleanup
total_all=0
matching_all=0

for i in 1 2 3 4 5; do
    result=$(run_test $i)
    echo "$result"

    # Extract totals for overall calculation
    last_line=$(echo "$result" | tail -1)
    total=$(echo "$last_line" | awk '{print $1}')
    matching=$(echo "$last_line" | awk '{print $2}')

    total_all=$((total_all + total))
    matching_all=$((matching_all + matching))
done

# Calculate overall pass rate
overall_pass_rate=$(awk "BEGIN {printf \"%.2f\", ($matching_all / $total_all) * 100}")

echo "=== OVERALL RESULTS ==="
echo "Total lines across all tests: $total_all"
echo "Matching lines: $matching_all"
echo "Overall pass rate: $overall_pass_rate%"
echo ""

if (( $(echo "$overall_pass_rate >= 95.0" | bc -l) )); then
    echo "✓ OVERALL PASSED (>= 95%)"
else
    echo "✗ OVERALL FAILED (< 95%)"
fi

# Save summary
cat > "$WORKSPACE/M3_2_2_verification_summary.txt" <<EOF
M3.2.2 Independent Blind Verification
Date: $(date)

Overall Pass Rate: $overall_pass_rate%
Total Lines: $total_all
Matching Lines: $matching_all

Target: Test 1 >= 98.8%, Overall >= 95%
Result: $(if (( $(echo "$overall_pass_rate >= 95.0" | bc -l) )); then echo "PASSED"; else echo "FAILED"; fi)
EOF

echo ""
echo "Verification complete. Results saved to $WORKSPACE/"
