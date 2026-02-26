#!/bin/bash
# Check if the expected output has consistent ordering

# Extract a specific query_order output block from expected file
# Look for a count line followed by order lines

echo "Checking expected output file for Glaucus user..."

# Find the first query_order output for Glaucus
grep -n "Glaucus" data/017/data/basic_3/2.out | head -20

echo ""
echo "Checking actual output file for Glaucus user..."
grep -n "Glaucus" /tmp/test_2_fixed.out | head -20
