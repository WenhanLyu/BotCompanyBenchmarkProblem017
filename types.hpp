#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstring>

/**
 * Basic data types for the train ticket management system
 */

// User data structure
struct User {
    char username[25];    // Max 20 chars + null terminator + padding
    char password[35];    // Max 30 chars + null terminator + padding
    char name[35];        // Chinese characters, max length consideration
    char email[35];       // Max 30 chars + null terminator + padding
    int privilege;        // 0-10

    User() : privilege(0) {
        username[0] = '\0';
        password[0] = '\0';
        name[0] = '\0';
        email[0] = '\0';
    }

    User(const char* user, const char* pass, const char* n, const char* mail, int priv)
        : privilege(priv) {
        strncpy(username, user, 24);
        username[24] = '\0';

        strncpy(password, pass, 34);
        password[34] = '\0';

        strncpy(name, n, 34);
        name[34] = '\0';

        strncpy(email, mail, 34);
        email[34] = '\0';
    }

    // Check password
    bool checkPassword(const char* pass) const {
        return strcmp(password, pass) == 0;
    }

    // Update password
    void setPassword(const char* pass) {
        strncpy(password, pass, 34);
        password[34] = '\0';
    }

    // Update name
    void setName(const char* n) {
        strncpy(name, n, 34);
        name[34] = '\0';
    }

    // Update email
    void setEmail(const char* mail) {
        strncpy(email, mail, 34);
        email[34] = '\0';
    }
};

// Session data for tracking logged-in users
struct Session {
    char username[25];
    bool logged_in;

    Session() : logged_in(false) {
        username[0] = '\0';
    }

    explicit Session(const char* user) : logged_in(true) {
        strncpy(username, user, 24);
        username[24] = '\0';
    }
};

// Train station structure
struct Station {
    char name[35];  // Chinese characters, up to 10 characters

    Station() {
        name[0] = '\0';
    }

    explicit Station(const char* n) {
        strncpy(name, n, 34);
        name[34] = '\0';
    }

    bool operator==(const Station& other) const {
        return strcmp(name, other.name) == 0;
    }

    bool operator<(const Station& other) const {
        return strcmp(name, other.name) < 0;
    }
};

// Date structure for handling dates (mm-dd format, June-August 2021)
struct Date {
    int month;  // 6-8
    int day;    // 1-31

    Date() : month(6), day(1) {}

    Date(int m, int d) : month(m), day(d) {}

    // Parse from string "mm-dd"
    explicit Date(const char* str) {
        if (str && strlen(str) >= 5) {
            month = (str[0] - '0') * 10 + (str[1] - '0');
            day = (str[3] - '0') * 10 + (str[4] - '0');
        } else {
            month = 6;
            day = 1;
        }
    }

    // Convert to day number (for easy comparison)
    int toDayNumber() const {
        const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int total = day;
        for (int i = 1; i < month; i++) {
            total += days_in_month[i];
        }
        return total;
    }

    bool operator==(const Date& other) const {
        return month == other.month && day == other.day;
    }

    bool operator<(const Date& other) const {
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    bool operator<=(const Date& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const Date& other) const {
        return !(*this <= other);
    }

    bool operator>=(const Date& other) const {
        return !(*this < other);
    }

    // Format as "mm-dd"
    void format(char* buffer) const {
        buffer[0] = '0' + (month / 10);
        buffer[1] = '0' + (month % 10);
        buffer[2] = '-';
        buffer[3] = '0' + (day / 10);
        buffer[4] = '0' + (day % 10);
        buffer[5] = '\0';
    }
};

// Time structure for handling time (hr:mi format)
struct Time {
    int hour;    // 0-23
    int minute;  // 0-59

    Time() : hour(0), minute(0) {}

    Time(int h, int m) : hour(h), minute(m) {}

    // Parse from string "hr:mi"
    explicit Time(const char* str) {
        if (str && strlen(str) >= 5) {
            hour = (str[0] - '0') * 10 + (str[1] - '0');
            minute = (str[3] - '0') * 10 + (str[4] - '0');
        } else {
            hour = 0;
            minute = 0;
        }
    }

    // Convert to minutes since midnight
    int toMinutes() const {
        return hour * 60 + minute;
    }

    // Add minutes (returns number of days passed)
    int addMinutes(int mins) {
        int total = toMinutes() + mins;
        int days = total / (24 * 60);
        total %= (24 * 60);
        hour = total / 60;
        minute = total % 60;
        return days;
    }

    bool operator==(const Time& other) const {
        return hour == other.hour && minute == other.minute;
    }

    bool operator<(const Time& other) const {
        if (hour != other.hour) return hour < other.hour;
        return minute < other.minute;
    }

    // Format as "hr:mi"
    void format(char* buffer) const {
        buffer[0] = '0' + (hour / 10);
        buffer[1] = '0' + (hour % 10);
        buffer[2] = ':';
        buffer[3] = '0' + (minute / 10);
        buffer[4] = '0' + (minute % 10);
        buffer[5] = '\0';
    }
};

// DateTime structure combining date and time
struct DateTime {
    Date date;
    Time time;

    DateTime() {}

    DateTime(const Date& d, const Time& t) : date(d), time(t) {}

    bool operator<(const DateTime& other) const {
        if (!(date == other.date)) return date < other.date;
        return time < other.time;
    }

    bool operator==(const DateTime& other) const {
        return date == other.date && time == other.time;
    }

    // Add minutes to datetime
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
};

#endif // TYPES_HPP
