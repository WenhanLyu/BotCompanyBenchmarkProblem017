# Date/Time Calculation Verification Report

## 1. Code Review of types.hpp

### Time::addMinutes() (lines 189-196)
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

**Analysis:**
- Converts current time to minutes: `toMinutes()` returns `hour * 60 + minute` ✅
- Adds minutes to get total
- Calculates days overflow: `total / 1440` (1440 = 24 * 60 minutes per day) ✅
- Gets remaining minutes in day: `total % 1440` ✅
- Converts back to hour and minute ✅
- Returns number of days that overflowed ✅

**Edge Case Test - Midnight Crossing:**
- Start: 23:50 (1430 minutes from midnight)
- Add 30 minutes: total = 1460 minutes
- Days: 1460 / 1440 = 1 day ✅
- Remaining: 1460 % 1440 = 20 minutes
- Result: 00:20, 1 day overflow ✅

### DateTime::addMinutes() (lines 237-257)
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

**Analysis:**
- Gets day overflow from time.addMinutes() ✅
- `days_remaining` = number of days we can add without changing month ✅
- If days fit in current month: add to day ✅
- If days exceed month: subtract (days_remaining + 1) and roll to next month ✅

**Edge Case Test - Month Boundary:**
- June 30 + 1 day:
  - days_remaining = 30 - 30 = 0
  - days (1) > days_remaining (0)
  - days -= (0 + 1) = 0
  - Result: July 1 ✅

- June 29 + 2 days:
  - days_remaining = 30 - 29 = 1
  - days (2) > days_remaining (1)
  - days -= (1 + 1) = 0
  - Result: July 1 ✅

### Date and Time Parsing
**Date::Date(const char* str)** (lines 111-119):
- Parses "mm-dd" format correctly ✅
- month = (str[0] - '0') * 10 + (str[1] - '0')
- day = (str[3] - '0') * 10 + (str[4] - '0')

**Time::Time(const char* str)** (lines 173-181):
- Parses "hr:mi" format correctly ✅
- hour = (str[0] - '0') * 10 + (str[1] - '0')
- minute = (str[3] - '0') * 10 + (str[4] - '0')

### Date and Time Formatting
**Date::format()** (lines 153-160):
- Formats as "mm-dd" correctly ✅

**Time::format()** (lines 208-215):
- Formats as "hr:mi" correctly ✅

## 2. Query Train Implementation Review (main.cpp lines 584-668)

**Key Logic Flow:**
1. Initialize current_time = (query_date, startTime)
2. For each station:
   - Station 0: Output start time as departure
   - Stations 1 to n-1:
     - Output current_time as arrival
     - Add stopover time
     - Output current_time as departure
     - Add travel time to next station
   - Station n: Output current_time as arrival

**Implementation is CORRECT** ✅

## 3. Manual Calculation Verification - TOFOREIGNLANDS Train

**Train Data:**
- trainID: TOFOREIGNLANDS
- Query date: 08-01
- Start time: 01:26
- Stations: 12
- Travel times: 215|20|146|66|30|129|30|209|104|206|111 (minutes)
- Stopover times: 10|9|10|9|4|9|5|3|8|8 (minutes)
- Prices: 214|3139|1892|4712|4937|1843|3057|2796|186|1322|199

**Manual Calculation vs Actual Output:**

| Station | Manual Arrival | Actual Arrival | Manual Departure | Actual Departure | Manual Price | Actual Price |
|---------|---------------|----------------|------------------|------------------|--------------|--------------|
| 0 (湖北省黄石市) | xx-xx xx:xx | xx-xx xx:xx | 08-01 01:26 | 08-01 01:26 | 0 | 0 |
| 1 (安徽省滁州市) | 08-01 05:01 | 08-01 05:01 | 08-01 05:11 | 08-01 05:11 | 214 | 214 |
| 2 (江西省乐平市) | 08-01 05:31 | 08-01 05:31 | 08-01 05:40 | 08-01 05:40 | 3353 | 3353 |
| 3 (河南省鹤壁市) | 08-01 08:06 | 08-01 08:06 | 08-01 08:16 | 08-01 08:16 | 5245 | 5245 |
| 4 (四川省都江堰市) | 08-01 09:22 | 08-01 09:22 | 08-01 09:31 | 08-01 09:31 | 9957 | 9957 |
| 5 (贵州省贵阳市) | 08-01 10:01 | 08-01 10:01 | 08-01 10:05 | 08-01 10:05 | 14894 | 14894 |
| 6 (北京市) | 08-01 12:14 | 08-01 12:14 | 08-01 12:23 | 08-01 12:23 | 16737 | 16737 |
| 7 (福建省建阳市) | 08-01 12:53 | 08-01 12:53 | 08-01 12:58 | 08-01 12:58 | 19794 | 19794 |
| 8 (江西省上饶市) | 08-01 16:27 | 08-01 16:27 | 08-01 16:30 | 08-01 16:30 | 22590 | 22590 |
| 9 (广东省清远市) | 08-01 18:14 | 08-01 18:14 | 08-01 18:22 | 08-01 18:22 | 22776 | 22776 |
| 10 (河北省任丘市) | 08-01 21:48 | 08-01 21:48 | 08-01 21:56 | 08-01 21:56 | 24098 | 24098 |
| 11 (四川省峨眉山市) | 08-01 23:47 | 08-01 23:47 | xx-xx xx:xx | xx-xx xx:xx | 24297 | 24297 |

**Result: 100% MATCH** ✅

### Sample Calculation Details (Station 1):
- Start time: 08-01 01:26
- Travel time from station 0 to 1: 215 minutes
- Calculation: 01:26 = 1×60 + 26 = 86 minutes
- 86 + 215 = 301 minutes
- 301 ÷ 60 = 5 hours remainder 1 minute
- Arrival: 08-01 05:01 ✅

### Sample Calculation Details (Station 7):
- Previous departure: 08-01 12:58
- Travel time from station 7 to 8: 209 minutes
- Calculation: 12:58 = 12×60 + 58 = 778 minutes
- 778 + 209 = 987 minutes
- 987 ÷ 60 = 16 hours remainder 27 minutes
- Arrival at station 8: 08-01 16:27 ✅

## 4. Edge Cases Testing

Now testing day boundary crossing with a custom test case...
