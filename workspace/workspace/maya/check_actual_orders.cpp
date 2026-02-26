// Check if actual orders.dat file has ordering issues
#include "../../../bptree.hpp"
#include "../../../types.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

int main() {
    BPTree<OrderKey, Order, 32> orders;

    if (!orders.open("../../../orders.dat")) {
        std::cerr << "Failed to open orders.dat - make sure you run this after running tests" << std::endl;
        return 1;
    }

    // Collect all orders per user
    std::map<std::string, std::vector<long long>> user_timestamps;

    orders.forEach([&](const OrderKey& key, const Order& order) {
        user_timestamps[std::string(key.username)].push_back(order.timestamp);
    });

    std::cout << "Found " << user_timestamps.size() << " users with orders" << std::endl;

    // Check ordering for each user
    int total_users = 0;
    int users_with_errors = 0;
    int total_errors = 0;

    for (const auto& pair : user_timestamps) {
        const std::string& username = pair.first;
        const std::vector<long long>& timestamps = pair.second;

        total_users++;

        bool sorted = std::is_sorted(timestamps.begin(), timestamps.end());
        if (!sorted) {
            users_with_errors++;

            // Count how many out-of-order pairs
            int errors_for_user = 0;
            for (size_t i = 1; i < timestamps.size(); i++) {
                if (timestamps[i] < timestamps[i-1]) {
                    errors_for_user++;
                    if (errors_for_user <= 5) {  // Show first 5 errors
                        std::cout << "User " << username << ": timestamp[" << (i-1) << "]="
                                  << timestamps[i-1] << " > timestamp[" << i << "]="
                                  << timestamps[i] << std::endl;
                    }
                }
            }
            total_errors += errors_for_user;

            std::cout << "User " << username << " has " << errors_for_user
                      << " out-of-order timestamp pairs (out of " << timestamps.size() << " orders)" << std::endl;
        }
    }

    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Total users: " << total_users << std::endl;
    std::cout << "Users with ordering errors: " << users_with_errors << std::endl;
    std::cout << "Total out-of-order pairs: " << total_errors << std::endl;

    if (users_with_errors == 0) {
        std::cout << "SUCCESS: All orders are correctly sorted!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Ordering errors detected!" << std::endl;
        return 1;
    }
}
