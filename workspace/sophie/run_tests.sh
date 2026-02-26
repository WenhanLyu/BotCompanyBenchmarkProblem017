#!/bin/bash

# Comprehensive performance testing script for basic_1 through basic_6
# Measures: time, memory, disk usage, file count

RESULTS_FILE="workspace/sophie/test_results.txt"
CODE_EXEC="./code"

echo "=== PERFORMANCE TEST RESULTS ===" > $RESULTS_FILE
echo "Test Date: $(date)" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Function to run a single test
run_test() {
    local test_name=$1
    local input_file=$2

    echo "Testing: $test_name" >> $RESULTS_FILE
    echo "Input file: $input_file" >> $RESULTS_FILE

    # Clean up any previous data files (but not the executable)
    rm -f *.dat *.db *.idx 2>/dev/null

    # Count files before test
    files_before=$(ls -1 | wc -l)

    # Check disk usage before
    disk_before=$(du -sk . | cut -f1)

    # Run the test with time measurement
    echo "Running test..." >> $RESULTS_FILE
    START_TIME=$(date +%s.%N)
    /usr/bin/time -l $CODE_EXEC < $input_file > workspace/sophie/${test_name}_output.txt 2> workspace/sophie/${test_name}_time.txt
    EXIT_CODE=$?
    END_TIME=$(date +%s.%N)

    # Calculate elapsed time
    ELAPSED=$(echo "$END_TIME - $START_TIME" | bc)

    # Count files after test
    files_after=$(ls -1 | wc -l)
    files_created=$((files_after - files_before))

    # Check disk usage after
    disk_after=$(du -sk . | cut -f1)
    disk_used=$((disk_after - disk_before))

    # Extract memory info from time output
    if [ -f workspace/sophie/${test_name}_time.txt ]; then
        max_mem=$(grep "maximum resident set size" workspace/sophie/${test_name}_time.txt | awk '{print $1}')
        real_time=$(grep "real" workspace/sophie/${test_name}_time.txt | head -1 | awk '{print $1}')
    fi

    # Record results
    echo "Exit Code: $EXIT_CODE" >> $RESULTS_FILE
    echo "Elapsed Time: ${ELAPSED} seconds" >> $RESULTS_FILE
    echo "Max Memory: ${max_mem} bytes" >> $RESULTS_FILE
    if [ ! -z "$max_mem" ]; then
        max_mem_mb=$(echo "scale=2; $max_mem / 1048576" | bc)
        echo "Max Memory: ${max_mem_mb} MiB" >> $RESULTS_FILE
    fi
    echo "Disk Used: ${disk_used} KB" >> $RESULTS_FILE
    disk_used_mb=$(echo "scale=2; $disk_used / 1024" | bc)
    echo "Disk Used: ${disk_used_mb} MiB" >> $RESULTS_FILE
    echo "Files Created: $files_created" >> $RESULTS_FILE

    # List created data files
    echo "Data files:" >> $RESULTS_FILE
    ls -lh *.dat *.db *.idx 2>/dev/null | awk '{print $9, $5}' >> $RESULTS_FILE

    echo "---" >> $RESULTS_FILE
    echo "" >> $RESULTS_FILE
}

# Test basic_1
echo "## BASIC_1 ##" >> $RESULTS_FILE
run_test "basic_1_1" "data/017/data/basic_1/1.in"

# Test basic_2
echo "## BASIC_2 ##" >> $RESULTS_FILE
run_test "basic_2_1" "data/017/data/basic_2/1.in"

# Test basic_3
echo "## BASIC_3 ##" >> $RESULTS_FILE
for i in 1 2 3 4 5; do
    if [ -f "data/017/data/basic_3/$i.in" ]; then
        run_test "basic_3_$i" "data/017/data/basic_3/$i.in"
    fi
done

# Test basic_4
echo "## BASIC_4 ##" >> $RESULTS_FILE
for i in 1 2 3 4 5; do
    if [ -f "data/017/data/basic_4/$i.in" ]; then
        run_test "basic_4_$i" "data/017/data/basic_4/$i.in"
    fi
done

# Test basic_5
echo "## BASIC_5 ##" >> $RESULTS_FILE
for i in 1 2 3 4 5 6 7 8 9 10; do
    if [ -f "data/017/data/basic_5/$i.in" ]; then
        run_test "basic_5_$i" "data/017/data/basic_5/$i.in"
    fi
done

# Test basic_6
echo "## BASIC_6 ##" >> $RESULTS_FILE
for i in 1 2 3 4 5 6 7 8 9 10; do
    if [ -f "data/017/data/basic_6/$i.in" ]; then
        run_test "basic_6_$i" "data/017/data/basic_6/$i.in"
    fi
done

echo "Testing complete. Results saved to $RESULTS_FILE"
