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

// Key type for trains (wraps char array to make it assignable)
struct TrainKey {
    char id[25];

    TrainKey() {
        id[0] = '\0';
    }

    explicit TrainKey(const char* str) {
        strncpy(id, str, 24);
        id[24] = '\0';
    }

    TrainKey& operator=(const TrainKey& other) {
        if (this != &other) {
            strncpy(id, other.id, 24);
            id[24] = '\0';
        }
        return *this;
    }

    bool operator==(const TrainKey& other) const {
        return strcmp(id, other.id) == 0;
    }

    bool operator<(const TrainKey& other) const {
        return strcmp(id, other.id) < 0;
    }

    bool operator>(const TrainKey& other) const {
        return strcmp(id, other.id) > 0;
    }

    bool operator>=(const TrainKey& other) const {
        return !(*this < other);
    }

    bool operator<=(const TrainKey& other) const {
        return !(*this > other);
    }
};

// Train data structure
struct Train {
    char trainID[25];          // Train identifier (max 20 chars)
    int stationNum;            // Number of stations (2-100)
    int seatNum;               // Number of seats per train
    Station stations[100];     // Array of stations (up to 100)
    int prices[99];            // Price from station i to i+1 (up to 99)
    Time startTime;            // Departure time from first station
    int travelTimes[99];       // Travel time from station i to i+1 in minutes (up to 99)
    int stopoverTimes[98];     // Stopover time at station i (1 to n-2, up to 98)
    Date saleDate[2];          // Sale date range: [0]=start, [1]=end
    char type;                 // Train type (single character)
    bool released;             // Whether train has been released

    Train() : stationNum(0), seatNum(0), type('\0'), released(false) {
        trainID[0] = '\0';
        for (int i = 0; i < 99; i++) {
            prices[i] = 0;
            travelTimes[i] = 0;
            if (i < 98) stopoverTimes[i] = 0;
        }
    }

    Train(const char* id, int stNum, int seNum, char t)
        : stationNum(stNum), seatNum(seNum), type(t), released(false) {
        strncpy(trainID, id, 24);
        trainID[24] = '\0';
        for (int i = 0; i < 99; i++) {
            prices[i] = 0;
            travelTimes[i] = 0;
            if (i < 98) stopoverTimes[i] = 0;
        }
    }
};

// Order data structure for ticket management
struct Order {
    char username[25];        // User who made the order
    char trainID[25];        // Train ID
    Date departure_date;     // Date of departure from starting station
    char from_station[35];   // Starting station
    char to_station[35];     // Ending station
    int from_idx;            // Index of starting station in train
    int to_idx;              // Index of ending station in train
    int ticket_count;        // Number of tickets
    int total_price;         // Total price for all tickets
    char status;             // 's'=success, 'p'=pending, 'r'=refunded
    long long timestamp;     // For ordering (monotonic counter)

    Order() : from_idx(0), to_idx(0), ticket_count(0), total_price(0),
              status('s'), timestamp(0) {
        username[0] = '\0';
        trainID[0] = '\0';
        from_station[0] = '\0';
        to_station[0] = '\0';
    }

    Order(const char* user, const char* train_id, const Date& date,
          const char* from_st, const char* to_st, int f_idx, int t_idx,
          int count, int price, char stat, long long ts)
        : departure_date(date), from_idx(f_idx), to_idx(t_idx),
          ticket_count(count), total_price(price), status(stat), timestamp(ts) {
        strncpy(username, user, 24);
        username[24] = '\0';
        strncpy(trainID, train_id, 24);
        trainID[24] = '\0';
        strncpy(from_station, from_st, 34);
        from_station[34] = '\0';
        strncpy(to_station, to_st, 34);
        to_station[34] = '\0';
    }
};

// Key for ordering orders by username and timestamp
struct OrderKey {
    char username[25];
    long long timestamp;

    OrderKey() : timestamp(0) {
        username[0] = '\0';
    }

    OrderKey(const char* user, long long ts) : timestamp(ts) {
        strncpy(username, user, 24);
        username[24] = '\0';
    }

    bool operator==(const OrderKey& other) const {
        return strcmp(username, other.username) == 0 && timestamp == other.timestamp;
    }

    bool operator<(const OrderKey& other) const {
        int cmp = strcmp(username, other.username);
        if (cmp != 0) return cmp < 0;
        return timestamp < other.timestamp;
    }

    bool operator>(const OrderKey& other) const {
        return other < *this;
    }

    bool operator<=(const OrderKey& other) const {
        return !(*this > other);
    }

    bool operator>=(const OrderKey& other) const {
        return !(*this < other);
    }
};

// Seat availability tracking per train and date
struct __attribute__((packed)) SeatAvailability {
    char trainID[25];
    Date date;
    int available[99];  // available[i] = seats available from station i to i+1

    SeatAvailability() {
        trainID[0] = '\0';
        for (int i = 0; i < 99; i++) {
            available[i] = 0;
        }
    }

    SeatAvailability(const char* train_id, const Date& d, int total_seats, int num_segments) : date(d) {
        strncpy(trainID, train_id, 24);
        trainID[24] = '\0';
        for (int i = 0; i < num_segments; i++) {
            available[i] = total_seats;
        }
        for (int i = num_segments; i < 99; i++) {
            available[i] = 0;
        }
    }
};

// Key for seat availability (trainID + date)
struct SeatKey {
    TrainKey trainID;
    Date date;

    SeatKey() {}

    SeatKey(const char* train_id, const Date& d) : trainID(train_id), date(d) {}

    bool operator==(const SeatKey& other) const {
        return trainID == other.trainID && date == other.date;
    }

    bool operator<(const SeatKey& other) const {
        if (!(trainID == other.trainID)) return trainID < other.trainID;
        return date < other.date;
    }

    bool operator>(const SeatKey& other) const {
        return other < *this;
    }

    bool operator<=(const SeatKey& other) const {
        return !(*this > other);
    }

    bool operator>=(const SeatKey& other) const {
        return !(*this < other);
    }
};

#endif // TYPES_HPP
