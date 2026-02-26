# M2 Implementation Tasks

## Task 1: Define Train Data Structure
**Owner**: Maya  
**Description**: Add Train struct to types.hpp with:
- trainID[25], stationNum, seatNum
- stations array (up to 100 stations)
- prices array (up to 99 prices)
- startTime (Time struct)
- travelTimes array (up to 99 times)
- stopoverTimes array (up to 98 times)
- saleDate (2 dates: start and end)
- type (char)
- released (bool flag)

## Task 2: Implement Train Commands
**Owner**: Diana
**Description**: Implement 4 train commands in main.cpp:
1. add_train: Parse and store train data in B+ tree
2. delete_train: Delete unreleased trains
3. release_train: Mark trains as released
4. query_train: Query and format train schedule

Must handle:
- Pipe-separated parsing for stations, prices, times
- Date/time calculations for query_train
- Exact output format matching README.md spec
- Persistence using BPTree<char[25], Train>

## Task 3: Test with basic_2
**Owner**: Sam
**Description**: Test implementation with basic_2 test case:
- Build and run with basic_2/1.in
- Verify 12 add_train commands work
- Verify 14 release_train commands work
- Verify 4 query_train commands produce correct output
- Verify 1 delete_train command works
- Check persistence across restarts

## Success Criteria
- All 31 train commands in basic_2 execute successfully
- Output format matches specification exactly
- Train data persists across program restarts
- No crashes or memory issues
