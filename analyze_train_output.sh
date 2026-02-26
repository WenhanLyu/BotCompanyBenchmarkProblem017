#!/bin/bash
# Get line numbers for train commands
grep -n "add_train\|delete_train\|release_train\|query_train" data/017/data/basic_2/1.in | while IFS=: read -r linenum cmd; do
    # Extract command type
    cmdtype=$(echo "$cmd" | awk '{print $1}')
    # Get the result from the output file (same line number)
    result=$(sed -n "${linenum}p" basic_2_test_results.txt)
    echo "Line $linenum: $cmdtype => $result"
done
