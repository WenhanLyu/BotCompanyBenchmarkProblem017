#!/bin/bash

# Script to run all basic_* tests and measure performance
# Sophie - Performance and Constraints Analyst

WORKSPACE="/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017"
DATADIR="$WORKSPACE/data/017/data"
CODE="$WORKSPACE/code"
OUTPUT_DIR="$WORKSPACE/workspace/workspace/sophie"

cd "$WORKSPACE"

echo "==================================================================="
echo "RUNNING ALL BASIC_* TESTS"
echo "Date: $(date)"
echo "==================================================================="
echo ""

# Function to run a single test
run_test() {
    local test_category=$1
    local test_num=$2
    local input_file="$DATADIR/${test_category}/${test_num}.in"
    local answer_file="$DATADIR/${test_category}/${test_num}.ans"
    local output_file="$OUTPUT_DIR/${test_category}_${test_num}_output.txt"

    if [ ! -f "$input_file" ]; then
        echo "  [SKIP] Test file not found: $input_file"
        return
    fi

    echo "  Running ${test_category}/${test_num}..."

    # Clean up any existing data files
    rm -f *.dat 2>/dev/null

    # Run the test with time and memory measurement
    /usr/bin/time -l "$CODE" < "$input_file" > "$output_file" 2>&1
    local exit_code=$?

    # Check if output file was created
    if [ ! -f "$output_file" ]; then
        echo "  [ERROR] No output generated"
        return
    fi

    # Compare with expected answer if available
    if [ -f "$answer_file" ]; then
        if diff -q "$output_file" "$answer_file" > /dev/null 2>&1; then
            echo "  [PASS] Output matches expected answer"
        else
            echo "  [FAIL] Output differs from expected answer"
            # Show first few differences
            diff "$output_file" "$answer_file" | head -20
        fi
    else
        echo "  [INFO] No answer file to compare"
    fi

    # Extract performance metrics from stderr (if captured in output)
    echo "  Exit code: $exit_code"
    echo ""
}

# Test each category
for category in basic_1 basic_2 basic_3 basic_4 basic_5 basic_6; do
    echo "==================================================================="
    echo "Testing: $category"
    echo "==================================================================="

    # Find all test cases in this category
    test_files=$(ls "$DATADIR/$category/"*.in 2>/dev/null | sed 's/.*\///g' | sed 's/\.in//g' | sort -n)

    if [ -z "$test_files" ]; then
        echo "  [SKIP] No test files found in $category"
        echo ""
        continue
    fi

    for test_num in $test_files; do
        run_test "$category" "$test_num"
    done

    echo ""
done

echo "==================================================================="
echo "ALL TESTS COMPLETE"
echo "==================================================================="
echo ""
echo "Output files saved to: $OUTPUT_DIR"
