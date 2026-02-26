// Test to investigate B+ tree ordering issue with delete-then-insert pattern
#include "../../../bptree.hpp"
#include "../../../types.hpp"
#include <iostream>
#include <vector>

int main() {
    // Create a test B+ tree with small degree to force splits
    BPTree<OrderKey, Order, 8> test_tree;  // MAX_KEYS = 7

    if (!test_tree.open("test_ordering.dat")) {
        std::cerr << "Failed to open test file" << std::endl;
        return 1;
    }

    // Clear any existing data
    test_tree.clear();

    const char* username = "testuser";

    // Insert orders with sequential timestamps
    std::cout << "=== Inserting 20 orders ===" << std::endl;
    for (int i = 1; i <= 20; i++) {
        OrderKey key(username, i * 100);  // timestamps: 100, 200, 300, ..., 2000
        Order order;
        order.timestamp = i * 100;
        strncpy(order.username, username, 24);
        order.username[24] = '\0';
        order.status = 's';
        order.ticket_count = i;
        test_tree.insert(key, order);
        std::cout << "Inserted order with timestamp " << (i * 100) << std::endl;
    }

    // Now iterate to verify order
    std::cout << "\n=== Initial iteration (should be 100, 200, ..., 2000) ===" << std::endl;
    std::vector<long long> timestamps1;
    test_tree.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            timestamps1.push_back(order.timestamp);
            std::cout << order.timestamp << " ";
        }
    });
    std::cout << std::endl;

    // Now simulate updateOrderStatus: delete and re-insert orders 5, 10, 15
    std::cout << "\n=== Simulating updateOrderStatus on orders 5, 10, 15 ===" << std::endl;
    for (int i : {5, 10, 15}) {
        OrderKey key(username, i * 100);
        Order* order_ptr = test_tree.find(key);
        if (order_ptr) {
            Order order_copy = *order_ptr;
            order_copy.status = 'r';  // Change status to refunded

            // Delete then insert (as done in updateOrderStatus)
            test_tree.remove(key);
            test_tree.insert(key, order_copy);
            std::cout << "Updated order with timestamp " << (i * 100) << std::endl;
        }
    }

    // Iterate again to check if order is maintained
    std::cout << "\n=== After update (should still be 100, 200, ..., 2000) ===" << std::endl;
    std::vector<long long> timestamps2;
    test_tree.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            timestamps2.push_back(order.timestamp);
            std::cout << order.timestamp << " ";
        }
    });
    std::cout << std::endl;

    // Check if order changed
    std::cout << "\n=== Verification ===" << std::endl;
    bool order_correct = true;
    for (size_t i = 0; i < timestamps2.size(); i++) {
        if (i + 1 > 0 && timestamps2[i] != (long long)((i + 1) * 100)) {
            std::cout << "ERROR: Position " << i << " has timestamp " << timestamps2[i]
                      << " but expected " << ((i + 1) * 100) << std::endl;
            order_correct = false;
        }
    }

    if (order_correct && timestamps2.size() == 20) {
        std::cout << "SUCCESS: Order maintained correctly after delete-then-insert" << std::endl;
    } else {
        std::cout << "FAILURE: Order corrupted after delete-then-insert" << std::endl;
    }

    test_tree.close();
    return order_correct ? 0 : 1;
}
