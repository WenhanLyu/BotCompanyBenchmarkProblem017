#!/bin/bash
# Sequential Basic_3 Test Runner - Issue #39
# Tests all 5 basic_3 tests with persistence after Alex's fix

cd /Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017

# Clean up old data files to start fresh
echo "Cleaning old data files..."
rm -f *.dat

# Create output directory
mkdir -p workspace/workspace/sam/basic3_results

# Run tests sequentially with persistence
for i in {1..5}; do
    echo "========================================"
    echo "Running Test $i..."
    echo "========================================"

    # Run test and save output
    ./code < data/017/data/basic_3/$i.in > workspace/workspace/sam/basic3_results/test_$i.out 2>&1

    # Compare with expected output
    echo "Comparing Test $i output..."
    diff -u data/017/data/basic_3/$i.out workspace/workspace/sam/basic3_results/test_$i.out > workspace/workspace/sam/basic3_results/test_${i}_diff.txt

    # Calculate pass rate for this test
    total_lines=$(wc -l < data/017/data/basic_3/$i.out)
    diff_lines=$(diff data/017/data/basic_3/$i.out workspace/workspace/sam/basic3_results/test_$i.out | grep -c "^[<>]" || echo "0")
    matching_lines=$((total_lines - diff_lines / 2))

    if [ $total_lines -gt 0 ]; then
        pass_rate=$(echo "scale=2; $matching_lines * 100 / $total_lines" | bc)
    else
        pass_rate="0"
    fi

    echo "Test $i: $matching_lines / $total_lines lines match ($pass_rate%)"
    echo "$matching_lines / $total_lines ($pass_rate%)" > workspace/workspace/sam/basic3_results/test_${i}_rate.txt
done

echo "========================================"
echo "Sequential Test Run Complete"
echo "========================================"
