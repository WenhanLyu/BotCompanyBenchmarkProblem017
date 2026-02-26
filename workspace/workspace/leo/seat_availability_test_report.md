# Seat Availability Test Report

## Date: 2026-02-26
## Task: Fix run_basic_tests.sh and verify query_ticket seat availability

## Changes Made

### 1. Fixed run_basic_tests.sh
**Problem**: The script was only cleaning `data_*` and `seat_*` files, but there are other persistence files that need to be cleaned:
- `orders.dat`
- `seats.dat`
- `trains.dat`
- `users.dat`

**Fix**: Changed all instances of `rm -f data_* seat_*` to `rm -f *.dat` to properly clean ALL data files between test runs.

**Modified lines**:
- Line 41: `rm -f *.dat` (before all tests)
- Line 45: `rm -f *.dat` (between basic_3 and basic_4)
- Line 48: `rm -f *.dat` (after basic_4)

## Manual Test Results

### Test Setup
Created a simple manual test with:
1. Add admin user
2. Login
3. Add train G1234 with 5 stations (Beijing to Suzhou)
4. Release train for date 06-01
5. Query tickets from Beijing to Shanghai
6. Attempt to buy tickets
7. Query again to verify seat count changes

### Test Input (leo_complete_test.in)
```
add_user -c cur -u admin -p pass -n Admin -m admin@test.com -g 10
login -u admin -p pass
add_train -i G1234 -n 5 -m 100 -s Beijing|Shanghai|Nanjing|Hangzhou|Suzhou -p 100|150|200|250 -x 08:00 -t 60|60|60|60 -o 10|8|6 -d 06-01|06-30 -y A
release_train -i G1234
query_ticket -s Beijing -t Shanghai -d 06-01 -p cost
buy_ticket -u admin -i G1234 -d 06-01 -n 5 -f Beijing -t Shanghai -q false
query_ticket -s Beijing -t Shanghai -d 06-01 -p cost
buy_ticket -u admin -i G1234 -d 06-01 -n 3 -f Beijing -t Shanghai -q false
query_ticket -s Beijing -t Shanghai -d 06-01 -p cost
exit
```

### Test Output
```
0     # add_user success
0     # login success
0     # add_train success
0     # release_train success
1     # query_ticket found 1 result
G1234 Beijing 06-01 08:00 -> Shanghai 06-01 09:00 100 100
-1    # buy_ticket FAILED
1     # query_ticket found 1 result (unchanged)
G1234 Beijing 06-01 08:00 -> Shanghai 06-01 09:00 100 100
-1    # buy_ticket FAILED again
1     # query_ticket found 1 result (unchanged)
G1234 Beijing 06-01 08:00 -> Shanghai 06-01 09:00 100 100
bye
```

## Findings

### Issue Discovered: buy_ticket Returns -1 (Failure)

**Observation**: All buy_ticket commands are returning -1, indicating failure. This prevents us from testing whether query_ticket properly reflects seat availability changes.

**Possible Causes**:
1. Train definition may be incorrect (the `-o 10|8|6` parameter has 3 values for 4 segments - unclear if this is correct)
2. User may not have proper permissions or may need additional setup
3. There may be a bug in the buy_ticket implementation itself

**query_ticket Behavior**:
- Successfully returns train information: `G1234 Beijing 06-01 08:00 -> Shanghai 06-01 09:00 100 100`
- Format appears to be: `trainID from date time -> to date time price seats_available`
- Seat count remains at 100 throughout because purchases are failing

## Status

### ✅ Completed
1. Fixed run_basic_tests.sh to properly clean all *.dat files
2. Verified the script now removes all persistence files between test runs
3. Created manual test with clean state (all .dat files removed)
4. Verified query_ticket command works and returns seat information

### ⚠️ Blocked
Cannot fully verify seat availability tracking because buy_ticket is failing. Need to investigate why buy_ticket returns -1 with correct command format.

## Next Steps

1. **Investigate buy_ticket failure**: Review buy_ticket implementation or find working examples in existing test data
2. **Review train definition**: Verify the `-o` parameter format is correct for the number of stations
3. **Check user permissions**: Ensure admin user has rights to purchase tickets
4. **Alternative test**: Try using existing test data (basic_3/basic_4) to verify behavior

## Files Created
- `/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/leo_complete_test.in` - Test input
- `/Users/wenhanlyu/Documents/BotCompany/tbc_projdevbench_runs/tbc-pdb-017/leo_complete_test_output.txt` - Test output
