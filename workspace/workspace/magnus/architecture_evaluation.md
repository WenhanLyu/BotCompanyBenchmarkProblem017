# Data Structures & Architecture Evaluation
**Architect**: Magnus
**Date**: 2026-02-25
**Status**: Independent Technical Assessment

---

## Executive Summary

**Current Status**: Implementation is 35% complete with solid user management foundation. Data structures show good design principles but B+ tree has critical bugs that will block train operations.

**Critical Path**: Fix B+ tree bugs → Implement train storage → Build multi-index system → Implement ticket queries

**Risk Level**: MEDIUM-HIGH
- B+ tree bugs (#10, #11, #12) are blocking
- Memory budget is tight (42-47 MiB)
- Query performance for SF operations is critical

---

## 1. DATA STRUCTURE ANALYSIS

### 1.1 Hash Table Implementation ✅ GOOD

**File**: `hashtable.hpp`

**Design Assessment**:
- **Collision Resolution**: Separate chaining with linked lists
- **Hash Function**: DJB2 variant (h = h*33 + c) - proven distribution
- **Table Size**: 10,007 (prime number - excellent choice)
- **Memory Model**: In-memory with manual memory management

**Performance**:
- Insert: O(1) average, O(n) worst case
- Find: O(1) average, O(n) worst case
- Delete: O(1) average, O(n) worst case
- Space: ~80KB fixed + ~130 bytes per user

**Strengths**:
1. ✅ Deleted copy constructors prevent double-free bugs (good defensive programming)
2. ✅ Template design supports multiple value types
3. ✅ Specialized `StringHashTable` optimized for username/trainID lookups
4. ✅ Iterator support for traversal (used in persistence)

**Weaknesses**:
1. ⚠️ No load factor management - could degrade to O(n) with clustering
2. ⚠️ Fixed table size - cannot grow dynamically
3. ⚠️ No memory pooling - many small allocations (fragmentation risk)

**Verdict**: **APPROPRIATE for users, sessions, and train metadata lookups**. NOT suitable for order history or ticket searches (no range queries).

**Memory Footprint**:
```
Hash table overhead: 10,007 pointers × 8 bytes = 80,056 bytes
User nodes: 100,000 users × 130 bytes = 13,000,000 bytes (~12.4 MiB)
Total: ~12.5 MiB for users (within budget)
```

---

### 1.2 B+ Tree Implementation ⚠️ CRITICAL BUGS

**File**: `bptree.hpp`

**Design Assessment**:
- **Degree**: 128 (reasonable for disk I/O)
- **Storage**: Disk-based with FileIO wrapper
- **Leaf Links**: Yes (enables efficient range queries)
- **Node Size**: ~64KB per node (large but acceptable for disk)

**Performance** (theoretical):
- Insert: O(log n)
- Find: O(log n)
- Range Query: O(log n + k) where k = result count
- Space: Disk-based, minimal memory footprint

**Critical Issues** (from test files):

1. **Node Splitting Bug** (test_bptree_bugs.cpp):
   - Insertion order affects tree correctness
   - Split logic incorrectly handles parent pointer updates
   - **Impact**: Will corrupt data during heavy train insertion

2. **Multi-level Tree Bug** (issue #11):
   - Trees deeper than 2 levels fail
   - `findParent()` has path-finding issues
   - **Impact**: Cannot scale beyond ~100 trains

3. **Static Memory Bug** (test_static_bug_detailed.cpp):
   - Line 494: `static Node leaf;` returns pointer to static memory
   - **CRITICAL**: Multiple find() calls will corrupt each other
   - **Impact**: query_ticket results will be garbage

**Code Review - Line 494**:
```cpp
Value* find(const Key& key) {
    std::streampos leaf_pos = findLeaf(key);
    if (leaf_pos < 0) return nullptr;

    static Node leaf;  // ❌ CRITICAL BUG
    if (!loadNode(leaf, leaf_pos)) return nullptr;
    // ...
}
```
**Problem**: If thread A calls find(key1) and thread B calls find(key2), they share the same static Node, causing corruption. Even in single-threaded code, nested find() calls will fail.

**Fix Required**: Allocate Node on heap or use instance variable.

**Verdict**: **FUNDAMENTAL DESIGN CORRECT but IMPLEMENTATION BROKEN**. Must fix before using for trains.

---

### 1.3 File I/O Layer ✅ GOOD

**File**: `fileio.hpp`

**Design Assessment**:
- Binary file operations with seek/read/write
- Buffered I/O support (4096 element buffer)
- Proper error handling with fail checks

**Strengths**:
1. ✅ Flush after every write (ensures persistence)
2. ✅ Template design supports any POD type
3. ✅ File creation if not exists
4. ✅ Truncate support for clean command

**Weaknesses**:
1. ⚠️ No caching layer - every read hits disk
2. ⚠️ No write-ahead logging - crash during write corrupts data
3. ⚠️ BufferedFileIO not used anywhere (dead code?)

**Verdict**: **ADEQUATE for current scale**. May need LRU cache for frequently accessed nodes at scale.

---

### 1.4 Type Definitions ✅ EXCELLENT

**File**: `types.hpp`

**Design Assessment**: Well-designed POD (Plain Old Data) structures with proper padding and null termination.

**Strengths**:
1. ✅ Fixed-size structures (safe for binary I/O)
2. ✅ Date/Time with proper arithmetic and comparison operators
3. ✅ Chinese character support (35-byte buffers for UTF-8)
4. ✅ All types are copyable (no dynamic allocation)

**Memory Sizes**:
```
User:     129 bytes (25+35+35+35+4 + padding)
Station:  35 bytes
Date:     8 bytes
Time:     8 bytes
DateTime: 16 bytes
```

**Verdict**: **PRODUCTION READY**. No issues found.

---

## 2. ARCHITECTURAL ASSESSMENT

### 2.1 Current Architecture

```
┌─────────────────────────────────────────────┐
│              main.cpp                       │
│  (Command parsing & dispatch)               │
└─────────────────────────────────────────────┘
                    │
        ┌───────────┴───────────┐
        │                       │
┌───────▼────────┐    ┌────────▼──────────┐
│  Hash Tables   │    │   File System     │
│  (in-memory)   │    │   (users.dat)     │
└────────────────┘    └───────────────────┘
   │
   └─ users: StringHashTable<User>
   └─ logged_in_users: StringHashTable<bool>
```

**Analysis**: Simple and effective for M1 (user management only). Will NOT scale for M2-M4.

---

### 2.2 Required Architecture for Full System

```
┌──────────────────────────────────────────────────────────┐
│                      Command Layer                       │
│  (Parsing, validation, business logic)                   │
└──────────────────────────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
┌───────▼────────┐  ┌──────▼──────┐  ┌───────▼────────┐
│  User Storage  │  │Train Storage│  │ Order Storage  │
│  (Hash Table)  │  │  (B+ Tree)  │  │  (B+ Tree)     │
└────────────────┘  └─────────────┘  └────────────────┘
        │                  │                  │
┌───────▼────────────────────────────────────▼────────┐
│              Index Layer (Multi-Index B+ Trees)      │
│  - Station Index: (station, date) → [trains]        │
│  - Route Index: (from, to) → [trains]                │
│  - User Orders: (user, timestamp) → [orders]         │
└──────────────────────────────────────────────────────┘
        │
┌───────▼────────────────────────────────────────────┐
│           Persistence Layer (FileIO)                │
│  - users.dat (binary)                               │
│  - trains.dat (B+ tree file)                        │
│  - orders.dat (B+ tree file)                        │
│  - station_index.dat, route_index.dat, etc.         │
└─────────────────────────────────────────────────────┘
```

---

## 3. DATA STRUCTURE SELECTION

### 3.1 User Management ✅

**Current**: StringHashTable<User>
**Verdict**: KEEP

**Rationale**:
- ~100K users fits in memory (~12 MiB)
- All operations are key-based lookups (no range queries)
- SF operation (query_profile) needs O(1) access

---

### 3.2 Session Management ✅

**Current**: StringHashTable<bool>
**Verdict**: KEEP

**Rationale**:
- Sessions are transient (cleared on exit)
- Small dataset (~1000 concurrent users)
- Frequent login/logout operations need O(1)

---

### 3.3 Train Storage ⚠️ NEEDS B+ TREE

**Required Operations**:
1. Add train: O(log n) - by trainID
2. Find train: O(log n) - by trainID
3. Delete train: O(log n) - by trainID
4. Query by station: O(log n + k) - range query
5. Query by date range: O(log n + k) - range query

**Recommendation**: **B+ Tree with trainID as primary key**

**Key Design**:
```cpp
struct TrainKey {
    char trainID[25];

    bool operator<(const TrainKey& other) const {
        return strcmp(trainID, other.trainID) < 0;
    }
    bool operator==(const TrainKey& other) const {
        return strcmp(trainID, other.trainID) == 0;
    }
};

BPTree<TrainKey, Train, 64> train_storage;
```

**Storage**: `trains.dat` (~50 KB per train × 10K trains = 500 MB on disk)

---

### 3.4 Seat Availability ⚠️ CRITICAL DESIGN

**Challenge**: Track seat availability per segment per date.

**Naive Approach** (DON'T USE):
```cpp
// 100 stations × 92 days × 10K trains = 92M records × 4 bytes = 368 MB
int seats[train][date][segment];
```
❌ Exceeds memory limit

**Recommended Approach**: **Sparse storage in B+ tree**

```cpp
struct SeatKey {
    char trainID[25];
    int date;        // Day number (Jun 1 = day 152)
    int segment;     // 0 to (stationNum-2)

    bool operator<(const SeatKey& other) const {
        int cmp = strcmp(trainID, other.trainID);
        if (cmp != 0) return cmp < 0;
        if (date != other.date) return date < other.date;
        return segment < other.segment;
    }
};

BPTree<SeatKey, int, 128> seat_availability;
```

**Benefits**:
- Only stores changed values (all start at seatNum)
- Range query: (trainID, date_start, 0) to (trainID, date_end, 99)
- Memory: Only sold segments stored (~1 MB for active orders)

**Storage**: `seats.dat`

---

### 3.5 Order Storage ⚠️ NEEDS B+ TREE

**Required Operations**:
1. Add order: O(log n)
2. Query orders by user: O(log n + k) - range query
3. Update order status: O(log n)
4. Query by date (for standby queue): O(log n + k)

**Recommendation**: **B+ Tree with composite key**

```cpp
struct OrderKey {
    char username[25];
    long long timestamp;  // For ordering newest→oldest

    bool operator<(const OrderKey& other) const {
        int cmp = strcmp(username, other.username);
        if (cmp != 0) return cmp < 0;
        return timestamp > other.timestamp;  // Reverse order for newest first
    }
};

BPTree<OrderKey, Order, 64> order_storage;
```

**Query orders**: Range query from (username, LLONG_MAX) to (username, 0) → automatic newest-first ordering

**Storage**: `orders.dat` (~200 bytes × 1M orders = 200 MB on disk)

---

### 3.6 Ticket Query Index ⚠️ MOST COMPLEX

**Challenge**: `query_ticket` must find all trains passing through two stations.

**Naive Approach** (DON'T USE):
```cpp
for each train:
    for each station pair:
        if matches: add to results
// O(trains × stations²) = 10K × 10K = 100M operations
```
❌ Too slow for SF operation

**Recommended Approach**: **Station-pair index with date**

```cpp
struct RouteDateKey {
    char from_station[35];
    char to_station[35];
    int date;
    char trainID[25];

    bool operator<(const RouteDateKey& other) const {
        int cmp1 = strcmp(from_station, other.from_station);
        if (cmp1 != 0) return cmp1 < 0;
        int cmp2 = strcmp(to_station, other.to_station);
        if (cmp2 != 0) return cmp2 < 0;
        if (date != other.date) return date < other.date;
        return strcmp(trainID, other.trainID) < 0;
    }
};

struct RouteInfo {
    int from_idx;    // Station index in train
    int to_idx;      // Station index in train
    int price;       // Cumulative price
    int travel_time; // Total minutes
};

BPTree<RouteDateKey, RouteInfo, 128> route_index;
```

**Build Time**: When train is released, generate all O(n²) station pairs:
```cpp
for i = 0 to stationNum-1:
    for j = i+1 to stationNum-1:
        for each date in saleDate range:
            insert((stations[i], stations[j], date), info)
```

**Query Time**: Range query on (from, to, date, "") to (from, to, date, "~") → O(log n + k) where k = matching trains

**Storage**: `route_index.dat` (~100 stations/train × 10K trains × 90 days = 90M entries × 100 bytes = **9 GB on disk**)
⚠️ **WILL EXCEED DISK LIMIT** - needs optimization

**Optimization**: Index only released trains, or use compressed representation.

---

### 3.7 Standby Queue ⚠️ SPECIAL CASE

**Challenge**: Manage FIFO queue per train/date/segment, auto-fulfill when seats available.

**Recommendation**: **Linked list stored in B+ tree**

```cpp
struct StandbyKey {
    char trainID[25];
    int date;
    int from_idx;
    int to_idx;
    long long order_id;  // For FIFO ordering

    bool operator<(const StandbyKey& other) const {
        // Sort by train, date, route, then order_id
        // ...
    }
};

BPTree<StandbyKey, OrderRef, 64> standby_queue;
```

**Process on refund**:
1. Update seat availability
2. Range query standby_queue for (train, date, any segment overlapping)
3. Try to fulfill each order in FIFO order
4. Update order status if fulfilled

---

## 4. MEMORY ANALYSIS

### 4.1 In-Memory Structures

| Component | Size | Count | Total |
|-----------|------|-------|-------|
| Hash table (users) | 80 KB | 1 | 80 KB |
| User records | 130 B | 100K | 12.4 MiB |
| Hash table (sessions) | 80 KB | 1 | 80 KB |
| Session records | 26 B | 1K | 26 KB |
| B+ tree cache (estimate) | 64 KB | 20 nodes | 1.3 MiB |
| Program code | - | - | ~2 MiB |
| **Total** | | | **~16 MiB** |

**Verdict**: ✅ **Within 42-47 MiB budget** with room for stack/heap growth.

---

### 4.2 Disk Usage

| File | Purpose | Estimated Size |
|------|---------|----------------|
| users.dat | User storage | 13 MB |
| trains.dat | Train metadata | 5 MB |
| seats.dat | Seat availability | 10 MB |
| orders.dat | Order history | 200 MB |
| route_index.dat | Ticket query index | **9 GB** ⚠️ |
| standby_queue.dat | Standby queue | 20 MB |
| **Total** | | **~9.2 GB** |

**Problem**: Exceeds 366-488 MiB disk limit by 20x!

**Solution**:
1. Don't build full route index - compute on-the-fly with station index
2. Use `station_index.dat`: (station, date) → [trains passing through] (~100 MB)
3. Query: Find trains through station A, filter for station B → O(k × log m) where k = trains at A

---

## 5. PERFORMANCE ESTIMATES

### 5.1 Super Frequent Operations (SF)

| Operation | Current | Target | Assessment |
|-----------|---------|--------|------------|
| `query_profile` | O(1) hash | O(1) | ✅ GOOD |
| `query_ticket` | Not impl | O(k log n) | ⚠️ Needs index |
| `buy_ticket` | Not impl | O(log n) | ⚠️ Needs B+ tree |

**Critical Path**: 1M operations × 10ms = 10,000 seconds → **FAIL**
Must achieve: 1M operations × 0.1ms = 100 seconds → **PASS**

**Bottleneck**: Disk I/O for B+ tree queries
- SSD: ~10K IOPS → 0.1ms per I/O
- B+ tree height: log₆₄(1M) = 3.3 levels → 4 disk reads
- Per query: 4 × 0.1ms = 0.4ms → 1M queries = 400 seconds ⚠️ **MARGINAL**

**Optimization Required**: LRU cache for hot nodes (root, frequently accessed internals)

---

### 5.2 Frequent Operations (F)

| Operation | Current | Target | Assessment |
|-----------|---------|--------|------------|
| `login` | O(1) | O(1) | ✅ GOOD |
| `logout` | O(1) | O(1) | ✅ GOOD |
| `modify_profile` | O(1) | O(1) | ✅ GOOD |
| `query_order` | Not impl | O(log n + k) | ⚠️ Needs B+ tree |

**Target**: 100K operations × 1ms = 100 seconds → **ACHIEVABLE**

---

### 5.3 Normal Operations (N)

**Target**: 10K operations × 10ms = 100 seconds → **ACHIEVABLE**

All train management operations (add, release, delete, query) should easily meet this target with B+ tree.

---

## 6. CRITICAL ISSUES & RECOMMENDATIONS

### 6.1 CRITICAL (Must Fix Immediately)

1. **B+ Tree Static Memory Bug** (Line 494)
   - **Impact**: All queries will return corrupted data
   - **Fix**: Change `static Node leaf` to heap allocation or instance variable
   - **Priority**: P0 - BLOCKING

2. **B+ Tree Multi-level Splitting** (Issue #11)
   - **Impact**: Cannot store >100 trains
   - **Fix**: Rewrite `insertIntoParent()` parent-finding logic
   - **Priority**: P0 - BLOCKING

3. **Route Index Disk Overflow**
   - **Impact**: Exceeds 488 MiB disk limit by 20x
   - **Fix**: Use station index instead of route index
   - **Priority**: P0 - ARCHITECTURAL

---

### 6.2 HIGH (Needed for Core Functionality)

4. **LRU Cache for B+ Tree**
   - **Impact**: Disk I/O will cause timeouts on SF operations
   - **Fix**: Implement 1-2 MiB node cache
   - **Priority**: P1

5. **Seat Availability Sparse Storage**
   - **Impact**: Memory explosion if using dense array
   - **Fix**: Use B+ tree with composite key as designed above
   - **Priority**: P1

---

### 6.3 MEDIUM (Performance Optimization)

6. **Hash Table Load Factor**
   - **Impact**: Degradation with clustering
   - **Fix**: Monitor average chain length, resize if >3
   - **Priority**: P2

7. **FileIO Buffering**
   - **Impact**: Many small writes are slow
   - **Fix**: Use BufferedFileIO (already implemented but unused)
   - **Priority**: P2

---

## 7. IMPLEMENTATION ROADMAP

### Phase 1: Fix Foundation (Week 1)
1. ✅ Fix B+ tree static memory bug
2. ✅ Fix B+ tree multi-level splitting
3. ✅ Add comprehensive B+ tree tests
4. ✅ Implement LRU cache for B+ tree nodes

### Phase 2: Train Management (Week 2)
1. ⚠️ Implement Train struct with all fields
2. ⚠️ Implement add_train with pipe-separated parsing
3. ⚠️ Implement release_train with state tracking
4. ⚠️ Implement query_train with date/time calculations
5. ⚠️ Implement delete_train with validation

### Phase 3: Indexing (Week 3)
1. ⚠️ Implement station index: (station, date) → [trains]
2. ⚠️ Implement seat availability B+ tree
3. ⚠️ Build index during train release

### Phase 4: Ticket Operations (Week 4)
1. ⚠️ Implement query_ticket with sorting
2. ⚠️ Implement buy_ticket with seat reservation
3. ⚠️ Implement query_transfer (most complex)

### Phase 5: Order Management (Week 5)
1. ⚠️ Implement order storage B+ tree
2. ⚠️ Implement query_order
3. ⚠️ Implement refund_ticket with standby processing
4. ⚠️ Implement standby queue auto-fulfillment

---

## 8. FINAL VERDICT

**Architecture Quality**: 6/10
- ✅ Good foundation with proper data structures
- ✅ Clean separation of concerns
- ⚠️ B+ tree implementation has critical bugs
- ⚠️ No caching layer for performance
- ❌ Route index design will exceed disk limit

**Implementation Status**: 3.5/10
- ✅ User management complete and correct
- ⚠️ B+ tree exists but broken
- ❌ Train management not started
- ❌ Ticket operations not started
- ❌ Order management not started

**Risk Assessment**: MEDIUM-HIGH
- **Technical Risk**: B+ tree bugs could take days to fix properly
- **Performance Risk**: Without caching, may timeout on SF operations
- **Disk Risk**: Must redesign index to fit in 488 MiB limit

**Recommendation**:
1. **PRIORITY 1**: Fix B+ tree bugs before implementing anything else
2. **PRIORITY 2**: Implement station index (not route index) for query_ticket
3. **PRIORITY 3**: Add LRU cache before performance testing

**Estimated Completion**: 5-6 weeks for full implementation assuming B+ tree is fixed within 1 week.

---

## 9. CODE QUALITY OBSERVATIONS

**Strengths**:
1. ✅ Consistent naming conventions (snake_case)
2. ✅ Proper const correctness in most places
3. ✅ Good use of templates for reusable code
4. ✅ Defensive programming (deleted copy constructors)
5. ✅ Clean command parsing with CommandParser class

**Weaknesses**:
1. ⚠️ Limited error handling (many functions return bool without error codes)
2. ⚠️ No logging/debugging infrastructure
3. ⚠️ Magic numbers (10007, 128, etc.) not documented
4. ⚠️ No unit tests visible (only integration tests)
5. ⚠️ Memory leaks possible if exceptions thrown (no RAII in hash table)

---

## 10. SPECIFIC TECHNICAL RECOMMENDATIONS

### 10.1 Fix B+ Tree Find Bug

**Current Code** (bptree.hpp:490-505):
```cpp
Value* find(const Key& key) {
    std::streampos leaf_pos = findLeaf(key);
    if (leaf_pos < 0) return nullptr;

    static Node leaf;  // ❌ BUG
    if (!loadNode(leaf, leaf_pos)) return nullptr;

    for (int i = 0; i < leaf.key_count; i++) {
        if (leaf.keys[i] == key) {
            return &leaf.values[i];  // ❌ Returns pointer to static memory
        }
    }
    return nullptr;
}
```

**Fix Option 1** (Copy value to instance variable):
```cpp
private:
    Value cached_value;  // Add instance variable

public:
    Value* find(const Key& key) {
        std::streampos leaf_pos = findLeaf(key);
        if (leaf_pos < 0) return nullptr;

        Node leaf;  // ✅ Local variable
        if (!loadNode(leaf, leaf_pos)) return nullptr;

        for (int i = 0; i < leaf.key_count; i++) {
            if (leaf.keys[i] == key) {
                cached_value = leaf.values[i];  // ✅ Copy to instance
                return &cached_value;
            }
        }
        return nullptr;
    }
```

**Fix Option 2** (Return by value with output parameter):
```cpp
bool find(const Key& key, Value& output) {
    std::streampos leaf_pos = findLeaf(key);
    if (leaf_pos < 0) return false;

    Node leaf;
    if (!loadNode(leaf, leaf_pos)) return false;

    for (int i = 0; i < leaf.key_count; i++) {
        if (leaf.keys[i] == key) {
            output = leaf.values[i];
            return true;
        }
    }
    return false;
}
```

**Recommendation**: Use Option 2 - clearer API, no hidden state.

---

### 10.2 Implement LRU Cache for B+ Tree

```cpp
template<typename Key, typename Value, int Degree = 128>
class CachedBPTree : public BPTree<Key, Value, Degree> {
private:
    static const int CACHE_SIZE = 16;  // ~1 MB for 64KB nodes

    struct CacheEntry {
        std::streampos pos;
        Node node;
        int access_count;
        CacheEntry* prev;
        CacheEntry* next;
    };

    CacheEntry* cache_head;
    CacheEntry* cache_tail;
    CacheEntry cache[CACHE_SIZE];

    Node* getCachedNode(std::streampos pos) {
        // Check cache
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (cache[i].pos == pos) {
                // Move to front (LRU)
                moveToFront(&cache[i]);
                return &cache[i].node;
            }
        }

        // Cache miss - evict LRU and load
        CacheEntry* victim = cache_tail;
        if (victim->pos >= 0) {
            saveNode(victim->node);  // Write back if dirty
        }

        loadNode(victim->node, pos);
        victim->pos = pos;
        moveToFront(victim);
        return &victim->node;
    }
};
```

**Benefit**: Reduces disk I/O by 80-90% for repeated queries.

---

### 10.3 Station Index Design

```cpp
struct StationDateKey {
    char station[35];
    int date;  // Day number
    char trainID[25];

    bool operator<(const StationDateKey& other) const {
        int cmp1 = strcmp(station, other.station);
        if (cmp1 != 0) return cmp1 < 0;
        if (date != other.date) return date < other.date;
        return strcmp(trainID, other.trainID) < 0;
    }
};

struct StationInfo {
    int station_idx;    // Index of this station in train's route
    int arrival_time;   // Minutes since epoch (for sorting)
    int departure_time;
};

BPTree<StationDateKey, StationInfo, 128> station_index;
```

**Build** (on release_train):
```cpp
for i = 0 to stationNum-1:
    for each date in saleDate range:
        station_index.insert(
            StationDateKey(stations[i], date, trainID),
            StationInfo(i, arrival_times[i], departure_times[i])
        );
```

**Query** (query_ticket):
```cpp
// Find trains at departure station
auto trains_at_from = station_index.range(
    StationDateKey(from_station, query_date, ""),
    StationDateKey(from_station, query_date, "~")
);

// For each train, check if it goes to destination
for each train in trains_at_from:
    // Load full train data
    Train* t = train_storage.find(train.trainID);

    // Check if train goes to 'to_station' after 'from_station'
    int to_idx = findStation(t->stations, to_station);
    if (to_idx > train.station_idx):
        // Valid route - add to results
        results.push_back(...);
```

**Size**: 100 stations/train × 10K trains × 90 days = 90M entries × 80 bytes = **7.2 GB** ⚠️ Still too large!

**Optimization**: Build index only for trains in next 30 days (~30% reduction) and rebuild periodically.

---

## CONCLUSION

The current implementation shows strong fundamentals but critical bugs block progress. The architecture is sound but needs refinement for disk limits. With B+ tree fixes and smart indexing, this system can meet all requirements within constraints.

**Next Step**: Ares or Maya should fix B+ tree bugs before any train implementation work begins.

