// Test B+ tree persistence - simulate sequential test runs
#include "../../../bptree.hpp"
#include "../../../types.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

bool checkOrdering(BPTree<OrderKey, Order, 32>& tree, const char* username) {
    std::vector<long long> timestamps;
    tree.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            timestamps.push_back(order.timestamp);
        }
    });

    bool sorted = std::is_sorted(timestamps.begin(), timestamps.end());
    if (!sorted) {
        std::cout << "ERROR: Orders NOT sorted. Sequence: ";
        for (size_t i = 0; i < std::min(timestamps.size(), (size_t)20); i++) {
            std::cout << timestamps[i] << " ";
        }
        std::cout << "..." << std::endl;
    }
    return sorted;
}

void runTest(int test_num, int start_timestamp, int end_timestamp, int num_updates) {
    const char* username = "testuser";

    std::cout << "\n=== Test " << test_num << " ===" << std::endl;

    // Open existing file
    BPTree<OrderKey, Order, 32> tree;
    if (!tree.open("test_persist.dat")) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    // Add new orders
    std::cout << "Adding orders " << start_timestamp << " to " << end_timestamp << std::endl;
    for (int i = start_timestamp; i <= end_timestamp; i += 100) {
        OrderKey key(username, i);
        Order order;
        order.timestamp = i;
        strncpy(order.username, username, 24);
        order.username[24] = '\0';
        order.status = 's';
        order.ticket_count = i / 100;
        tree.insert(key, order);
    }

    // Update some orders (delete-then-insert)
    std::cout << "Updating " << num_updates << " orders" << std::endl;
    for (int i = 0; i < num_updates; i++) {
        long long ts = start_timestamp + (i * 500);
        if (ts > end_timestamp) break;

        OrderKey key(username, ts);
        Order* order_ptr = tree.find(key);
        if (order_ptr) {
            Order order_copy = *order_ptr;
            order_copy.status = 'r';

            tree.remove(key);
            tree.insert(key, order_copy);
        }
    }

    // Check ordering
    if (!checkOrdering(tree, username)) {
        std::cout << "FAILURE in test " << test_num << "!" << std::endl;
    } else {
        std::cout << "Test " << test_num << " PASSED - ordering correct" << std::endl;
    }

    // Close to save
    tree.close();
}

int main() {
    const char* username = "testuser";

    // Initialize with clean file
    {
        BPTree<OrderKey, Order, 32> tree;
        if (!tree.open("test_persist.dat")) {
            std::cerr << "Failed to open file" << std::endl;
            return 1;
        }
        tree.clear();
        tree.close();
    }

    // Simulate sequential test runs (like basic_3 tests 1-5)
    runTest(1, 100, 2000, 5);      // Test 1: small dataset
    runTest(2, 2100, 4000, 10);    // Test 2: add more, update more
    runTest(3, 4100, 6000, 15);    // Test 3: keep adding
    runTest(4, 6100, 8000, 20);    // Test 4: more operations
    runTest(5, 8100, 10000, 25);   // Test 5: even more

    // Final check after all tests
    std::cout << "\n=== Final Check ===" << std::endl;
    {
        BPTree<OrderKey, Order, 32> tree;
        if (!tree.open("test_persist.dat")) {
            std::cerr << "Failed to open file" << std::endl;
            return 1;
        }

        if (!checkOrdering(tree, username)) {
            std::cout << "FINAL CHECK FAILED!" << std::endl;
            return 1;
        }

        std::cout << "FINAL CHECK PASSED - all orders in correct sequence across all tests" << std::endl;
        tree.close();
    }

    std::cout << "\n=== SUCCESS: All sequential tests passed ===" << std::endl;
    return 0;
}
