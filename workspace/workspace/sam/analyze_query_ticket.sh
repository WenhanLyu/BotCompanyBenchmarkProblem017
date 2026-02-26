#!/bin/bash
# Analyze query_ticket price differences

echo "=== Query Ticket Price Differences in basic_3/1.in ==="
echo

# Get all lines from the diff that show train ticket info
diff workspace/workspace/sam/basic_3_1_expected.txt workspace/workspace/sam/basic_3_1_actual.txt | \
  grep "LeavesofGrass" | \
  head -20

echo
echo "=== Summary ==="
expected_prices=$(diff workspace/workspace/sam/basic_3_1_expected.txt workspace/workspace/sam/basic_3_1_actual.txt | grep "^< LeavesofGrass" | awk '{print $NF}')
actual_prices=$(diff workspace/workspace/sam/basic_3_1_expected.txt workspace/workspace/sam/basic_3_1_actual.txt | grep "^> LeavesofGrass" | awk '{print $NF}')

echo "Sample expected prices: $(echo $expected_prices | tr ' ' '\n' | head -5 | tr '\n' ' ')"
echo "Sample actual prices: $(echo $actual_prices | tr ' ' '\n' | head -5 | tr '\n' ' ')"
