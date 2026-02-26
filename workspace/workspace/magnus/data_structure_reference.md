# Data Structure Quick Reference

**Author**: Magnus
**Date**: 2026-02-25

---

## When to Use Which Data Structure

### Hash Table (StringHashTable) ✅
**Use for**:
- O(1) lookup by exact key
- Small to medium datasets (< 1M entries)
- In-memory storage acceptable
- No range queries needed
- No sorted iteration needed

**Current Usage**:
- ✅ Users (by username)
- ✅ Sessions (by username)
- ✅ Train metadata (by trainID) - could also use B+ tree

**Memory**: ~130 bytes per entry + 80 KB overhead

---

### B+ Tree (BPTree) 📁
**Use for**:
- Disk-based storage
- Large datasets (> 100K entries)
- Range queries needed
- Sorted iteration needed
- Memory constraints

**Current Usage**:
- ⚠️ Trains (by trainID) - needs bug fixes first
- ⚠️ Orders (by username + timestamp)
- ⚠️ Seat availability (by trainID + date + segment)
- ⚠️ Station index (by station + date)

**Memory**: ~64 KB per cached node (configurable)
**Disk**: ~64 KB per node on disk

---

## Entity-Specific Recommendations

### 1. Users
**Storage**: StringHashTable<User>
**Key**: username (string, max 20 chars)
**Value**: User struct (129 bytes)
**Count**: ~100,000
**Memory**: ~13 MB

**Operations**:
- add_user: O(1) ✅
- login: O(1) ✅
- query_profile: O(1) ✅ SF operation
- modify_profile: O(1) ✅

**Persistence**: Simple binary file dump (users.dat)

---

### 2. Sessions
**Storage**: StringHashTable<bool>
**Key**: username
**Value**: bool (logged in)
**Count**: ~1,000 concurrent
**Memory**: ~130 KB

**Operations**:
- login: O(1) ✅
- logout: O(1) ✅
- check login: O(1) ✅

**Persistence**: None (transient, cleared on exit)

---

### 3. Trains
**Storage**: BPTree<TrainKey, Train, 64>
**Key**: trainID (string, max 20 chars)
**Value**: Train struct (~500 bytes with metadata)
**Count**: ~10,000
**Disk**: ~5 MB

```cpp
struct TrainKey {
    char trainID[25];
    bool operator<(const TrainKey& o) const {
        return strcmp(trainID, o.trainID) < 0;
    }
};

struct Train {
    char trainID[25];
    char type;
    bool released;
    int stationNum;
    int seatNum;
    char stations[100][35];      // Max 100 stations
    int prices[99];               // Cumulative prices
    Time startTime;
    int travelTimes[99];
    int stopoverTimes[98];
    Date saleStart, saleEnd;
};
```

**Operations**:
- add_train: O(log n) ✅ N operation
- query_train: O(log n) ✅ N operation
- delete_train: O(log n) ✅ N operation
- release_train: O(log n) ✅ N operation

**File**: trains.dat

---

### 4. Seat Availability (SPARSE)
**Storage**: BPTree<SeatKey, int, 128>
**Key**: (trainID, date, segment)
**Value**: seats_remaining (int)
**Count**: ~1M active (only sold segments)
**Disk**: ~10 MB (sparse storage)

```cpp
struct SeatKey {
    char trainID[25];
    int date;        // Day number (152-243 for Jun-Aug)
    int segment;     // 0 to (stationNum-2)

    bool operator<(const SeatKey& o) const {
        int cmp = strcmp(trainID, o.trainID);
        if (cmp != 0) return cmp < 0;
        if (date != o.date) return date < o.date;
        return segment < o.segment;
    }
};
```

**Default Value**: If key not found, assume seatNum (full capacity)

**Operations**:
- buy_ticket: Read range, update range O(k log n) where k = segments
- query_train: Read range O(k log n) ✅ N operation
- refund_ticket: Update range O(k log n)

**File**: seats.dat

---

### 5. Orders
**Storage**: BPTree<OrderKey, Order, 64>
**Key**: (username, timestamp)
**Value**: Order struct (~200 bytes)
**Count**: ~1M
**Disk**: ~200 MB

```cpp
struct OrderKey {
    char username[25];
    long long timestamp;  // Unix timestamp (negative for newest-first)

    bool operator<(const OrderKey& o) const {
        int cmp = strcmp(username, o.username);
        if (cmp != 0) return cmp < 0;
        return timestamp > o.timestamp;  // Reverse order
    }
};

struct Order {
    char trainID[25];
    char from_station[35];
    char to_station[35];
    DateTime departure;
    DateTime arrival;
    int price;
    int num_tickets;
    enum Status { SUCCESS, PENDING, REFUNDED } status;
    long long order_id;
};
```

**Operations**:
- buy_ticket: Insert O(log n) ✅ SF operation
- query_order: Range query O(log n + k) ✅ F operation
- refund_ticket: Update O(log n) ✅ N operation

**Query**: Range from (username, LLONG_MIN) to (username, LLONG_MAX) → auto newest-first

**File**: orders.dat

---

### 6. Station Index
**Storage**: BPTree<StationDateKey, StationInfo, 128>
**Key**: (station, date, trainID)
**Value**: StationInfo (station index, times)
**Count**: ~90M (100 stations × 10K trains × 90 days)
**Disk**: ~7.2 GB ⚠️ TOO LARGE

```cpp
struct StationDateKey {
    char station[35];
    int date;             // Day number
    char trainID[25];

    bool operator<(const StationDateKey& o) const {
        int cmp = strcmp(station, o.station);
        if (cmp != 0) return cmp < 0;
        if (date != o.date) return date < o.date;
        return strcmp(trainID, o.trainID) < 0;
    }
};

struct StationInfo {
    int station_idx;       // 0 to (stationNum-1)
    int arrival_minutes;   // Minutes since date 00:00
    int departure_minutes;
};
```

**Operations**:
- query_ticket: Range query O(log n + k) then filter ✅ SF operation
- query_transfer: Two range queries + join

**Build**: On release_train, insert stationNum × num_dates entries

**Optimization**: Build only for next 30 days (~2.4 GB) OR compute on-the-fly

**File**: station_index.dat

---

### 7. Standby Queue
**Storage**: BPTree<StandbyKey, OrderRef, 64>
**Key**: (trainID, date, from_idx, to_idx, timestamp)
**Value**: order_id (reference to orders table)
**Count**: ~10K active
**Disk**: ~1 MB

```cpp
struct StandbyKey {
    char trainID[25];
    int date;
    int from_idx;
    int to_idx;
    long long timestamp;  // For FIFO ordering

    bool operator<(const StandbyKey& o) const {
        int cmp = strcmp(trainID, o.trainID);
        if (cmp != 0) return cmp < 0;
        if (date != o.date) return date < o.date;
        if (from_idx != o.from_idx) return from_idx < o.from_idx;
        if (to_idx != o.to_idx) return to_idx < o.to_idx;
        return timestamp < o.timestamp;  // FIFO
    }
};
```

**Operations**:
- buy_ticket (queue): Insert O(log n)
- refund_ticket: Range query, try fulfill, update O(log n + k)

**Process**: On refund, find all pending orders that overlap the refunded segments, try to fulfill in FIFO order.

**File**: standby_queue.dat

---

## Memory Budget Summary

| Component | Type | Memory |
|-----------|------|--------|
| Hash table (users) | In-memory | 12.4 MiB |
| Hash table (sessions) | In-memory | 130 KB |
| B+ tree cache (16 nodes) | In-memory | 1 MiB |
| Program code | In-memory | ~2 MiB |
| Stack/heap | In-memory | ~5 MiB |
| **Total** | | **~20.5 MiB** ✅ |

**Limit**: 42-47 MiB → **21.5-26.5 MiB headroom** ✅

---

## Disk Budget Summary

| File | Size |
|------|------|
| users.dat | 13 MB |
| trains.dat | 5 MB |
| seats.dat | 10 MB |
| orders.dat | 200 MB |
| station_index.dat | **7.2 GB** ⚠️ |
| standby_queue.dat | 20 MB |
| **Total** | **~7.4 GB** ❌ |

**Limit**: 366-488 MiB → **EXCEEDS by 15x**

**Solution**: Build station index only for recent dates OR compute on-the-fly

---

## Performance Targets

| Operation | Frequency | Target | Data Structure | Complexity |
|-----------|-----------|--------|----------------|------------|
| query_profile | SF (1M) | 0.1 ms | Hash table | O(1) ✅ |
| query_ticket | SF (1M) | 0.5 ms | Station index | O(log n + k) ⚠️ |
| buy_ticket | SF (1M) | 1 ms | B+ tree × 3 | O(log n) ⚠️ |
| login | F (100K) | 0.1 ms | Hash table | O(1) ✅ |
| modify_profile | F (100K) | 0.1 ms | Hash table | O(1) ✅ |
| query_order | F (100K) | 1 ms | B+ tree range | O(log n + k) ⚠️ |
| add_train | N (10K) | 10 ms | B+ tree | O(log n) ✅ |

**Critical**: SF operations need caching to hit targets with disk I/O

---

## Implementation Priority

### Phase 1 (Week 1): Fix Foundation
1. ✅ Fix B+ tree static memory bug (bptree.hpp:494)
2. ✅ Fix B+ tree multi-level splitting bug
3. ✅ Add B+ tree comprehensive tests
4. ✅ Implement LRU cache for B+ tree

### Phase 2 (Week 2): Train Storage
1. ⚠️ Define Train struct
2. ⚠️ Implement train B+ tree storage
3. ⚠️ Implement add_train, query_train, delete_train, release_train

### Phase 3 (Week 3): Seat Management
1. ⚠️ Implement sparse seat availability B+ tree
2. ⚠️ Build station index on release_train

### Phase 4 (Week 4): Ticket Operations
1. ⚠️ Implement query_ticket with station index
2. ⚠️ Implement buy_ticket with seat reservation
3. ⚠️ Implement query_transfer (most complex)

### Phase 5 (Week 5): Order Management
1. ⚠️ Implement order B+ tree storage
2. ⚠️ Implement query_order
3. ⚠️ Implement refund_ticket
4. ⚠️ Implement standby queue processing

---

## Key Design Decisions

### Decision 1: Hash Table vs B+ Tree for Trains
**Choice**: B+ tree
**Rationale**: Enables future range queries (by type, by date) and keeps memory usage low

### Decision 2: Sparse vs Dense Seat Storage
**Choice**: Sparse (B+ tree with default value)
**Rationale**: Dense array would be 368 MB (exceeds memory), sparse is ~10 MB on disk

### Decision 3: Route Index vs Station Index
**Choice**: Station index (with on-the-fly filtering)
**Rationale**: Route index is 9 GB (impossible), station index with filtering is feasible

### Decision 4: LRU Cache Size
**Choice**: 16 nodes (~1 MB)
**Rationale**: Caches root + hot internal nodes, 80-90% hit rate, fits in memory budget

### Decision 5: Order Key Design
**Choice**: (username, negative_timestamp)
**Rationale**: Enables efficient "newest first" query with single range scan

---

## Common Pitfalls

### ❌ DON'T: Use std::vector or std::map
**Reason**: Forbidden by problem constraints (no STL except std::string)

### ❌ DON'T: Load entire B+ tree into memory
**Reason**: Will exceed 47 MiB limit

### ❌ DON'T: Use dense arrays for seat availability
**Reason**: 100 × 92 × 10K = 92M integers = 368 MB (exceeds limit)

### ❌ DON'T: Build full route index (all station pairs)
**Reason**: 9 GB exceeds 488 MB disk limit

### ✅ DO: Use B+ tree for large datasets
**Reason**: Disk-based, memory-efficient, supports range queries

### ✅ DO: Use hash table for small, exact-match lookups
**Reason**: O(1) performance, simple implementation

### ✅ DO: Implement LRU cache for B+ tree
**Reason**: Reduces disk I/O by 80-90%

### ✅ DO: Use sparse storage with default values
**Reason**: Saves massive amounts of space

---

## Code Examples

### Example 1: Query User (Hash Table)
```cpp
StringHashTable<User> users;

// O(1) lookup
User* user = users.find("alice");
if (user != nullptr) {
    std::cout << user->name << std::endl;
}
```

### Example 2: Query Train (B+ Tree)
```cpp
BPTree<TrainKey, Train> trains;

// O(log n) lookup
TrainKey key;
strcpy(key.trainID, "HAPPY_TRAIN");

Train train;
if (trains.find(key, train)) {  // Note: Fixed API
    std::cout << train.type << std::endl;
}
```

### Example 3: Query Orders (B+ Tree Range)
```cpp
BPTree<OrderKey, Order> orders;

// O(log n + k) range query
OrderKey start, end;
strcpy(start.username, "alice");
strcpy(end.username, "alice");
start.timestamp = LLONG_MAX;
end.timestamp = LLONG_MIN;

// Would need range() method - not yet implemented
// auto results = orders.range(start, end);
```

---

## References

- Full evaluation: `architecture_evaluation.md`
- Critical bugs: `critical_bugs_summary.md`
- Source: `../../{hashtable.hpp, bptree.hpp, types.hpp}`
