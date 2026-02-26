#!/bin/bash

echo "=== PERFORMANCE TEST RESULTS ==="
echo "Test Date: $(date)"
echo ""
echo "RESOURCE LIMITS:"
echo "- Time Limit: 5000-40000 ms"
echo "- Memory Limit: 42-47 MiB"
echo "- Disk Usage: 366-488 MiB"
echo "- File Count: 20 files max"
echo ""

extract_stats() {
    local name=$1
    local file=$2

    if [ ! -f "$file" ]; then
        echo "ERROR: $file not found"
        return
    fi

    real_time=$(grep "real" $file | awk '{print $1}')
    max_mem=$(grep "maximum resident set size" $file | awk '{print $1}')
    peak_mem=$(grep "peak memory footprint" $file | awk '{print $1}')

    # Convert to MiB
    max_mem_mb=$(echo "scale=2; $max_mem / 1048576" | bc)
    peak_mem_mb=$(echo "scale=2; $peak_mem / 1048576" | bc)

    # Convert time to ms
    real_time_ms=$(echo "scale=2; $real_time * 1000" | bc)

    echo "$name:"
    echo "  Time: ${real_time_ms} ms (${real_time} s)"
    echo "  Memory (resident): ${max_mem_mb} MiB (${max_mem} bytes)"
    echo "  Memory (peak): ${peak_mem_mb} MiB (${peak_mem} bytes)"
    echo ""
}

echo "## BASIC_1 ##"
extract_stats "basic_1_1" "workspace/sophie/basic_1_time.txt"

echo "## BASIC_2 ##"
extract_stats "basic_2_1" "workspace/sophie/basic_2_time.txt"

echo "## BASIC_3 ##"
extract_stats "basic_3_1" "workspace/sophie/basic_3_1_time.txt"

echo "## BASIC_4 ##"
extract_stats "basic_4_1" "workspace/sophie/basic_4_1_time.txt"

echo "## BASIC_5 ##"
extract_stats "basic_5_1" "workspace/sophie/basic_5_1_time.txt"

echo "## BASIC_6 ##"
extract_stats "basic_6_1" "workspace/sophie/basic_6_1_time.txt"
extract_stats "basic_6_7" "workspace/sophie/basic_6_7_time.txt"
extract_stats "basic_6_10" "workspace/sophie/basic_6_10_time.txt"
