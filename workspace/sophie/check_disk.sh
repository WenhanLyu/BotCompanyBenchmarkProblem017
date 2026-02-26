#!/bin/bash

check_disk() {
    local test_name=$1
    local input_file=$2

    echo "Testing: $test_name"

    # Clean and measure before
    rm -f *.dat 2>/dev/null
    disk_before=$(du -sk . | cut -f1)
    files_before=$(ls -1 | wc -l)

    # Run test (silent)
    ./code < $input_file > /dev/null 2>&1

    # Measure after
    disk_after=$(du -sk . | cut -f1)
    files_after=$(ls -1 | wc -l)
    disk_used=$((disk_after - disk_before))
    files_created=$((files_after - files_before))

    disk_used_mb=$(echo "scale=2; $disk_used / 1024" | bc)

    echo "  Disk Used: ${disk_used_mb} MiB (${disk_used} KB)"
    echo "  Files Created: $files_created"
    if [ $files_created -gt 0 ]; then
        echo "  Files:"
        ls -lh *.dat 2>/dev/null | awk '{print "    " $9, $5}'
    fi
    echo ""
}

echo "=== DISK USAGE AND FILE COUNT ANALYSIS ==="
echo ""

check_disk "basic_1" "data/017/data/basic_1/1.in"
check_disk "basic_2" "data/017/data/basic_2/1.in"
check_disk "basic_3_1" "data/017/data/basic_3/1.in"
check_disk "basic_4_1" "data/017/data/basic_4/1.in"
check_disk "basic_5_1" "data/017/data/basic_5/1.in"
check_disk "basic_6_1" "data/017/data/basic_6/1.in"
check_disk "basic_6_7" "data/017/data/basic_6/7.in"
check_disk "basic_6_10" "data/017/data/basic_6/10.in"
