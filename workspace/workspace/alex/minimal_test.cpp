#include "order_system.hpp"
#include <iostream>

// Define the global B+ trees
BPTree<OrderKey, Order, 32> orders("orders.dat");
BPTree<SeatKey, SeatAvailability, 16> seats("seats.dat");

int main() {
    // Test updateOrderStatus fix

    // Create a test order
    const char* username = "alice";
    long long timestamp = 12345;

    Order test_order;
    strcpy(test_order.username, username);
    strcpy(test_order.trainID, "T001");
    test_order.departure_date = Date(6, 15);
    strcpy(test_order.from_station, "Shanghai");
    strcpy(test_order.to_station, "Beijing");
    test_order.from_idx = 0;
    test_order.to_idx = 2;
    test_order.ticket_count = 2;
    test_order.total_price = 1000;
    test_order.status = 's';  // success
    test_order.timestamp = timestamp;

    // Insert the order
    OrderKey key(username, timestamp);
    orders.insert(key, test_order);

    std::cout << "Initial order status: " << test_order.status << std::endl;

    // Update the order status (this should trigger the fix)
    bool result = updateOrderStatus(username, timestamp, 'r');  // refunded

    std::cout << "Update result: " << (result ? "success" : "failed") << std::endl;

    // Verify the update
    Order* updated_order = orders.find(key);
    if (updated_order) {
        std::cout << "Updated order status: " << updated_order->status << std::endl;
        std::cout << "Order details preserved: trainID=" << updated_order->trainID
                  << " from=" << updated_order->from_station
                  << " to=" << updated_order->to_station << std::endl;
    } else {
        std::cout << "ERROR: Order not found after update!" << std::endl;
    }

    return 0;
}
