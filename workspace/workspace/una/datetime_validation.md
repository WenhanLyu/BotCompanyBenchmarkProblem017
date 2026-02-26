# DateTime Calculation Validation Report

## Overview
This report validates the accuracy of date/time calculations in types.hpp for the Date, Time, and DateTime structures.

## Code Review

### 1. Date Parsing (lines 111-119)
```cpp
explicit Date(const char* str) {
    if (str && strlen(str) >= 5) {
        month = (str[0] - '0') * 10 + (str[1] - '0');
        day = (str[3] - '0') * 10 + (str[4] - '0');
    }
}
```
**Analysis**: Correct parsing of "mm-dd" format.
- Extracts month from positions 0-1
- Extracts day from positions 3-4 (skipping '-' at position 2)

### 2. Time Parsing (lines 173-181)
```cpp
explicit Time(const char* str) {
    if (str && strlen(str) >= 5) {
        hour = (str[0] - '0') * 10 + (str[1] - '0');
        minute = (str[3] - '0') * 10 + (str[4] - '0');
    }
}
```
**Analysis**: Correct parsing of "hr:mi" format.
- Extracts hour from positions 0-1
- Extracts minute from positions 3-4 (skipping ':' at position 2)

### 3. Date::toDayNumber() (lines 122-129)
```cpp
int toDayNumber() const {
    const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int total = day;
    for (int i = 1; i < month; i++) {
        total += days_in_month[i];
    }
    return total;
}
```
**Analysis**: Converts date to day number from start of year.
- For June-August period: June=30 days, July=31 days, August=31 days
- Example: 06-01 = 1 + 31+28+31+30+31 = 152 days
- Example: 07-01 = 1 + 31+28+31+30+31+30 = 182 days
- Example: 08-01 = 1 + 31+28+31+30+31+30+31 = 213 days

**VERIFICATION**:
- June has 30 days ✓ (days_in_month[6] = 30)
- July has 31 days ✓ (days_in_month[7] = 31)
- August has 31 days ✓ (days_in_month[8] = 31)

### 4. Time::addMinutes() (lines 189-196)
```cpp
int addMinutes(int mins) {
    int total = toMinutes() + mins;
    int days = total / (24 * 60);
    total %= (24 * 60);
    hour = total / 60;
    minute = total % 60;
    return days;
}
```
**Analysis**: Adds minutes and handles 24-hour wraparound.
- Converts current time to minutes since midnight
- Adds new minutes
- Calculates days that passed (1440 minutes = 1 day)
- Updates hour and minute with remainder
- Returns number of days for date adjustment

**Test**: 23:00 + 90 minutes
- total = 23*60 + 0 + 90 = 1380 + 90 = 1470
- days = 1470 / 1440 = 1
- total = 1470 % 1440 = 30
- hour = 30 / 60 = 0
- minute = 30 % 60 = 30
- Result: 00:30 next day ✓

### 5. DateTime::addMinutes() (lines 237-257)
```cpp
void addMinutes(int mins) {
    int days = time.addMinutes(mins);

    // Add days to date
    while (days > 0) {
        const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int days_remaining = days_in_month[date.month] - date.day;

        if (days <= days_remaining) {
            date.day += days;
            break;
        } else {
            days -= (days_remaining + 1);
            date.day = 1;
            date.month++;
            if (date.month > 12) {
                date.month = 1;
            }
        }
    }
}
```
**Analysis**: Adds days to date handling month boundaries.

**POTENTIAL BUG FOUND**: Line 243
```cpp
int days_remaining = days_in_month[date.month] - date.day;
```

This calculates days remaining in the current month, but let me verify with an example:
- If date is 06-28 (June 28), and we need to add 5 days
- days_remaining = 30 - 28 = 2 days remaining in June
- Since days (5) > days_remaining (2):
  - days = 5 - (2 + 1) = 5 - 3 = 2
  - date.day = 1, date.month = 7
  - Next iteration: days_remaining = 31 - 1 = 30
  - Since days (2) <= days_remaining (30):
  - date.day = 1 + 2 = 3
- Result: 07-03 ✓

Let me verify another edge case: 06-30 + 1 day
- days_remaining = 30 - 30 = 0
- Since days (1) > days_remaining (0):
  - days = 1 - (0 + 1) = 0
  - date.day = 1, date.month = 7
  - Loop exits since days = 0
- Result: 07-01 ✓

### 6. Date::format() (lines 153-160)
```cpp
void format(char* buffer) const {
    buffer[0] = '0' + (month / 10);
    buffer[1] = '0' + (month % 10);
    buffer[2] = '-';
    buffer[3] = '0' + (day / 10);
    buffer[4] = '0' + (day % 10);
    buffer[5] = '\0';
}
```
**Analysis**: Formats date as "mm-dd" ✓

### 7. Time::format() (lines 208-215)
```cpp
void format(char* buffer) const {
    buffer[0] = '0' + (hour / 10);
    buffer[1] = '0' + (hour % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (minute / 10);
    buffer[4] = '0' + (minute % 10);
    buffer[5] = '\0';
}
```
**Analysis**: Formats time as "hr:mi" ✓

## Hand-Verified Test Case from basic_2

### Train: TOFOREIGNLANDS
```
add_train -i TOFOREIGNLANDS -n 12 -m 92416
-s 湖北省黄石市|安徽省滁州市|江西省乐平市|河南省鹤壁市|四川省都江堰市|贵州省贵阳市|北京市|福建省建阳市|江西省上饶市|广东省清远市|河北省任丘市|四川省峨眉山市
-x 01:26
-t 215|20|146|66|30|129|30|209|104|206|111
-o 10|9|10|9|4|9|5|3|8|8
```

Query: `query_train -d 08-01 -i TOFOREIGNLANDS`

### Manual Calculation:

| Station | Calculation | Arrive | Stopover | Depart | Expected | Match |
|---------|------------|--------|----------|--------|----------|-------|
| 1. 湖北省黄石市 | Start | - | - | 08-01 01:26 | xx-xx xx:xx -> 08-01 01:26 | ✓ |
| 2. 安徽省滁州市 | 01:26 + 215 min = 01:26 + 3:35 | 08-01 05:01 | 10 min | 08-01 05:11 | 08-01 05:01 -> 08-01 05:11 | ✓ |
| 3. 江西省乐平市 | 05:11 + 20 min | 08-01 05:31 | 9 min | 08-01 05:40 | 08-01 05:31 -> 08-01 05:40 | ✓ |
| 4. 河南省鹤壁市 | 05:40 + 146 min = 05:40 + 2:26 | 08-01 08:06 | 10 min | 08-01 08:16 | 08-01 08:06 -> 08-01 08:16 | ✓ |
| 5. 四川省都江堰市 | 08:16 + 66 min = 08:16 + 1:06 | 08-01 09:22 | 9 min | 08-01 09:31 | 08-01 09:22 -> 08-01 09:31 | ✓ |
| 6. 贵州省贵阳市 | 09:31 + 30 min | 08-01 10:01 | 4 min | 08-01 10:05 | 08-01 10:01 -> 08-01 10:05 | ✓ |
| 7. 北京市 | 10:05 + 129 min = 10:05 + 2:09 | 08-01 12:14 | 9 min | 08-01 12:23 | 08-01 12:14 -> 08-01 12:23 | ✓ |
| 8. 福建省建阳市 | 12:23 + 30 min | 08-01 12:53 | 5 min | 08-01 12:58 | 08-01 12:53 -> 08-01 12:58 | ✓ |
| 9. 江西省上饶市 | 12:58 + 209 min = 12:58 + 3:29 | 08-01 16:27 | 3 min | 08-01 16:30 | 08-01 16:27 -> 08-01 16:30 | ✓ |
| 10. 广东省清远市 | 16:30 + 104 min = 16:30 + 1:44 | 08-01 18:14 | 8 min | 08-01 18:22 | 08-01 18:14 -> 08-01 18:22 | ✓ |
| 11. 河北省任丘市 | 18:22 + 206 min = 18:22 + 3:26 | 08-01 21:48 | 8 min | 08-01 21:56 | 08-01 21:48 -> 08-01 21:56 | ✓ |
| 12. 四川省峨眉山市 | 21:56 + 111 min = 21:56 + 1:51 | 08-01 23:47 | - | - | 08-01 23:47 -> xx-xx xx:xx | ✓ |

**All 12 stations match perfectly!** ✓

### Testing Midnight Crossing

Let's verify a calculation that crosses midnight:
- Station 11 departs at 21:56
- Add 111 minutes of travel time
- 21:56 + 111 min = 21:56 + 1:51

Manual calculation:
- 21 * 60 + 56 = 1316 minutes
- 1316 + 111 = 1427 minutes
- 1427 / 60 = 23 hours
- 1427 % 60 = 47 minutes
- Result: 23:47 (same day since 1427 < 1440)

This is correct! The train arrives at 23:47 on 08-01, which is 13 minutes before midnight.

### Testing Month Boundary Crossing

Let me verify a case that crosses from June to July.

Looking for a train query on 06-30:
Let me search for this in the test data.

## Edge Case Analysis

### Edge Case 1: Time crossing midnight
Example: 23:00 + 120 minutes
- Expected: 01:00 next day
- Code calculation:
  - total = 1380 + 120 = 1500
  - days = 1500 / 1440 = 1
  - total = 1500 % 1440 = 60
  - hour = 1, minute = 0
  - Returns 1 day ✓

### Edge Case 2: Date crossing month boundary (June -> July)
Example: 06-30 + 1 day
- Expected: 07-01
- Code calculation (line 243-255):
  - days_remaining = 30 - 30 = 0
  - days (1) > days_remaining (0): true
  - days = 1 - (0 + 1) = 0
  - date.day = 1, date.month = 7
  - Loop exits
  - Result: 07-01 ✓

### Edge Case 3: Date crossing month boundary (July -> August)
Example: 07-31 + 1 day
- Expected: 08-01
- Code calculation:
  - days_remaining = 31 - 31 = 0
  - days (1) > days_remaining (0): true
  - days = 1 - (0 + 1) = 0
  - date.day = 1, date.month = 8
  - Result: 08-01 ✓

### Edge Case 4: Multiple day addition within same month
Example: 07-10 + 5 days
- Expected: 07-15
- Code calculation:
  - days_remaining = 31 - 10 = 21
  - days (5) <= days_remaining (21): true
  - date.day = 10 + 5 = 15
  - Result: 07-15 ✓

### Edge Case 5: Addition spanning multiple months
Example: 06-28 + 35 days
- Expected: 08-02
- Code calculation:
  - Iteration 1: days_remaining = 30 - 28 = 2
    - days (35) > days_remaining (2): true
    - days = 35 - (2 + 1) = 32
    - date = 07-01
  - Iteration 2: days_remaining = 31 - 1 = 30
    - days (32) > days_remaining (30): true
    - days = 32 - (30 + 1) = 1
    - date = 08-01
  - Iteration 3: days_remaining = 31 - 1 = 30
    - days (1) <= days_remaining (30): true
    - date.day = 1 + 1 = 2
    - Result: 08-02 ✓

## Summary

### Functions Reviewed:
1. ✓ Date::Date(const char*) - Correct parsing of mm-dd format
2. ✓ Time::Time(const char*) - Correct parsing of hr:mi format
3. ✓ Date::toDayNumber() - Correct day counting (June=30, July=31, August=31)
4. ✓ Time::addMinutes() - Correct 24-hour wraparound and day overflow
5. ✓ DateTime::addMinutes() - Correct date advancement with month boundaries
6. ✓ Date::format() - Correct mm-dd output
7. ✓ Time::format() - Correct hr:mi output

### Test Results:
- ✓ 12-station train query fully verified with hand calculations
- ✓ Near-midnight time calculation (23:47) verified
- ✓ Month boundary edge cases verified
- ✓ Multi-day addition verified
- ✓ No off-by-one errors detected

## Verdict: **PASS**

All date/time calculations are mathematically correct. The implementation properly handles:
- Time parsing and formatting
- Date parsing and formatting
- Time arithmetic with midnight wraparound
- Date arithmetic with month boundaries
- Multi-day calculations spanning months
- Edge cases at month boundaries (June 30, July 31)

No bugs or incorrect calculations were found.
