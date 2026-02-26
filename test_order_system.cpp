#include <iostream>
#include <cassert>
#include "types.hpp"
#include "bptree.hpp"
#include "order_system.hpp"

// Define the global B+ trees
BPTree<OrderKey, Order, 32> orders;
BPTree<SeatKey, SeatAvailability, 16> seats;

void test_order_creation() {
    std::cout << "Test 1: Order Creation..." << std::flush;

    Date date(6, 1);
    long long ts = createOrder("alice", "T1234", date, "Shanghai", "Beijing",
                               0, 5, 2, 500, 's');

    assert(ts > 0);

    OrderKey key("alice", ts);
    Order* order = orders.find(key);
    assert(order != nullptr);
    assert(strcmp(order->username, "alice") == 0);
    assert(strcmp(order->trainID, "T1234") == 0);
    assert(order->ticket_count == 2);
    assert(order->total_price == 500);
    assert(order->status == 's');

    std::cout << " PASS" << std::endl;
}

void test_seat_availability() {
    std::cout << "Test 2: Seat Availability..." << std::flush;

    // Create a train
    Train train;
    strncpy(train.trainID, "G1001", 24);
    train.stationNum = 5;
    train.seatNum = 100;
    train.released = true;
    train.saleDate[0] = Date(6, 1);
    train.saleDate[1] = Date(6, 3);

    // Initialize seats
    initializeSeatsForTrain(train);

    // Check initial availability
    Date date(6, 1);
    int available = checkAvailableSeats("G1001", date, 0, 4);
    assert(available == 100);

    // Reserve 20 seats
    bool success = reserveSeats("G1001", date, 0, 4, 20);
    assert(success);

    // Check updated availability
    available = checkAvailableSeats("G1001", date, 0, 4);
    assert(available == 80);

    // Reserve more than available (should fail)
    success = reserveSeats("G1001", date, 0, 4, 85);
    assert(!success);

    // Release 10 seats
    success = releaseSeats("G1001", date, 0, 4, 10);
    assert(success);

    // Check updated availability
    available = checkAvailableSeats("G1001", date, 0, 4);
    assert(available == 90);

    std::cout << " PASS" << std::endl;
}

void test_segment_seat_tracking() {
    std::cout << "Test 3: Segment Seat Tracking..." << std::flush;

    // Create a train with 4 stations (3 segments)
    Train train;
    strncpy(train.trainID, "D2002", 24);
    train.stationNum = 4;
    train.seatNum = 50;
    train.released = true;
    train.saleDate[0] = Date(6, 5);
    train.saleDate[1] = Date(6, 5);

    initializeSeatsForTrain(train);

    Date date(6, 5);

    // Reserve 10 seats from station 0 to 2 (segments 0-1)
    bool success = reserveSeats("D2002", date, 0, 2, 10);
    assert(success);

    // Check availability for full journey (0 to 3)
    // Should be min(40, 40, 50) = 40
    int available = checkAvailableSeats("D2002", date, 0, 3);
    assert(available == 40);

    // Reserve 30 seats from station 1 to 3 (segments 1-2)
    success = reserveSeats("D2002", date, 1, 3, 30);
    assert(success);

    // Now segment 0 has 40, segment 1 has 10, segment 2 has 20
    // Check availability for journey 0 to 3: min(40, 10, 20) = 10
    available = checkAvailableSeats("D2002", date, 0, 3);
    assert(available == 10);

    // Check availability for journey 0 to 2: min(40, 10) = 10
    available = checkAvailableSeats("D2002", date, 0, 2);
    assert(available == 10);

    // Check availability for journey 1 to 3: min(10, 20) = 10
    available = checkAvailableSeats("D2002", date, 1, 3);
    assert(available == 10);

    // Check availability for journey 2 to 3: 20
    available = checkAvailableSeats("D2002", date, 2, 3);
    assert(available == 20);

    std::cout << " PASS" << std::endl;
}

void test_query_user_orders() {
    std::cout << "Test 4: Query User Orders..." << std::flush;

    Date date(6, 10);

    // Create multiple orders for user "bob"
    long long ts1 = createOrder("bob", "T1111", date, "A", "B", 0, 2, 1, 100, 's');
    long long ts2 = createOrder("bob", "T2222", date, "C", "D", 1, 3, 2, 200, 'p');
    long long ts3 = createOrder("bob", "T3333", date, "E", "F", 0, 1, 3, 300, 's');

    // Query bob's orders
    int count = 0;
    queryUserOrders("bob", [&](const Order& order) {
        count++;
        assert(strcmp(order.username, "bob") == 0);

        if (order.timestamp == ts1) {
            assert(strcmp(order.trainID, "T1111") == 0);
            assert(order.ticket_count == 1);
            assert(order.status == 's');
        } else if (order.timestamp == ts2) {
            assert(strcmp(order.trainID, "T2222") == 0);
            assert(order.ticket_count == 2);
            assert(order.status == 'p');
        } else if (order.timestamp == ts3) {
            assert(strcmp(order.trainID, "T3333") == 0);
            assert(order.ticket_count == 3);
            assert(order.status == 's');
        }
    });

    assert(count >= 3);  // At least 3 orders

    std::cout << " PASS" << std::endl;
}

void test_order_status_update() {
    std::cout << "Test 5: Order Status Update..." << std::flush;

    Date date(6, 15);

    // Create a pending order
    long long ts = createOrder("charlie", "T9999", date, "X", "Y", 0, 1, 1, 50, 'p');

    // Verify it's pending
    OrderKey key("charlie", ts);
    Order* order = orders.find(key);
    assert(order != nullptr);
    assert(order->status == 'p');

    // Update to success
    bool success = updateOrderStatus("charlie", ts, 's');
    assert(success);

    // Verify updated
    order = orders.find(key);
    assert(order != nullptr);
    assert(order->status == 's');

    // Update to refunded
    success = updateOrderStatus("charlie", ts, 'r');
    assert(success);

    // Verify updated
    order = orders.find(key);
    assert(order != nullptr);
    assert(order->status == 'r');

    std::cout << " PASS" << std::endl;
}

void test_standby_queue() {
    std::cout << "Test 6: Standby Queue Processing..." << std::flush;

    // Create a train with limited seats
    Train train;
    strncpy(train.trainID, "K5555", 24);
    train.stationNum = 3;
    train.seatNum = 10;
    train.released = true;
    train.saleDate[0] = Date(6, 20);
    train.saleDate[1] = Date(6, 20);

    initializeSeatsForTrain(train);

    Date date(6, 20);

    // Create successful orders that fill the train
    createOrder("user1", "K5555", date, "A", "C", 0, 2, 10, 100, 's');
    reserveSeats("K5555", date, 0, 2, 10);

    // Create pending orders (no seats available)
    long long ts1 = createOrder("user2", "K5555", date, "A", "C", 0, 2, 5, 50, 'p');
    long long ts2 = createOrder("user3", "K5555", date, "A", "C", 0, 2, 3, 30, 'p');

    // Release 5 seats
    releaseSeats("K5555", date, 0, 2, 5);

    // Process standby queue
    processStandbyQueue("K5555", date);

    // Check if user2's order (5 seats) was fulfilled
    OrderKey key2("user2", ts1);
    Order* order2 = orders.find(key2);
    assert(order2 != nullptr);
    assert(order2->status == 's');  // Should be fulfilled

    // Check if user3's order (3 seats) is still pending (not enough seats after user2)
    OrderKey key3("user3", ts2);
    Order* order3 = orders.find(key3);
    assert(order3 != nullptr);
    assert(order3->status == 'p');  // Should still be pending

    std::cout << " PASS" << std::endl;
}

int main() {
    std::cout << "Order System Tests\n" << std::endl;

    // Open B+ tree files
    orders.open("test_orders.dat");
    seats.open("test_seats.dat");

    // Clear any existing data
    orders.clear();
    seats.clear();

    // Initialize order system
    initOrderSystem();

    // Run tests
    test_order_creation();
    test_seat_availability();
    test_segment_seat_tracking();
    test_query_user_orders();
    test_order_status_update();
    test_standby_queue();

    // Cleanup
    orders.close();
    seats.close();

    std::cout << "\nAll tests passed!" << std::endl;

    return 0;
}
