#ifndef ORDER_SYSTEM_HPP
#define ORDER_SYSTEM_HPP

#include "types.hpp"
#include "bptree.hpp"
#include <fstream>

/**
 * Order and Seat Management System
 *
 * Provides functions for:
 * - Order creation and tracking
 * - Seat availability management per train/date/segment
 * - Order persistence using B+ trees
 */

// Global order counter for timestamp generation
static long long g_order_counter = 0;

// B+ Trees for persistence
extern BPTree<OrderKey, Order, 32> orders;           // Orders indexed by (username, timestamp)
extern BPTree<SeatKey, SeatAvailability, 16> seats;  // Seat availability by (trainID, date)

// Initialize order system (load counter from disk)
inline void initOrderSystem() {
    std::ifstream in("order_counter.dat", std::ios::binary);
    if (in) {
        in.read(reinterpret_cast<char*>(&g_order_counter), sizeof(long long));
        in.close();
    } else {
        g_order_counter = 0;
    }
}

// Save order counter to disk
inline void saveOrderCounter() {
    std::ofstream out("order_counter.dat", std::ios::binary);
    if (out) {
        out.write(reinterpret_cast<const char*>(&g_order_counter), sizeof(long long));
        out.close();
    }
}

// Get next timestamp for ordering
inline long long getNextTimestamp() {
    return ++g_order_counter;
}

/**
 * Initialize seat availability for a train when it's released
 * Creates SeatAvailability entries for all dates in the train's sale range
 */
inline void initializeSeatsForTrain(const Train& train) {
    if (!train.released) return;

    // Create seat availability for each date in sale range
    Date current_date = train.saleDate[0];
    Date end_date = train.saleDate[1];

    while (current_date <= end_date) {
        SeatKey key(train.trainID, current_date);

        // Check if already exists
        SeatAvailability* existing = seats.find(key);
        if (!existing) {
            // Create new seat availability entry
            int num_segments = train.stationNum - 1;
            SeatAvailability seat_data(train.trainID, current_date, train.seatNum, num_segments);
            seats.insert(key, seat_data);
        }

        // Move to next date
        const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        current_date.day++;
        if (current_date.day > days_in_month[current_date.month]) {
            current_date.day = 1;
            current_date.month++;
            if (current_date.month > 12) {
                current_date.month = 1;
            }
        }
    }
}

/**
 * Check available seats for a journey segment
 * Returns the minimum available seats across all segments in the journey
 */
inline int checkAvailableSeats(const char* trainID, const Date& date,
                               int from_idx, int to_idx) {
    SeatKey key(trainID, date);
    SeatAvailability* seat_data = seats.find(key);

    if (!seat_data) {
        return 0;  // No seat data available
    }

    // Find minimum available seats across all segments
    int min_available = seat_data->available[from_idx];
    for (int i = from_idx + 1; i < to_idx; i++) {
        if (seat_data->available[i] < min_available) {
            min_available = seat_data->available[i];
        }
    }

    return min_available;
}

/**
 * Reserve seats for a journey
 * Decreases available seats for all segments in the journey
 * Returns true if successful
 */
inline bool reserveSeats(const char* trainID, const Date& date,
                        int from_idx, int to_idx, int count) {
    SeatKey key(trainID, date);
    SeatAvailability* seat_data = seats.find(key);

    if (!seat_data) {
        return false;
    }

    // Check if enough seats available
    for (int i = from_idx; i < to_idx; i++) {
        if (seat_data->available[i] < count) {
            return false;
        }
    }

    // Reserve seats
    for (int i = from_idx; i < to_idx; i++) {
        seat_data->available[i] -= count;
    }

    // Update in B+ tree
    seats.insert(key, *seat_data);

    return true;
}

/**
 * Release seats for a journey (for refund)
 * Increases available seats for all segments in the journey
 * Returns true if successful
 */
inline bool releaseSeats(const char* trainID, const Date& date,
                        int from_idx, int to_idx, int count) {
    SeatKey key(trainID, date);
    SeatAvailability* seat_data = seats.find(key);

    if (!seat_data) {
        return false;
    }

    // Release seats
    for (int i = from_idx; i < to_idx; i++) {
        seat_data->available[i] += count;
    }

    // Update in B+ tree
    seats.insert(key, *seat_data);

    return true;
}

/**
 * Create an order
 * Returns the timestamp of the created order
 */
inline long long createOrder(const char* username, const char* trainID,
                            const Date& date, const char* from_station,
                            const char* to_station, int from_idx, int to_idx,
                            int count, int price, char status) {
    long long timestamp = getNextTimestamp();

    Order order(username, trainID, date, from_station, to_station,
                from_idx, to_idx, count, price, status, timestamp);

    OrderKey key(username, timestamp);
    orders.insert(key, order);

    return timestamp;
}

/**
 * Query all orders for a user
 * Calls the callback function for each order
 */
template<typename Func>
inline void queryUserOrders(const char* username, Func callback) {
    // Create range for this user
    OrderKey start(username, 0);
    OrderKey end(username, 0x7FFFFFFFFFFFFFFFLL);  // Max long long

    // Iterate through orders
    orders.forEach([&](const OrderKey& key, const Order& order) {
        if (strcmp(key.username, username) == 0) {
            callback(order);
        }
    });
}

/**
 * Find a specific order by username, trainID, from_station, to_station
 * Returns pointer to order if found, nullptr otherwise
 * Note: The returned pointer is only valid until the next B+ tree operation
 */
inline Order* findOrder(const char* username, const char* trainID,
                       const char* from_station, const char* to_station) {
    Order* found_order = nullptr;

    orders.forEach([&](const OrderKey& key, Order& order) {
        if (found_order) return;  // Already found

        if (strcmp(key.username, username) == 0 &&
            strcmp(order.trainID, trainID) == 0 &&
            strcmp(order.from_station, from_station) == 0 &&
            strcmp(order.to_station, to_station) == 0 &&
            order.status != 'r') {  // Not refunded
            found_order = &order;
        }
    });

    return found_order;
}

/**
 * Update an order's status
 * Returns true if successful
 */
inline bool updateOrderStatus(const char* username, long long timestamp, char new_status) {
    OrderKey key(username, timestamp);
    Order* order = orders.find(key);

    if (!order) {
        return false;
    }

    order->status = new_status;
    orders.insert(key, *order);

    return true;
}

/**
 * Process standby queue for a train/date
 * Tries to fulfill pending orders after a refund
 */
inline void processStandbyQueue(const char* trainID, const Date& date) {
    // Collect all pending orders for this train and date
    struct PendingOrder {
        OrderKey key;
        Order order;
    };

    PendingOrder pending[1000];
    int pending_count = 0;

    orders.forEach([&](const OrderKey& key, const Order& order) {
        if (pending_count >= 1000) return;

        if (strcmp(order.trainID, trainID) == 0 &&
            order.departure_date == date &&
            order.status == 'p') {
            pending[pending_count].key = key;
            pending[pending_count].order = order;
            pending_count++;
        }
    });

    // Try to fulfill pending orders in timestamp order (already sorted by forEach)
    for (int i = 0; i < pending_count; i++) {
        const Order& order = pending[i].order;

        // Check if enough seats available
        int available = checkAvailableSeats(trainID, date, order.from_idx, order.to_idx);

        if (available >= order.ticket_count) {
            // Fulfill this pending order
            if (reserveSeats(trainID, date, order.from_idx, order.to_idx, order.ticket_count)) {
                updateOrderStatus(order.username, order.timestamp, 's');
            }
        }
    }
}

#endif // ORDER_SYSTEM_HPP
