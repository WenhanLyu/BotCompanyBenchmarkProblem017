#!/bin/bash

# Lines where output differs (from diff -n output)
failing_lines="280 300 436 700 931 946 1172 1302 1406"

echo "=== Identifying Failing Commands ==="
for line in $failing_lines; do
    echo ""
    echo "Line $line:"
    echo "  Command: $(sed -n "${line}p" data/017/data/basic_3/1.in)"
    echo "  Expected: $(sed -n "${line}p" data/017/data/basic_3/1.out)"
    echo "  Got: $(sed -n "${line}p" /tmp/actual.txt)"
done
