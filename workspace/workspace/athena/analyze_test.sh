#!/bin/bash

# Clean state and run test
rm -f *.dat
./code < data/017/data/basic_3/1.in > /tmp/actual.txt 2>&1

# Compare
echo "=== Line count comparison ==="
wc -l /tmp/actual.txt data/017/data/basic_3/1.out

echo ""
echo "=== Difference count ==="
diff data/017/data/basic_3/1.out /tmp/actual.txt | grep -c "^<\|^>" || echo "0"

echo ""
echo "=== Pass rate ==="
total=$(wc -l < data/017/data/basic_3/1.out)
diff_lines=$(diff data/017/data/basic_3/1.out /tmp/actual.txt | grep -c "^<\|^>" || echo "0")
correct=$((total - diff_lines))
echo "$correct / $total = $(awk "BEGIN {printf \"%.1f%%\", 100.0*$correct/$total}")"

echo ""
echo "=== First 10 differences ==="
diff -y --suppress-common-lines data/017/data/basic_3/1.out /tmp/actual.txt | head -20
