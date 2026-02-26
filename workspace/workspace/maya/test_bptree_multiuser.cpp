// Test B+ tree with multiple users to check global ordering
#include "../../../bptree.hpp"
#include "../../../types.hpp"
#include <iostream>
#include <vector>
#include <string>

int main() {
    BPTree<OrderKey, Order, 32> tree;
    if (!tree.open("test_multiuser.dat")) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    tree.clear();

    const char* users[] = {"alice", "bob", "charlie", "diana", "eve"};
    const int num_users = 5;

    // Insert orders for multiple users, interleaved
    std::cout << "=== Inserting orders for " << num_users << " users ===" << std::endl;
    for (int t = 1; t <= 50; t++) {
        for (int u = 0; u < num_users; u++) {
            OrderKey key(users[u], t * 100);
            Order order;
            order.timestamp = t * 100;
            strncpy(order.username, users[u], 24);
            order.username[24] = '\0';
            order.status = 's';
            order.ticket_count = t;
            tree.insert(key, order);
        }
    }

    // Check global ordering
    std::cout << "\n=== Checking global ordering ===" << std::endl;
    std::vector<std::pair<std::string, long long>> all_orders;
    tree.forEach([&](const OrderKey& key, const Order& order) {
        all_orders.push_back({std::string(key.username), order.timestamp});
    });

    bool globally_sorted = true;
    for (size_t i = 1; i < all_orders.size(); i++) {
        const auto& prev = all_orders[i-1];
        const auto& curr = all_orders[i];

        // Check if properly sorted (first by username, then by timestamp)
        if (prev.first > curr.first) {
            std::cout << "ERROR: Username out of order at position " << i << ": "
                      << prev.first << " > " << curr.first << std::endl;
            globally_sorted = false;
        } else if (prev.first == curr.first && prev.second > curr.second) {
            std::cout << "ERROR: Timestamp out of order at position " << i << ": "
                      << prev.first << ":" << prev.second << " > " << curr.second << std::endl;
            globally_sorted = false;
        }
    }

    if (globally_sorted) {
        std::cout << "Global ordering: OK" << std::endl;
    } else {
        std::cout << "Global ordering: FAILED" << std::endl;
        std::cout << "First 50 orders:" << std::endl;
        for (size_t i = 0; i < std::min((size_t)50, all_orders.size()); i++) {
            std::cout << "  " << all_orders[i].first << ":" << all_orders[i].second << std::endl;
        }
        return 1;
    }

    // Now update some orders and recheck
    std::cout << "\n=== Updating orders for alice and charlie ===" << std::endl;
    for (int t = 10; t <= 40; t += 5) {
        for (const char* user : {"alice", "charlie"}) {
            OrderKey key(user, t * 100);
            Order* order_ptr = tree.find(key);
            if (order_ptr) {
                Order order_copy = *order_ptr;
                order_copy.status = 'r';

                tree.remove(key);
                tree.insert(key, order_copy);
            }
        }
    }

    // Recheck global ordering
    std::cout << "\n=== Rechecking global ordering after updates ===" << std::endl;
    all_orders.clear();
    tree.forEach([&](const OrderKey& key, const Order& order) {
        all_orders.push_back({std::string(key.username), order.timestamp});
    });

    globally_sorted = true;
    for (size_t i = 1; i < all_orders.size(); i++) {
        const auto& prev = all_orders[i-1];
        const auto& curr = all_orders[i];

        if (prev.first > curr.first) {
            std::cout << "ERROR: Username out of order at position " << i << ": "
                      << prev.first << " > " << curr.first << std::endl;
            globally_sorted = false;
        } else if (prev.first == curr.first && prev.second > curr.second) {
            std::cout << "ERROR: Timestamp out of order at position " << i << ": "
                      << prev.first << ":" << prev.second << " > " << curr.second << std::endl;
            globally_sorted = false;
        }
    }

    if (globally_sorted) {
        std::cout << "Global ordering after updates: OK" << std::endl;
        std::cout << "\n=== SUCCESS: All tests passed ===" << std::endl;
        tree.close();
        return 0;
    } else {
        std::cout << "Global ordering after updates: FAILED" << std::endl;
        tree.close();
        return 1;
    }
}
