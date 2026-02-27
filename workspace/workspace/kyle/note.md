# Kyle - Cycle 1 Complete

## Status
Investigated Bug #2 using targeted debug logging. Root cause partially identified but fix not yet implemented.

## Key Findings

1. **The Bug**: Input line 1097 of test 5 outputs "15169000" (price) instead of "queue" at output line 1559

2. **Root Cause - Login Failure**:
   - Test 3: Mostima's password changed to "u9iPl9k2seP8_niK-4"
   - Test 5 line 812: Mostima tries login with wrong password "RsKRWsG_QZ"
   - Login FAILS - Mostima not logged in
   - Debug confirms: `[BUG2] is_logged_in pointer: NULL`

3. **The Contradiction**:
   - Debug shows buy_ticket correctly returns -1 (user not logged in)
   - But output line 1559 shows "15169000" (a price), not -1
   - This means either:
     a. Line mapping is off (modify_profile output interfering?)
     b. Different command producing line 1559
     c. There's a hidden bug in the output logic

## Debug Code Added
- Added DEBUG_ISSUE_54 logging in main.cpp and order_system.hpp
- Tracks exact command with all parameters (username, trainID, date, num)
- Logs login state, seat availability, and decision path

## Next Steps for Cycle 2

1. **Verify Line Mapping**:
   - Add output line counter to precisely match input commands to output lines
   - Determine which input command ACTUALLY produces output line 1559

2. **Investigate modify_profile Impact**:
   - Input 1096: `modify_profile -c Jessica -u Hung -p zVLQ8raGi$wdC6617j`
   - This might change Jessica's username, affecting input 1098
   - Check if this creates unexpected output patterns

3. **Fix Options**:
   - If login is the issue: Fix password or login logic
   - If line mapping is wrong: Identify actual buggy command
   - If it's a different bug: Trace through seat availability logic

## Files Created
- `bug2_analysis.md`: Detailed analysis of findings
- `debug_log_v2.txt`: Debug output from sequential run
- `test5_output_v2.txt`: Actual output from test 5

## Command to Reproduce
```bash
rm -f *.dat
./ticket_system < ./data/017/data/basic_3/1.in > /dev/null 2>&1
./ticket_system < ./data/017/data/basic_3/2.in > /dev/null 2>&1
./ticket_system < ./data/017/data/basic_3/3.in > /dev/null 2>&1
./ticket_system < ./data/017/data/basic_3/4.in > /dev/null 2>&1
./ticket_system < ./data/017/data/basic_3/5.in > test5_out.txt 2> debug.txt
sed -n '1559p' test5_out.txt  # Should be "queue", actually "15169000"
```
