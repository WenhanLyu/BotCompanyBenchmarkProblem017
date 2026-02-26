#!/bin/bash

# Test runner for basic_3 and basic_4
# Runs each test file and compares with expected output

test_suite() {
    local suite_name=$1
    local data_dir="./data/017/data/${suite_name}"
    local expected_output="${data_dir}/out.txt"

    echo "================================"
    echo "Testing ${suite_name}"
    echo "================================"

    # Concatenate all test files in order
    cat "${data_dir}/1.in" "${data_dir}/2.in" "${data_dir}/3.in" "${data_dir}/4.in" "${data_dir}/5.in" > temp_input.txt

    # Run the program
    timeout 300 ./code < temp_input.txt > temp_output.txt 2>&1

    # Compare outputs
    if diff -q temp_output.txt "${expected_output}" > /dev/null 2>&1; then
        echo "✅ ${suite_name}: PASS - All outputs match!"
    else
        echo "❌ ${suite_name}: FAIL - Outputs differ"
        echo ""
        echo "First 50 lines of differences:"
        diff -u "${expected_output}" temp_output.txt | head -50
        echo ""
        echo "Sample differences (first 20 lines):"
        diff "${expected_output}" temp_output.txt | head -20
    fi

    # Save output for reference
    cp temp_output.txt "${suite_name}_actual_output.txt"

    echo ""
}

# Clean up any existing data files
rm -f *.dat

# Run tests
test_suite "basic_3"
rm -f *.dat

test_suite "basic_4"
rm -f *.dat

# Clean up temp files
rm -f temp_input.txt temp_output.txt

echo "Test outputs saved as basic_3_actual_output.txt and basic_4_actual_output.txt"
