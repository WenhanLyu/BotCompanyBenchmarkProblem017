#!/bin/bash
# Extract test input up to first 30 query_ticket commands from basic_3/1.in

INPUT_FILE="./data/017/data/basic_3/1.in"
OUTPUT_FILE="workspace/workspace/maya/query_ticket_test_30.in"

# Find line numbers of first 30 query_ticket commands
QUERY_LINES=($(grep -n "query_ticket" "$INPUT_FILE" | head -30 | cut -d: -f1))

# Get the line number of the 30th query_ticket command
LAST_LINE=${QUERY_LINES[29]}

echo "First query_ticket at line: ${QUERY_LINES[0]}"
echo "30th query_ticket at line: $LAST_LINE"

# Extract from beginning to line after 30th query_ticket
head -n "$LAST_LINE" "$INPUT_FILE" > "$OUTPUT_FILE"

echo "Created test input: $OUTPUT_FILE"
echo "Total lines: $(wc -l < $OUTPUT_FILE)"
echo "Query_ticket commands: $(grep -c 'query_ticket' $OUTPUT_FILE)"
