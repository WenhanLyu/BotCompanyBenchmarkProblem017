#!/bin/bash
# Extract query_ticket lines from input and their corresponding output
input_file="data/017/data/basic_3/1.in"
actual_file="workspace/workspace/leo/basic_3_full_output.txt"
expected_file="data/017/data/basic_3/out.txt"

echo "Comparing query_ticket results:"
echo "================================"

# Get line numbers of query_ticket commands
grep -n "query_ticket" "$input_file" | head -10 | while IFS=: read -r line_num query; do
    echo ""
    echo "Line $line_num: $query"
    echo "Actual output line $line_num:"
    sed -n "${line_num}p" "$actual_file"
    echo "Expected output line $line_num:"
    sed -n "${line_num}p" "$expected_file"
done
