# Alex - Cycle Notes

## Task Completed
Fixed query_order command in main.cpp (lines 1107-1214)

## Changes Made

1. **Added required headers**:
   - `#include <vector>` for storing orders
   - `#include <algorithm>` for std::reverse

2. **Fixed output format**:
   - Changed from: `[timestamp] [trainID] [from] [to] [status] [price] [count]`
   - Changed to: `[<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>`
   - STATUS now shows [success]/[pending]/[refunded] in brackets
   - Times are calculated from train schedule and formatted as mm-dd hh:mm

3. **Fixed sort order**:
   - Collect all orders into a vector
   - Reverse the vector to output newest-to-oldest (instead of oldest-to-newest)

4. **Price calculation**:
   - Output single ticket price (total_price / ticket_count) instead of total_price

## Implementation Details

- Used DateTime structure with addMinutes() to calculate leaving and arriving times
- Followed the same pattern as query_ticket (lines 758-829) for time calculations
- Used Date::format() and Time::format() methods to output times in mm-dd hh:mm format

## Testing

Tested with basic_3 test case:
- Test 1: **ALL 29 query_order outputs match perfectly**
- Tests 2-5: Have failures in other commands (not query_order related) that prevent reaching query_order commands

## Files Modified

- main.cpp (lines 1-10 for headers, lines 1107-1214 for query_order function)
