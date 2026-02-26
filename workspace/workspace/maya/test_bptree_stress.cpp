// Stress test to investigate B+ tree ordering issue
#include "../../../bptree.hpp"
#include "../../../types.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

void printOrders(BPTree<OrderKey, Order, 32>& tree, const char* username) {
    std::vector<long long> timestamps;
    tree.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            timestamps.push_back(order.timestamp);
        }
    });

    std::cout << "Orders for " << username << " (" << timestamps.size() << " total): ";
    for (auto ts : timestamps) {
        std::cout << ts << " ";
    }
    std::cout << std::endl;

    // Check if sorted
    bool sorted = std::is_sorted(timestamps.begin(), timestamps.end());
    if (!sorted) {
        std::cout << "ERROR: Orders are NOT sorted!" << std::endl;
    }
}

bool checkOrdering(BPTree<OrderKey, Order, 32>& tree, const char* username) {
    std::vector<long long> timestamps;
    tree.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            timestamps.push_back(order.timestamp);
        }
    });

    return std::is_sorted(timestamps.begin(), timestamps.end());
}

int main() {
    // Use same degree as actual orders B+ tree
    BPTree<OrderKey, Order, 32> test_tree;  // MAX_KEYS = 31

    if (!test_tree.open("test_stress.dat")) {
        std::cerr << "Failed to open test file" << std::endl;
        return 1;
    }

    test_tree.clear();

    const char* username = "testuser";

    std::srand(12345);  // Fixed seed for reproducibility

    // Phase 1: Insert 100 orders
    std::cout << "=== Phase 1: Inserting 100 orders ===" << std::endl;
    for (int i = 1; i <= 100; i++) {
        OrderKey key(username, i * 1000);
        Order order;
        order.timestamp = i * 1000;
        strncpy(order.username, username, 24);
        order.username[24] = '\0';
        order.status = 's';
        order.ticket_count = i;
        test_tree.insert(key, order);
    }

    if (!checkOrdering(test_tree, username)) {
        std::cout << "FAILURE after Phase 1!" << std::endl;
        printOrders(test_tree, username);
        return 1;
    }
    std::cout << "Phase 1: OK - all orders in correct order" << std::endl;

    // Phase 2: Update status of 50 random orders (delete-then-insert)
    std::cout << "\n=== Phase 2: Updating 50 random orders ===" << std::endl;
    std::vector<int> update_indices;
    for (int i = 1; i <= 100; i++) {
        update_indices.push_back(i);
    }
    // Just take every other element for simplicity
    std::vector<int> selected;
    for (size_t i = 0; i < update_indices.size() && selected.size() < 50; i += 2) {
        selected.push_back(update_indices[i]);
    }
    update_indices = selected;

    for (int idx : update_indices) {
        OrderKey key(username, idx * 1000);
        Order* order_ptr = test_tree.find(key);
        if (order_ptr) {
            Order order_copy = *order_ptr;
            order_copy.status = 'r';  // Refund

            test_tree.remove(key);
            test_tree.insert(key, order_copy);
        }
    }

    if (!checkOrdering(test_tree, username)) {
        std::cout << "FAILURE after Phase 2!" << std::endl;
        printOrders(test_tree, username);
        return 1;
    }
    std::cout << "Phase 2: OK - all orders still in correct order" << std::endl;

    // Phase 3: Insert more orders interleaved
    std::cout << "\n=== Phase 3: Inserting 50 more orders ===" << std::endl;
    for (int i = 101; i <= 150; i++) {
        OrderKey key(username, i * 1000);
        Order order;
        order.timestamp = i * 1000;
        strncpy(order.username, username, 24);
        order.username[24] = '\0';
        order.status = 's';
        order.ticket_count = i;
        test_tree.insert(key, order);
    }

    if (!checkOrdering(test_tree, username)) {
        std::cout << "FAILURE after Phase 3!" << std::endl;
        printOrders(test_tree, username);
        return 1;
    }
    std::cout << "Phase 3: OK - all 150 orders in correct order" << std::endl;

    // Phase 4: Update more orders
    std::cout << "\n=== Phase 4: Updating another 75 orders ===" << std::endl;
    update_indices.clear();
    for (int i = 1; i <= 150; i++) {
        update_indices.push_back(i);
    }
    // Just take every other element for simplicity
    selected.clear();
    for (size_t i = 0; i < update_indices.size() && selected.size() < 75; i += 2) {
        selected.push_back(update_indices[i]);
    }
    update_indices = selected;

    for (int idx : update_indices) {
        OrderKey key(username, idx * 1000);
        Order* order_ptr = test_tree.find(key);
        if (order_ptr) {
            Order order_copy = *order_ptr;
            order_copy.status = (order_copy.status == 's') ? 'r' : 's';

            test_tree.remove(key);
            test_tree.insert(key, order_copy);
        }
    }

    if (!checkOrdering(test_tree, username)) {
        std::cout << "FAILURE after Phase 4!" << std::endl;
        printOrders(test_tree, username);
        return 1;
    }
    std::cout << "Phase 4: OK - all 150 orders still in correct order" << std::endl;

    // Final verification
    std::cout << "\n=== Final Verification ===" << std::endl;
    printOrders(test_tree, username);

    test_tree.close();
    std::cout << "\n=== SUCCESS: All phases passed ===" << std::endl;
    return 0;
}
