# Issue #54 Investigation: buy_ticket Bug Report

## Summary
Investigated the failing buy_ticket command at test 5, input line 1097 that should output "queue" but outputs price instead.

## Targeted Logging Added

Added DEBUG_ISSUE_54 flag with detailed logging for:
1. `cmd_buy_ticket`: Entry point, parameter validation, user login check, seat availability
2. `cmd_login`: Mostima login attempts and failures
3. `checkAvailableSeats`: Seat data lookup and availability calculation
4. `reserveSeats`: Seat reservation attempts

## Key Findings

### Finding #1: Login Failure (Root Cause Candidate)
**Command**: Line 812 of test 5.in
```
login -u Mostima -p RsKRWsG_QZ
```

**Debug Output**:
```
[DEBUG LOGIN] Mostima: password incorrect
```

**Analysis**:
- Mostima user was created in test 1, line 516 with password: `m9rQcRljCyv2fdygKU`
- Login attempt in test 5, line 812 uses password: `RsKRWsG_QZ`
- **Passwords do not match!**

### Finding #2: Buy_ticket Failure Due to Login State
**Command**: Line 1097 of test 5.in
```
buy_ticket -u Mostima -i puzzletheNewWorld -d 08-04 -n 1304 -f 黑龙江省尚志市 -t 四川省绵竹市 -q true
```

**Debug Output**:
```
[DEBUG] TARGET buy_ticket called!
[DEBUG TARGET] FAILED: user not logged in
```

**Actual Output**: `-1`
**Expected Output**: `queue`

**Analysis**:
- buy_ticket fails at user login check (line 1382-1392 of main.cpp)
- Returns -1 because Mostima is not in `logged_in_users` hash table
- This is a direct consequence of Finding #1 (login failure)

## Test Environment

**Test Execution Model**:
- Tests 1-5 run as **separate processes**
- User data persisted via `users.dat` (Mostima user exists from test 1)
- Login state **NOT persisted** - stored only in memory (`logged_in_users` hash table)
- Each test starts with empty login state

## Hypothesis

The reported bug (outputs price instead of "queue") may be:
1. **A test data error**: Wrong password for Mostima in test 5
2. **An intermittent bug**: Different behavior in different runs
3. **A misidentified line**: The failing command may be at a different line

## Next Steps for Cycle 2

1. **Verify test data**: Check if password mismatch is intentional or a data error
2. **Find actual failing command**: Search for buy_ticket command that outputs price when it should output "queue"
3. **Trace seat availability**: Once correct command identified, trace `checkAvailableSeats` to find why it reports seats available incorrectly
4. **Check for similar bugs**: Look for other buy_ticket commands that might have the same issue

## Debug Code Changes

### Files Modified:
- `main.cpp`: Added debug logging to `cmd_buy_ticket` and `cmd_login`
- `order_system.hpp`: Added debug logging to `checkAvailableSeats` and `reserveSeats`

### Debug Flag:
```cpp
#define DEBUG_ISSUE_54 1
```

Set to 0 to disable all debug logging.

## Artifacts Generated

- `workspace/workspace/leo/test5_final.log`: Full stderr debug log from test 5
- `workspace/workspace/leo/target_debug.log`: Targeted debug for Mostima commands
- This report: `issue_54_findings.md`
