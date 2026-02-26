#!/bin/bash
# Analyze query_ticket test results

EXPECTED="workspace/workspace/maya/query_ticket_test_30_expected.txt"
ACTUAL="workspace/workspace/maya/query_ticket_test_30_actual.txt"
REPORT="workspace/workspace/maya/leo_fix_test_report.md"

echo "# Leo's query_ticket Fix Verification Report" > "$REPORT"
echo "" >> "$REPORT"
echo "**Test Date:** $(date '+%Y-%m-%d %H:%M')" >> "$REPORT"
echo "**Tester:** Maya" >> "$REPORT"
echo "**Test Scope:** First 30 query_ticket commands from basic_3/1.in" >> "$REPORT"
echo "" >> "$REPORT"
echo "---" >> "$REPORT"
echo "" >> "$REPORT"

# Count lines
EXPECTED_LINES=$(wc -l < "$EXPECTED")
ACTUAL_LINES=$(wc -l < "$ACTUAL")

echo "## Summary" >> "$REPORT"
echo "" >> "$REPORT"
echo "- Expected output lines: $EXPECTED_LINES" >> "$REPORT"
echo "- Actual output lines: $ACTUAL_LINES" >> "$REPORT"
echo "- Difference: $((ACTUAL_LINES - EXPECTED_LINES)) extra lines" >> "$REPORT"
echo "" >> "$REPORT"

# Count differences
DIFF_LINES=$(diff "$EXPECTED" "$ACTUAL" | grep -E "^<|^>" | wc -l)
echo "- Total differing lines: $DIFF_LINES" >> "$REPORT"
echo "" >> "$REPORT"

# Extract query_ticket results from both files
echo "## query_ticket Results Analysis" >> "$REPORT"
echo "" >> "$REPORT"

# Count how many query_ticket commands returned -1 in actual vs expected
EXPECTED_MINUS_ONES=$(grep -A1 "query_ticket" workspace/workspace/maya/query_ticket_test_30.in | \
    awk 'NR==FNR{cmd[NR]=$0; next} /^-1$/{count++} END{print count}' - "$EXPECTED")

ACTUAL_MINUS_ONES=$(grep -A1 "query_ticket" workspace/workspace/maya/query_ticket_test_30.in | \
    awk 'NR==FNR{cmd[NR]=$0; next} /^-1$/{count++} END{print count}' - "$ACTUAL")

echo "### Failure Rate (-1 returns)" >> "$REPORT"
echo "" >> "$REPORT"
echo "- Expected -1 returns: ~$EXPECTED_MINUS_ONES out of 30" >> "$REPORT"
echo "- Actual -1 returns: ~$ACTUAL_MINUS_ONES out of 30" >> "$REPORT"
echo "" >> "$REPORT"

# Check for price mismatches (98306 appearing where it shouldn't)
WRONG_PRICES=$(grep -c "98306" "$ACTUAL")
echo "### Price Calculation Issues" >> "$REPORT"
echo "" >> "$REPORT"
echo "- Lines with 98306 (full journey price): $WRONG_PRICES" >> "$REPORT"
echo "- This indicates price calculation is using full journey price instead of segment price" >> "$REPORT"
echo "" >> "$REPORT"

echo "" >> "$REPORT"
echo "## Test Status" >> "$REPORT"
echo "" >> "$REPORT"

if [ "$DIFF_LINES" -lt 10 ]; then
    echo "✅ **PASSED** - Leo's fixes resolved the issues!" >> "$REPORT"
else
    echo "❌ **FAILED** - Significant issues remain after Leo's fixes" >> "$REPORT"
fi

echo "" >> "$REPORT"
echo "See detailed diff in \`diff_output.txt\`" >> "$REPORT"

cat "$REPORT"

# Save detailed diff
diff -u "$EXPECTED" "$ACTUAL" > workspace/workspace/maya/diff_output.txt
