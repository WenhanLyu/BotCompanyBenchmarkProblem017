#!/bin/bash

# Script to run basic_3 tests sequentially with state persistence
# Clean *.dat once before test 1, then run tests 1-5 continuously

WORKSPACE_DIR="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017"
TEST_DIR="$WORKSPACE_DIR/data/017/data/basic_3"
IRIS_WORKSPACE="$WORKSPACE_DIR/workspace/workspace/iris"
EXECUTABLE="$WORKSPACE_DIR/code"

# Clean data files once before starting
echo "Cleaning *.dat files before test 1..."
rm -f "$WORKSPACE_DIR"/*.dat

# Initialize results
TOTAL_TESTS=0
TOTAL_PASSED=0

echo "Starting sequential test execution with state persistence..."
echo "============================================================"

# Run tests 1-5 sequentially
for i in 1 2 3 4 5; do
    TEST_INPUT="$TEST_DIR/$i.in"
    TEST_EXPECTED="$TEST_DIR/$i.out"
    TEST_ACTUAL="$IRIS_WORKSPACE/basic_3_${i}_actual.txt"
    TEST_DIFF="$IRIS_WORKSPACE/basic_3_${i}_diff.txt"

    echo ""
    echo "Running test $i..."

    # Run the test
    "$EXECUTABLE" < "$TEST_INPUT" > "$TEST_ACTUAL" 2>&1
    EXIT_CODE=$?

    if [ $EXIT_CODE -ne 0 ]; then
        echo "Test $i: ERROR (exit code $EXIT_CODE)"
        echo "ERROR: Exit code $EXIT_CODE" > "$TEST_DIFF"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
    else
        # Compare output
        diff "$TEST_EXPECTED" "$TEST_ACTUAL" > "$TEST_DIFF" 2>&1
        DIFF_EXIT=$?

        TOTAL_TESTS=$((TOTAL_TESTS + 1))

        if [ $DIFF_EXIT -eq 0 ]; then
            echo "Test $i: PASS"
            TOTAL_PASSED=$((TOTAL_PASSED + 1))
        else
            echo "Test $i: FAIL"
            echo "First 20 lines of diff:"
            head -20 "$TEST_DIFF"
        fi
    fi

    # Show state file sizes after each test
    echo "State after test $i:"
    ls -lh "$WORKSPACE_DIR"/*.dat 2>/dev/null | awk '{print "  " $9 ": " $5}' || echo "  No .dat files found"
done

# Calculate pass rates
echo ""
echo "============================================================"
echo "TEST RESULTS SUMMARY"
echo "============================================================"

for i in 1 2 3 4 5; do
    TEST_DIFF="$IRIS_WORKSPACE/basic_3_${i}_diff.txt"
    if [ -f "$TEST_DIFF" ]; then
        if [ ! -s "$TEST_DIFF" ]; then
            echo "Test $i: PASS"
        elif grep -q "TIMEOUT" "$TEST_DIFF"; then
            echo "Test $i: TIMEOUT"
        elif grep -q "ERROR" "$TEST_DIFF"; then
            echo "Test $i: ERROR"
        else
            echo "Test $i: FAIL"
        fi
    else
        echo "Test $i: NOT RUN"
    fi
done

echo ""
echo "Total tests run: $TOTAL_TESTS"
echo "Tests passed: $TOTAL_PASSED"
echo "Tests failed: $((TOTAL_TESTS - TOTAL_PASSED))"

if [ $TOTAL_TESTS -gt 0 ]; then
    PASS_RATE=$(awk "BEGIN {printf \"%.2f\", ($TOTAL_PASSED / $TOTAL_TESTS) * 100}")
    echo "Overall pass rate: $PASS_RATE%"
else
    echo "Overall pass rate: N/A (no tests run)"
fi

# Calculate individual test pass rates (for this run, each test runs once)
echo ""
echo "Individual test pass rates (1 run each):"
for i in 1 2 3 4 5; do
    TEST_DIFF="$IRIS_WORKSPACE/basic_3_${i}_diff.txt"
    if [ -f "$TEST_DIFF" ] && [ ! -s "$TEST_DIFF" ]; then
        echo "  Test $i: 100.00% (1/1)"
    else
        echo "  Test $i: 0.00% (0/1)"
    fi
done

echo ""
echo "============================================================"
echo "Test execution complete."
echo "Results saved in: $IRIS_WORKSPACE"
