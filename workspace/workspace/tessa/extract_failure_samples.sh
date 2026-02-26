#!/bin/bash

for i in 1 2 3 4 5; do
    echo "=== Test $i Sample Failures ==="
    
    # Extract first 10 different/wrong outputs
    grep -A1 -B1 '^[<>]' workspace/workspace/tessa/test_${i}_diff.txt | head -40
    
    echo ""
done
