#!/bin/bash
# Detailed failure analysis for basic_3 sequential tests

echo "Detailed Failure Analysis - Basic_3 Sequential Tests"
echo "======================================================================"

for i in {1..5}; do
    echo ""
    echo "Test $i Analysis:"
    echo "--------------------------------------------------------------------"
    
    expected="data/017/data/basic_3/$i.out"
    actual="workspace/workspace/sam/basic3_results/test_$i.out"
    
    # Count total lines
    total=$(wc -l < "$expected")
    
    # Count refund_ticket failures (0 expected but -1 actual)
    refund_fails=$(diff "$expected" "$actual" | grep -A1 "^< 0$" | grep -c "^> -1$")
    
    # Count query_order lines in expected
    query_order_lines=$(grep -c "^\[.*\]" "$expected" || echo "0")
    
    # Count differences in query_order lines (lines starting with [)
    query_order_diffs=$(diff "$expected" "$actual" | grep "^[<>]" | grep -c "^\[" || echo "0")
    
    echo "  Total lines: $total"
    echo "  refund_ticket failures (0 -> -1): $refund_fails"
    echo "  query_order lines in expected: $query_order_lines"
    echo "  query_order differences: $query_order_diffs"
    
    # Total failures
    total_fails=$((total - $(diff "$expected" "$actual" | grep -c "^[<>]") / 2))
    echo "  Pass rate: $(echo "scale=2; $total_fails * 100 / $total" | bc)%"
done

echo ""
echo "======================================================================"
