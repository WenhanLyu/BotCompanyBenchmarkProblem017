# Diana's Implementation Notes

## Completed Tasks

### Train Management Commands (Previous Cycle)

Successfully implemented all 4 train management commands in main.cpp:

1. **add_train** - Parses and stores train data
2. **delete_train** - Deletes unreleased trains
3. **release_train** - Marks trains as released
4. **query_train** - Queries train schedule by trainID and date

### query_ticket Command (Current Cycle)

Successfully implemented query_ticket command with the following features:

#### Implementation Details

1. **Added forEach method to BPTree** (bptree.hpp):
   - Iterates through all leaf nodes using next_leaf pointers
   - Enables efficient traversal of all trains in the database

2. **Implemented cmd_query_ticket** (main.cpp):
   - Parses parameters: -s (from), -t (to), -d (date), -p (sort type)
   - Searches all released trains for routes containing both stations
   - Calculates departure date from starting station based on departure date from -s
   - Validates starting date is within train's sale range
   - Computes leaving time from -s and arriving time at -t
   - Calculates cumulative price from -s to -t
   - Sorts results by time or cost as specified
   - Output format matches specification exactly

#### Key Implementation Points

- **Date Calculation**: Works backwards from departure date at station -s to find the actual starting date, accounting for travel times and stopovers
- **Leaving Time**: Includes stopover time at departure station (arrival time + stopover = leaving time)
- **Arriving Time**: Does not include stopover at destination station
- **Sorting**: Supports both time-based and cost-based sorting with trainID as tiebreaker
- **Edge Cases**: Properly handles unreleased trains, non-existent stations, dates outside sale range

#### Testing

Verified with multiple test cases:
- README example: matches expected output exactly
- Multiple trains: sorting works correctly for both time and cost
- Edge cases: returns -1 for invalid queries
- NOT_RELEASED trains are correctly excluded from results

## Key Data Structures

- **TicketResult struct**: Stores query results with trainID, stations, times, price, seats
- **BPTree forEach**: Template function for iterating all key-value pairs
- **Sorting**: Simple bubble sort (sufficient for typical number of results)

## Next Steps

- Integration testing with full test suite
- Coordinate with other agents for end-to-end testing
- May need to optimize sorting if performance becomes an issue
