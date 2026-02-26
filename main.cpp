#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "types.hpp"
#include "hashtable.hpp"
#include "fileio.hpp"
#include "bptree.hpp"
#include "order_system.hpp"

// User database and session management
StringHashTable<User> users;
StringHashTable<bool> logged_in_users;  // Track logged-in users
int user_count = 0;  // Track total number of users

// Train database (use smaller degree for large Train struct)
BPTree<TrainKey, Train, 16> trains;

// Order and seat tracking (defined here, declared extern in order_system.hpp)
BPTree<OrderKey, Order, 32> orders;
BPTree<SeatKey, SeatAvailability, 16> seats;

// Persistence functions
void save_users() {
    std::ofstream out("users.dat", std::ios::binary);
    if (!out) return;

    // Write user count
    out.write(reinterpret_cast<const char*>(&user_count), sizeof(int));

    // Write each user by iterating through the hash table
    users.forEach([&out](const char* key, const User& user) {
        out.write(reinterpret_cast<const char*>(&user), sizeof(User));
    });

    out.close();
}

void load_users() {
    std::ifstream in("users.dat", std::ios::binary);
    if (!in) return;  // File doesn't exist yet

    // Read user count
    int saved_count;
    in.read(reinterpret_cast<char*>(&saved_count), sizeof(int));

    // Validate read
    if (in.fail() || saved_count < 0 || saved_count > 1000000) {
        in.close();
        return;
    }

    // Read each user
    for (int i = 0; i < saved_count; i++) {
        User user;
        in.read(reinterpret_cast<char*>(&user), sizeof(User));
        if (in.fail()) {
            break;
        }
        users.insert(user.username, user);
    }

    user_count = saved_count;
    in.close();
}

// Parse command parameters
// Format: command -key1 value1 -key2 value2 ...
class CommandParser {
private:
    char params[20][4096];  // Max 20 parameters, each max 4KB (for long station lists)
    char keys[20];
    int param_count;

public:
    CommandParser() : param_count(0) {}

    void parse(const std::string& line) {
        param_count = 0;
        size_t pos = 0;

        while (pos < line.length()) {
            // Skip whitespace
            while (pos < line.length() && (line[pos] == ' ' || line[pos] == '\t')) {
                pos++;
            }

            if (pos >= line.length()) break;

            // Read key (starts with - after whitespace)
            if (line[pos] == '-' && pos + 1 < line.length()) {
                keys[param_count] = line[pos + 1];
                pos += 2;

                // Skip whitespace
                while (pos < line.length() && (line[pos] == ' ' || line[pos] == '\t')) {
                    pos++;
                }

                // Read value until next whitespace followed by dash, or end of line
                int val_idx = 0;
                while (pos < line.length()) {
                    // Check if we're at the start of a new parameter (whitespace + dash)
                    if (line[pos] == ' ' || line[pos] == '\t') {
                        size_t next_pos = pos + 1;
                        while (next_pos < line.length() && (line[next_pos] == ' ' || line[next_pos] == '\t')) {
                            next_pos++;
                        }
                        if (next_pos < line.length() && line[next_pos] == '-') {
                            break;  // Found next parameter
                        }
                    }
                    params[param_count][val_idx++] = line[pos++];
                }
                // Trim trailing whitespace from value
                while (val_idx > 0 && (params[param_count][val_idx - 1] == ' ' || params[param_count][val_idx - 1] == '\t')) {
                    val_idx--;
                }
                params[param_count][val_idx] = '\0';
                param_count++;
            } else {
                pos++;
            }
        }
    }

    const char* get(char key) const {
        for (int i = 0; i < param_count; i++) {
            if (keys[i] == key) {
                return params[i];
            }
        }
        return nullptr;
    }

    bool has(char key) const {
        return get(key) != nullptr;
    }
};

// Helper function to parse pipe-separated values
int parsePipeSeparated(const char* input, char result[][105], int max_count) {
    int count = 0;
    int pos = 0;
    int idx = 0;

    while (input[pos] != '\0' && count < max_count) {
        if (input[pos] == '|') {
            result[count][idx] = '\0';
            count++;
            idx = 0;
            pos++;
        } else {
            result[count][idx++] = input[pos++];
        }
    }

    if (idx > 0 && count < max_count) {
        result[count][idx] = '\0';
        count++;
    }

    return count;
}

// Helper function to parse integers from pipe-separated values
int parsePipeSeparatedInts(const char* input, int result[], int max_count) {
    int count = 0;
    int pos = 0;

    while (input[pos] != '\0' && count < max_count) {
        int value = 0;
        while (input[pos] != '\0' && input[pos] != '|') {
            value = value * 10 + (input[pos] - '0');
            pos++;
        }
        result[count++] = value;

        if (input[pos] == '|') {
            pos++;
        }
    }

    return count;
}

// Command implementations

int cmd_add_user(const CommandParser& parser) {
    const char* cur_username = parser.get('c');
    const char* username = parser.get('u');
    const char* password = parser.get('p');
    const char* name = parser.get('n');
    const char* mail = parser.get('m');
    const char* priv_str = parser.get('g');

    if (!username || !password || !name || !mail) {
        return -1;
    }

    // Check if username already exists
    if (users.contains(username)) {
        return -1;
    }

    int privilege = 0;
    if (priv_str) {
        privilege = 0;
        for (int i = 0; priv_str[i] != '\0'; i++) {
            privilege = privilege * 10 + (priv_str[i] - '0');
        }
    }

    // First user special case
    if (user_count == 0) {
        privilege = 10;
        User new_user(username, password, name, mail, privilege);
        users.insert(username, new_user);
        user_count++;
        save_users();
        return 0;
    }

    // Not first user - check permissions
    if (!cur_username) {
        return -1;
    }

    // Check if current user is logged in
    bool* is_logged_in = logged_in_users.find(cur_username);
    if (!is_logged_in || !(*is_logged_in)) {
        return -1;
    }

    // Get current user
    User* cur_user = users.find(cur_username);
    if (!cur_user) {
        return -1;
    }

    // Check privilege: new user's privilege must be lower than current user's
    if (privilege >= cur_user->privilege) {
        return -1;
    }

    // Create new user
    User new_user(username, password, name, mail, privilege);
    users.insert(username, new_user);
    user_count++;

    // Save users to disk
    save_users();

    return 0;
}

int cmd_login(const CommandParser& parser) {
    const char* username = parser.get('u');
    const char* password = parser.get('p');

    if (!username || !password) {
        return -1;
    }

    // Check if user exists
    User* user = users.find(username);
    if (!user) {
        return -1;
    }

    // Check password
    if (!user->checkPassword(password)) {
        return -1;
    }

    // Check if already logged in (duplicate login not allowed)
    bool* is_logged_in = logged_in_users.find(username);
    if (is_logged_in && *is_logged_in) {
        return -1;
    }

    // Log in user
    logged_in_users.insert(username, true);

    return 0;
}

int cmd_logout(const CommandParser& parser) {
    const char* username = parser.get('u');

    if (!username) {
        return -1;
    }

    // Check if user is logged in
    bool* is_logged_in = logged_in_users.find(username);
    if (!is_logged_in || !(*is_logged_in)) {
        return -1;
    }

    // Log out user
    logged_in_users.insert(username, false);

    return 0;
}

int cmd_query_profile(const CommandParser& parser) {
    const char* cur_username = parser.get('c');
    const char* username = parser.get('u');

    if (!cur_username || !username) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Check if current user is logged in
    bool* is_logged_in = logged_in_users.find(cur_username);
    if (!is_logged_in || !(*is_logged_in)) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Get both users
    User* cur_user = users.find(cur_username);
    User* target_user = users.find(username);

    if (!cur_user || !target_user) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Check permission: privilege of -c > privilege of -u, OR -c and -u are the same
    if (strcmp(cur_username, username) != 0 && cur_user->privilege <= target_user->privilege) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Output user information
    std::cout << target_user->username << " "
              << target_user->name << " "
              << target_user->email << " "
              << target_user->privilege << std::endl;

    return 0;
}

int cmd_modify_profile(const CommandParser& parser) {
    const char* cur_username = parser.get('c');
    const char* username = parser.get('u');

    if (!cur_username || !username) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Check if current user is logged in
    bool* is_logged_in = logged_in_users.find(cur_username);
    if (!is_logged_in || !(*is_logged_in)) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Get both users
    User* cur_user = users.find(cur_username);
    User* target_user = users.find(username);

    if (!cur_user || !target_user) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Save the current user's privilege in case target_user == cur_user
    int cur_privilege = cur_user->privilege;

    // Check permission: privilege of -c > privilege of -u, OR -c and -u are the same
    if (strcmp(cur_username, username) != 0 && cur_privilege <= target_user->privilege) {
        std::cout << "-1" << std::endl;
        return -1;
    }

    // Create a copy of the target user to safely modify
    User updated_user = *target_user;

    // Modify fields if provided
    if (parser.has('p')) {
        updated_user.setPassword(parser.get('p'));
    }

    if (parser.has('n')) {
        updated_user.setName(parser.get('n'));
    }

    if (parser.has('m')) {
        updated_user.setEmail(parser.get('m'));
    }

    if (parser.has('g')) {
        const char* priv_str = parser.get('g');
        int new_privilege = 0;
        for (int i = 0; priv_str[i] != '\0'; i++) {
            new_privilege = new_privilege * 10 + (priv_str[i] - '0');
        }

        // New privilege must be lower than current user's privilege
        if (new_privilege >= cur_privilege) {
            std::cout << "-1" << std::endl;
            return -1;
        }

        updated_user.privilege = new_privilege;
    }

    // Update the user in the hash table
    users.insert(username, updated_user);

    // Save users to disk
    save_users();

    // Output modified user information
    std::cout << updated_user.username << " "
              << updated_user.name << " "
              << updated_user.email << " "
              << updated_user.privilege << std::endl;

    return 0;
}

int cmd_clean() {
    users.clear();
    logged_in_users.clear();
    user_count = 0;

    // Delete the persistence file
    std::remove("users.dat");

    // Clear trains
    trains.clear();

    return 0;
}

// Train command implementations

int cmd_add_train(const CommandParser& parser) {
    const char* trainID_str = parser.get('i');
    const char* stationNum_str = parser.get('n');
    const char* seatNum_str = parser.get('m');
    const char* stations_str = parser.get('s');
    const char* prices_str = parser.get('p');
    const char* startTime_str = parser.get('x');
    const char* travelTimes_str = parser.get('t');
    const char* stopoverTimes_str = parser.get('o');
    const char* saleDate_str = parser.get('d');
    const char* type_str = parser.get('y');

    if (!trainID_str || !stationNum_str || !seatNum_str || !stations_str ||
        !prices_str || !startTime_str || !travelTimes_str ||
        !stopoverTimes_str || !saleDate_str || !type_str) {
        return -1;
    }

    // Create trainID key
    TrainKey trainID(trainID_str);

    // Check if train already exists
    if (trains.contains(trainID)) {
        return -1;
    }

    // Create new train
    Train train;
    strncpy(train.trainID, trainID_str, 24);
    train.trainID[24] = '\0';

    // Parse stationNum
    train.stationNum = 0;
    for (int i = 0; stationNum_str[i] != '\0'; i++) {
        train.stationNum = train.stationNum * 10 + (stationNum_str[i] - '0');
    }

    // Parse seatNum
    train.seatNum = 0;
    for (int i = 0; seatNum_str[i] != '\0'; i++) {
        train.seatNum = train.seatNum * 10 + (seatNum_str[i] - '0');
    }

    // Parse stations
    char station_names[100][105];
    parsePipeSeparated(stations_str, station_names, 100);
    for (int i = 0; i < train.stationNum; i++) {
        train.stations[i] = Station(station_names[i]);
    }

    // Parse prices
    parsePipeSeparatedInts(prices_str, train.prices, 99);

    // Parse startTime
    train.startTime = Time(startTime_str);

    // Parse travelTimes
    parsePipeSeparatedInts(travelTimes_str, train.travelTimes, 99);

    // Parse stopoverTimes
    parsePipeSeparatedInts(stopoverTimes_str, train.stopoverTimes, 98);

    // Parse saleDate (two dates separated by |)
    char dates[2][105];
    parsePipeSeparated(saleDate_str, dates, 2);
    train.saleDate[0] = Date(dates[0]);
    train.saleDate[1] = Date(dates[1]);

    // Parse type
    train.type = type_str[0];

    // Set released to false
    train.released = false;

    // Insert into B+ tree
    if (!trains.insert(trainID, train)) {
        return -1;
    }

    return 0;
}

int cmd_delete_train(const CommandParser& parser) {
    const char* trainID_str = parser.get('i');

    if (!trainID_str) {
        return -1;
    }

    // Create trainID key
    TrainKey trainID(trainID_str);

    // Find train and make a copy
    Train* train_ptr = trains.find(trainID);
    if (!train_ptr) {
        return -1;
    }
    Train train = *train_ptr;

    // Check if train is released
    if (train.released) {
        return -1;
    }

    // Delete train
    if (!trains.remove(trainID)) {
        return -1;
    }

    return 0;
}

int cmd_release_train(const CommandParser& parser) {
    const char* trainID_str = parser.get('i');

    if (!trainID_str) {
        return -1;
    }

    // Create trainID key
    TrainKey trainID(trainID_str);

    // Find train and make a copy
    Train* train_ptr = trains.find(trainID);
    if (!train_ptr) {
        return -1;
    }
    Train train = *train_ptr;

    // Check if already released
    if (train.released) {
        return -1;
    }

    // Mark as released
    train.released = true;

    // Initialize seat availability for this train
    initializeSeatsForTrain(train);

    // Update in B+ tree (remove and reinsert)
    trains.remove(trainID);
    trains.insert(trainID, train);

    return 0;
}

int cmd_query_train(const CommandParser& parser) {
    const char* trainID_str = parser.get('i');
    const char* date_str = parser.get('d');

    if (!trainID_str || !date_str) {
        return -1;
    }

    // Create trainID key
    TrainKey trainID(trainID_str);

    // Find train and make a copy (pointer becomes invalid after other operations)
    Train* train_ptr = trains.find(trainID);
    if (!train_ptr) {
        return -1;
    }
    Train train = *train_ptr;

    // Parse date
    Date query_date(date_str);

    // Check if date is within sale range
    if (query_date < train.saleDate[0] || query_date > train.saleDate[1]) {
        return -1;
    }

    // Output first line: trainID and type
    std::cout << train.trainID << " " << train.type << std::endl;

    // Calculate and output station info
    DateTime current_time(query_date, train.startTime);
    int cumulative_price = 0;

    for (int i = 0; i < train.stationNum; i++) {
        std::cout << train.stations[i].name << " ";

        // Arrival time
        if (i == 0) {
            std::cout << "xx-xx xx:xx";
        } else {
            char date_buf[6], time_buf[6];
            current_time.date.format(date_buf);
            current_time.time.format(time_buf);
            std::cout << date_buf << " " << time_buf;
        }

        std::cout << " -> ";

        // Leaving time
        if (i < train.stationNum - 1) {
            // Add stopover time if not first station
            if (i > 0) {
                current_time.addMinutes(train.stopoverTimes[i - 1]);
            }

            char date_buf[6], time_buf[6];
            current_time.date.format(date_buf);
            current_time.time.format(time_buf);
            std::cout << date_buf << " " << time_buf << " ";

            // Add travel time to next station
            current_time.addMinutes(train.travelTimes[i]);
        } else {
            std::cout << "xx-xx xx:xx ";
        }

        // Price
        std::cout << cumulative_price << " ";

        // Seats
        if (i < train.stationNum - 1) {
            std::cout << train.seatNum;
        } else {
            std::cout << "x";
        }

        std::cout << std::endl;

        // Update cumulative price
        if (i < train.stationNum - 1) {
            cumulative_price += train.prices[i];
        }
    }

    return 0;
}

// Helper struct for query_ticket results
struct TicketResult {
    char trainID[25];
    char from[35];
    char to[35];
    DateTime leaving_time;
    DateTime arriving_time;
    int price;
    int seat;
    int travel_minutes;  // For sorting by time

    TicketResult() : price(0), seat(0), travel_minutes(0) {
        trainID[0] = '\0';
        from[0] = '\0';
        to[0] = '\0';
    }
};

// Comparison functions for sorting
bool compareByTime(const TicketResult& a, const TicketResult& b) {
    if (a.travel_minutes != b.travel_minutes) {
        return a.travel_minutes < b.travel_minutes;
    }
    return strcmp(a.trainID, b.trainID) < 0;
}

bool compareByCost(const TicketResult& a, const TicketResult& b) {
    if (a.price != b.price) {
        return a.price < b.price;
    }
    return strcmp(a.trainID, b.trainID) < 0;
}

int cmd_query_ticket(const CommandParser& parser) {
    const char* from_station = parser.get('s');
    const char* to_station = parser.get('t');
    const char* date_str = parser.get('d');
    const char* sort_by = parser.get('p');

    if (!from_station || !to_station || !date_str) {
        return -1;
    }

    // Parse departure date from station -s
    Date departure_date(date_str);

    // Default sort by time if not specified
    bool sort_by_time = true;
    if (sort_by && strcmp(sort_by, "cost") == 0) {
        sort_by_time = false;
    }

    // Collect matching trains
    TicketResult results[1000];
    int result_count = 0;

    // Iterate through all trains
    trains.forEach([&](const TrainKey& key, const Train& train) {
        // Only consider released trains
        if (!train.released) return;

        // Find indices of from_station and to_station
        int from_idx = -1;
        int to_idx = -1;

        for (int i = 0; i < train.stationNum; i++) {
            if (strcmp(train.stations[i].name, from_station) == 0) {
                from_idx = i;
            }
            if (strcmp(train.stations[i].name, to_station) == 0) {
                to_idx = i;
            }
        }

        // Check if both stations exist and from comes before to
        if (from_idx == -1 || to_idx == -1 || from_idx >= to_idx) {
            return;
        }

        // Calculate time offset from starting station to DEPARTURE from from_station
        // This includes: travel times + stopover times up to and including from_station
        int minutes_to_from = 0;
        for (int i = 0; i < from_idx; i++) {
            minutes_to_from += train.travelTimes[i];
            if (i > 0) {
                minutes_to_from += train.stopoverTimes[i - 1];
            }
        }
        // Add stopover time at from_station (if not the first station)
        if (from_idx > 0) {
            minutes_to_from += train.stopoverTimes[from_idx - 1];
        }

        // Calculate the starting date by working backwards from departure_date
        // We want: start_date + minutes_to_from results in departure from from_station on departure_date

        // Start with an initial guess: subtract full days from departure_date
        const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int days_to_subtract = minutes_to_from / (24 * 60);

        Date start_date = departure_date;

        // Subtract days while staying in valid range (months 6-8)
        while (days_to_subtract > 0) {
            if (start_date.day > days_to_subtract) {
                start_date.day -= days_to_subtract;
                break;
            } else {
                days_to_subtract -= start_date.day;
                start_date.month--;
                if (start_date.month < 6) {
                    // Gone before June - train doesn't run this early
                    return;
                }
                start_date.day = days_in_month[start_date.month];
            }
        }

        // Fine-tune: check if leaving_at_from matches departure_date
        DateTime leaving_at_from(start_date, train.startTime);
        leaving_at_from.addMinutes(minutes_to_from);

        // Adjust start_date forward if we're too early
        while (leaving_at_from.date < departure_date) {
            start_date.day++;
            if (start_date.day > days_in_month[start_date.month]) {
                start_date.day = 1;
                start_date.month++;
                if (start_date.month > 8) {
                    // Gone past August - train doesn't run this late
                    return;
                }
            }
            leaving_at_from = DateTime(start_date, train.startTime);
            leaving_at_from.addMinutes(minutes_to_from);
        }

        // Adjust start_date backward if we're too late
        while (leaving_at_from.date > departure_date) {
            start_date.day--;
            if (start_date.day < 1) {
                start_date.month--;
                if (start_date.month < 6) {
                    // Gone before June - train doesn't run this early
                    return;
                }
                start_date.day = days_in_month[start_date.month];
            }
            leaving_at_from = DateTime(start_date, train.startTime);
            leaving_at_from.addMinutes(minutes_to_from);
        }

        // Check if start_date is within sale range
        if (start_date < train.saleDate[0] || start_date > train.saleDate[1]) {
            return;
        }

        // Calculate arrival time at to_station
        DateTime arriving_at_to(start_date, train.startTime);
        int minutes_to_to = 0;
        for (int i = 0; i < to_idx; i++) {
            minutes_to_to += train.travelTimes[i];
            if (i > 0) {
                minutes_to_to += train.stopoverTimes[i - 1];
            }
        }
        arriving_at_to.addMinutes(minutes_to_to);

        // Calculate cumulative price from from_station to to_station
        int cumulative_price = 0;
        for (int i = from_idx; i < to_idx; i++) {
            cumulative_price += train.prices[i];
        }

        // Calculate travel time in minutes
        int travel_minutes = minutes_to_to - minutes_to_from;

        // Store result
        if (result_count < 1000) {
            TicketResult& result = results[result_count++];
            strncpy(result.trainID, train.trainID, 24);
            result.trainID[24] = '\0';
            strncpy(result.from, from_station, 34);
            result.from[34] = '\0';
            strncpy(result.to, to_station, 34);
            result.to[34] = '\0';
            result.leaving_time = leaving_at_from;
            result.arriving_time = arriving_at_to;
            result.price = cumulative_price;

            // Check seat availability - need to distinguish between "no data" and "0 seats"
            SeatKey seat_key(train.trainID, start_date);
            SeatAvailability* seat_data = seats.find(seat_key);
            if (seat_data) {
                // Seat tracking data exists, use actual availability
                int avail = checkAvailableSeats(train.trainID, start_date, from_idx, to_idx);
                result.seat = avail;
            } else {
                // No seat tracking data yet (no tickets sold), use full capacity
                result.seat = train.seatNum;
            }

            result.travel_minutes = travel_minutes;
        }
    });

    // Check if any trains found
    if (result_count == 0) {
        std::cout << "0" << std::endl;
        return 0;
    }

    // Sort results
    if (sort_by_time) {
        // Simple bubble sort by time
        for (int i = 0; i < result_count - 1; i++) {
            for (int j = 0; j < result_count - i - 1; j++) {
                if (!compareByTime(results[j], results[j + 1])) {
                    TicketResult temp = results[j];
                    results[j] = results[j + 1];
                    results[j + 1] = temp;
                }
            }
        }
    } else {
        // Simple bubble sort by cost
        for (int i = 0; i < result_count - 1; i++) {
            for (int j = 0; j < result_count - i - 1; j++) {
                if (!compareByCost(results[j], results[j + 1])) {
                    TicketResult temp = results[j];
                    results[j] = results[j + 1];
                    results[j + 1] = temp;
                }
            }
        }
    }

    // Output results
    std::cout << result_count << std::endl;
    for (int i = 0; i < result_count; i++) {
        char leaving_date[6], leaving_time[6];
        char arriving_date[6], arriving_time[6];

        results[i].leaving_time.date.format(leaving_date);
        results[i].leaving_time.time.format(leaving_time);
        results[i].arriving_time.date.format(arriving_date);
        results[i].arriving_time.time.format(arriving_time);

        std::cout << results[i].trainID << " "
                  << results[i].from << " "
                  << leaving_date << " " << leaving_time << " -> "
                  << results[i].to << " "
                  << arriving_date << " " << arriving_time << " "
                  << results[i].price << " "
                  << results[i].seat << std::endl;
    }

    return 0;
}

int cmd_buy_ticket(const CommandParser& parser) {
    const char* username = parser.get('u');
    const char* trainID = parser.get('i');
    const char* date_str = parser.get('d');
    const char* num_str = parser.get('n');
    const char* from_station = parser.get('f');
    const char* to_station = parser.get('t');
    const char* queue_str = parser.get('q');

    // Validate parameters
    if (!username || !trainID || !date_str || !num_str || !from_station || !to_station) {
        return -1;
    }

    // Parse ticket count
    int ticket_count = std::atoi(num_str);
    if (ticket_count <= 0) {
        return -1;
    }

    // Parse queue flag (default false)
    bool allow_queue = false;
    if (queue_str && strcmp(queue_str, "true") == 0) {
        allow_queue = true;
    }

    // Check if user is logged in
    bool* is_logged_in = logged_in_users.find(username);
    if (!is_logged_in || !(*is_logged_in)) {
        return -1;
    }

    // Find the train
    TrainKey key(trainID);
    Train* train = trains.find(key);
    if (!train) {
        return -1;
    }

    // Check if train is released
    if (!train->released) {
        return -1;
    }

    // Find station indices
    int from_idx = -1;
    int to_idx = -1;
    for (int i = 0; i < train->stationNum; i++) {
        if (strcmp(train->stations[i].name, from_station) == 0) {
            from_idx = i;
        }
        if (strcmp(train->stations[i].name, to_station) == 0) {
            to_idx = i;
        }
    }

    // Validate stations
    if (from_idx == -1 || to_idx == -1 || from_idx >= to_idx) {
        return -1;
    }

    // Parse the date (this is the departure date from from_station)
    Date departure_date(date_str);

    // Calculate starting date (need to reverse engineer like query_ticket)
    // Calculate time offset from starting station to DEPARTURE from from_station
    int minutes_to_from = 0;
    for (int i = 0; i < from_idx; i++) {
        minutes_to_from += train->travelTimes[i];
        if (i > 0) {
            minutes_to_from += train->stopoverTimes[i - 1];
        }
    }
    if (from_idx > 0) {
        minutes_to_from += train->stopoverTimes[from_idx - 1];
    }

    // Calculate the starting date
    const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days_to_subtract = minutes_to_from / (24 * 60);
    Date start_date = departure_date;

    while (days_to_subtract > 0) {
        if (start_date.day > days_to_subtract) {
            start_date.day -= days_to_subtract;
            break;
        } else {
            days_to_subtract -= start_date.day;
            start_date.month--;
            if (start_date.month < 6) {
                return -1;  // Out of range
            }
            start_date.day = days_in_month[start_date.month];
        }
    }

    // Fine-tune the start_date
    DateTime leaving_at_from(start_date, train->startTime);
    leaving_at_from.addMinutes(minutes_to_from);

    while (leaving_at_from.date < departure_date) {
        start_date.day++;
        if (start_date.day > days_in_month[start_date.month]) {
            start_date.day = 1;
            start_date.month++;
            if (start_date.month > 8) {
                return -1;  // Out of range
            }
        }
        leaving_at_from = DateTime(start_date, train->startTime);
        leaving_at_from.addMinutes(minutes_to_from);
    }

    while (leaving_at_from.date > departure_date) {
        start_date.day--;
        if (start_date.day < 1) {
            start_date.month--;
            if (start_date.month < 6) {
                return -1;  // Out of range
            }
            start_date.day = days_in_month[start_date.month];
        }
        leaving_at_from = DateTime(start_date, train->startTime);
        leaving_at_from.addMinutes(minutes_to_from);
    }

    // Check if start_date is within sale range
    if (start_date < train->saleDate[0] || start_date > train->saleDate[1]) {
        return -1;
    }

    // Validate ticket count doesn't exceed train capacity
    // Per FAQ: "If the number of tickets purchased is greater than the maximum number of seats on the train, can you still wait? No."
    if (ticket_count > train->seatNum) {
        return -1;
    }

    // Calculate total price (n tickets * sum of segment prices)
    int price_per_ticket = 0;
    for (int i = from_idx; i < to_idx; i++) {
        price_per_ticket += train->prices[i];
    }
    int total_price = price_per_ticket * ticket_count;

    // Check seat availability
    int available = checkAvailableSeats(trainID, start_date, from_idx, to_idx);

    if (available >= ticket_count) {
        // Enough seats - reserve and create order
        if (reserveSeats(trainID, start_date, from_idx, to_idx, ticket_count)) {
            createOrder(username, trainID, start_date, from_station, to_station,
                       from_idx, to_idx, ticket_count, total_price, 's');
            std::cout << total_price << std::endl;
            return 0;
        } else if (allow_queue) {
            // reserveSeats failed but queue is allowed - queue instead
            createOrder(username, trainID, start_date, from_station, to_station,
                       from_idx, to_idx, ticket_count, total_price, 'p');
            std::cout << "queue" << std::endl;
            return 0;
        } else {
            return -1;
        }
    } else if (allow_queue) {
        // Not enough seats, but queuing is allowed
        createOrder(username, trainID, start_date, from_station, to_station,
                   from_idx, to_idx, ticket_count, total_price, 'p');
        std::cout << "queue" << std::endl;
        return 0;
    } else {
        // Not enough seats and queuing not allowed
        return -1;
    }
}

int cmd_query_order(const CommandParser& parser) {
    const char* username = parser.get('u');

    if (!username) {
        return -1;
    }

    // Check if user is logged in
    bool* is_logged_in = logged_in_users.find(username);
    if (!is_logged_in || !(*is_logged_in)) {
        return -1;
    }

    // Check if user exists
    User* user = users.find(username);
    if (!user) {
        return -1;
    }

    // Collect all orders into a vector
    std::vector<Order> order_list;
    queryUserOrders(username, [&](const Order& order) {
        order_list.push_back(order);
    });

    // If no orders, output 0 and return
    if (order_list.empty()) {
        std::cout << "0" << std::endl;
        return 0;
    }

    // Output count
    std::cout << order_list.size() << std::endl;

    // Sort by timestamp to ensure correct order (fix for issue #40)
    // Use stable_sort to maintain deterministic ordering for timestamp collisions
    std::stable_sort(order_list.begin(), order_list.end(), [](const Order& a, const Order& b) {
        return a.timestamp < b.timestamp;
    });

    // Reverse to get newest-to-oldest
    std::reverse(order_list.begin(), order_list.end());

    // Output each order with calculated times
    for (const Order& order : order_list) {
        // Get train details
        TrainKey key(order.trainID);
        Train* train = trains.find(key);

        // If train not found, skip this order (shouldn't happen in valid data)
        if (!train) {
            continue;
        }

        // Calculate leaving time at from_station
        // departure_date is the date the train departs from its starting station
        DateTime leaving_time(order.departure_date, train->startTime);

        // Add travel time to from_station
        int minutes_to_from = 0;
        for (int i = 0; i < order.from_idx; i++) {
            minutes_to_from += train->travelTimes[i];
            if (i > 0) {
                minutes_to_from += train->stopoverTimes[i - 1];
            }
        }
        // Add stopover time at from_station (if not the first station)
        if (order.from_idx > 0) {
            minutes_to_from += train->stopoverTimes[order.from_idx - 1];
        }
        leaving_time.addMinutes(minutes_to_from);

        // Calculate arriving time at to_station
        DateTime arriving_time(order.departure_date, train->startTime);
        int minutes_to_to = 0;
        for (int i = 0; i < order.to_idx; i++) {
            minutes_to_to += train->travelTimes[i];
            if (i > 0) {
                minutes_to_to += train->stopoverTimes[i - 1];
            }
        }
        arriving_time.addMinutes(minutes_to_to);

        // Format status
        const char* status_str;
        if (order.status == 's') {
            status_str = "success";
        } else if (order.status == 'p') {
            status_str = "pending";
        } else {
            status_str = "refunded";
        }

        // Format dates and times
        char leaving_date[6], leaving_time_str[6];
        char arriving_date[6], arriving_time_str[6];
        leaving_time.date.format(leaving_date);
        leaving_time.time.format(leaving_time_str);
        arriving_time.date.format(arriving_date);
        arriving_time.time.format(arriving_time_str);

        // Calculate single ticket price
        int single_ticket_price = order.total_price / order.ticket_count;

        // Output: [<STATUS>] <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <NUM>
        std::cout << "[" << status_str << "] "
                  << order.trainID << " "
                  << order.from_station << " "
                  << leaving_date << " " << leaving_time_str << " -> "
                  << order.to_station << " "
                  << arriving_date << " " << arriving_time_str << " "
                  << single_ticket_price << " "
                  << order.ticket_count << std::endl;
    }

    return 0;
}

int cmd_refund_ticket(const CommandParser& parser) {
    const char* username = parser.get('u');
    const char* order_num_str = parser.get('n');

    if (!username) {
        return -1;
    }

    // Default order number is 1
    int order_num = 1;
    if (order_num_str) {
        order_num = std::atoi(order_num_str);
        if (order_num < 1) {
            return -1;
        }
    }

    // Check if user is logged in
    bool* is_logged_in = logged_in_users.find(username);
    if (!is_logged_in || !(*is_logged_in)) {
        return -1;
    }

    // Check if user exists
    User* user = users.find(username);
    if (!user) {
        return -1;
    }

    // Collect all orders into a vector
    std::vector<Order> order_list;
    queryUserOrders(username, [&](const Order& order) {
        order_list.push_back(order);
    });

    // If no orders, return failure
    if (order_list.empty()) {
        return -1;
    }

    // Sort by timestamp to ensure correct order (fix for issue #40)
    // Use stable_sort to maintain deterministic ordering for timestamp collisions
    std::stable_sort(order_list.begin(), order_list.end(), [](const Order& a, const Order& b) {
        return a.timestamp < b.timestamp;
    });

    // Reverse to get newest-to-oldest
    std::reverse(order_list.begin(), order_list.end());

    // Check if order_num is valid
    if (order_num > (int)order_list.size()) {
        return -1;
    }

    // Get the n-th order (1-indexed)
    Order& target_order = order_list[order_num - 1];

    // Check if order is already refunded
    if (target_order.status == 'r') {
        return -1;
    }

    // If order was successful, release the seats
    if (target_order.status == 's') {
        releaseSeats(target_order.trainID, target_order.departure_date,
                    target_order.from_idx, target_order.to_idx, target_order.ticket_count);
    }

    // Update order status to refunded
    updateOrderStatus(username, target_order.timestamp, 'r');

    // Process standby queue to fulfill pending orders
    processStandbyQueue(target_order.trainID, target_order.departure_date);

    // Save order counter after creating/modifying orders
    saveOrderCounter();

    return 0;
}

int main() {
    // Load user data from disk at startup
    load_users();

    // Initialize order system (load order counter from disk)
    initOrderSystem();

    // Open train database
    trains.open("trains.dat");

    // Open order and seat databases
    orders.open("orders.dat");
    seats.open("seats.dat");

    std::string command;
    std::string line;

    while (std::cin >> command) {
        if (command == "exit") {
            std::cout << "bye" << std::endl;
            break;
        } else if (command == "clean") {
            cmd_clean();
            std::cout << "0" << std::endl;
        } else if (command == "add_user") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_add_user(parser);
            std::cout << result << std::endl;
        } else if (command == "login") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_login(parser);
            std::cout << result << std::endl;
        } else if (command == "logout") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_logout(parser);
            std::cout << result << std::endl;
        } else if (command == "query_profile") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            cmd_query_profile(parser);
        } else if (command == "modify_profile") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            cmd_modify_profile(parser);
        } else if (command == "add_train") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_add_train(parser);
            std::cout << result << std::endl;
        } else if (command == "delete_train") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_delete_train(parser);
            std::cout << result << std::endl;
        } else if (command == "release_train") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_release_train(parser);
            std::cout << result << std::endl;
        } else if (command == "query_train") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_query_train(parser);
            if (result == -1) {
                std::cout << "-1" << std::endl;
            }
        } else if (command == "query_ticket") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_query_ticket(parser);
            if (result == -1) {
                std::cout << "-1" << std::endl;
            }
        } else if (command == "buy_ticket") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_buy_ticket(parser);
            if (result == -1) {
                std::cout << "-1" << std::endl;
            }
        } else if (command == "query_order") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_query_order(parser);
            if (result == -1) {
                std::cout << "-1" << std::endl;
            }
        } else if (command == "refund_ticket") {
            std::getline(std::cin, line);
            CommandParser parser;
            parser.parse(line);
            int result = cmd_refund_ticket(parser);
            std::cout << result << std::endl;
        } else {
            // Unknown command or not yet implemented
            std::getline(std::cin, line);
            std::cout << "-1" << std::endl;
        }
    }

    return 0;
}
