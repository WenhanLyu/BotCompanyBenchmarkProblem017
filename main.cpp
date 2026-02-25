#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdio>
#include "types.hpp"
#include "hashtable.hpp"
#include "fileio.hpp"

// User database and session management
StringHashTable<User> users;
StringHashTable<bool> logged_in_users;  // Track logged-in users
int user_count = 0;  // Track total number of users

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
    char params[20][105];  // Max 20 parameters, each max 100 chars
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

    return 0;
}

int main() {
    // Load user data from disk at startup
    load_users();

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
        } else {
            // Unknown command or not yet implemented
            std::getline(std::cin, line);
            std::cout << "-1" << std::endl;
        }
    }

    return 0;
}
