#!/bin/bash

# Comprehensive query_ticket test script for basic_3 and basic_4

WORKSPACE="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/workspace/workspace/sam"
PROJECT_ROOT="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017"
DATA_DIR="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/data/017/data"

cd "$PROJECT_ROOT"

# Clean data files
rm -f *.dat

echo "========================================="
echo "COMPREHENSIVE QUERY_TICKET TEST SUITE"
echo "========================================="
echo ""

total_tests=0
passed_tests=0
failed_tests=0

# Test function
run_test() {
    local test_group=$1
    local test_num=$2

    total_tests=$((total_tests + 1))

    input_file="${DATA_DIR}/${test_group}/${test_num}.in"
    expected_file="${DATA_DIR}/${test_group}/${test_num}.out"
    actual_file="${WORKSPACE}/${test_group}_${test_num}_actual.txt"
    query_actual="${WORKSPACE}/${test_group}_${test_num}_query_actual.txt"
    query_expected="${WORKSPACE}/${test_group}_${test_num}_query_expected.txt"

    echo "Testing ${test_group}/${test_num}..."

    # Run test
    ./code < "$input_file" > "$actual_file" 2>&1

    # Extract query_ticket lines from actual output
    grep "^query_ticket" "$input_file" -A 0 > /tmp/query_commands.txt

    # Extract query_ticket results from actual output
    # Query results are the lines immediately after the commands in the output
    awk '/^[A-Za-z0-9_-]+ [0-9]+ [0-9]+$|^0$/ {print}' "$actual_file" > "$query_actual"

    # Extract query_ticket results from expected output
    awk '/^[A-Za-z0-9_-]+ [0-9]+ [0-9]+$|^0$/ {print}' "$expected_file" > "$query_expected"

    # Compare
    if diff -q "$query_actual" "$query_expected" > /dev/null 2>&1; then
        echo "✅ PASS: ${test_group}/${test_num}"
        passed_tests=$((passed_tests + 1))
    else
        echo "❌ FAIL: ${test_group}/${test_num}"
        failed_tests=$((failed_tests + 1))

        # Show first 5 differences
        echo "   First 5 differences:"
        diff "$query_expected" "$query_actual" | head -20 | sed 's/^/   /'
    fi
    echo ""
}

# Test basic_3 (1-5)
echo "========================================="
echo "TESTING basic_3 (Query Ticket Focus)"
echo "========================================="
echo ""

for i in 1 2 3 4 5; do
    rm -f *.dat
    run_test "basic_3" "$i"
done

# Test basic_4 (1-5)
echo "========================================="
echo "TESTING basic_4 (Comprehensive)"
echo "========================================="
echo ""

for i in 1 2 3 4 5; do
    rm -f *.dat
    run_test "basic_4" "$i"
done

# Summary
echo "========================================="
echo "TEST SUMMARY"
echo "========================================="
echo "Total Tests:  $total_tests"
echo "Passed:       $passed_tests"
echo "Failed:       $failed_tests"
echo "Success Rate: $(( passed_tests * 100 / total_tests ))%"
echo "========================================="

if [ $failed_tests -eq 0 ]; then
    echo "✅ ALL TESTS PASSED!"
    exit 0
else
    echo "❌ SOME TESTS FAILED"
    exit 1
fi
