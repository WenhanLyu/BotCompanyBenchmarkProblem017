# CRITICAL BUGS - Immediate Action Required

**Architect**: Magnus
**Date**: 2026-02-25

---

## 🔴 P0 - BLOCKING PRODUCTION

### Bug #1: B+ Tree Static Memory Corruption
**Location**: `bptree.hpp:494`

**Code**:
```cpp
Value* find(const Key& key) {
    static Node leaf;  // ❌ CRITICAL BUG
    if (!loadNode(leaf, leaf_pos)) return nullptr;
    // ...
    return &leaf.values[i];  // Returns pointer to static memory
}
```

**Problem**: Multiple calls to `find()` share same static Node. Second call corrupts first call's data.

**Impact**: ALL query operations will return garbage data.

**Example Failure**:
```cpp
User* u1 = users.find("alice");  // Returns &leaf.values[0]
User* u2 = users.find("bob");    // OVERWRITES same leaf, u1 now invalid
cout << u1->name;                // ❌ CRASHES or prints "bob"
```

**Fix** (choose one):

**Option A** - Output parameter (recommended):
```cpp
bool find(const Key& key, Value& output) {
    Node leaf;  // ✅ Local variable
    if (!loadNode(leaf, leaf_pos)) return false;
    for (int i = 0; i < leaf.key_count; i++) {
        if (leaf.keys[i] == key) {
            output = leaf.values[i];  // ✅ Copy to caller
            return true;
        }
    }
    return false;
}
```

**Option B** - Instance variable:
```cpp
class BPTree {
private:
    Value cached_value;  // ✅ Instance variable

public:
    Value* find(const Key& key) {
        Node leaf;
        if (!loadNode(leaf, leaf_pos)) return nullptr;
        for (int i = 0; i < leaf.key_count; i++) {
            if (leaf.keys[i] == key) {
                cached_value = leaf.values[i];  // ✅ Copy to instance
                return &cached_value;
            }
        }
        return nullptr;
    }
};
```

**Effort**: 30 minutes + testing
**Priority**: P0 - DO FIRST

---

### Bug #2: B+ Tree Multi-Level Split Fails
**Location**: `bptree.hpp:203-231` (findParent)

**Problem**: When tree grows beyond 2 levels, parent-finding logic fails.

**Impact**: Cannot store more than ~100 trains.

**Symptoms**:
- Insertions succeed but tree becomes corrupted
- Later queries return wrong results
- Tree height > 2 causes assertion failures in tests

**Root Cause**: `findParent()` uses incorrect traversal logic for multi-level trees.

**Test**: `test_bptree_bugs.cpp` demonstrates failure

**Fix**: Rewrite parent-finding to use proper tree traversal:
```cpp
bool findParent(std::streampos child_pos, Node& parent, int& child_index) {
    if (child_pos == header.root_pos) return false;

    // Use stack-based iterative traversal instead of recursion
    std::streampos path[MAX_HEIGHT];
    int path_len = 0;

    // Descend from root, tracking path
    std::streampos curr = header.root_pos;
    while (curr >= 0) {
        Node node;
        if (!loadNode(node, curr)) return false;

        // Check if any child matches
        for (int i = 0; i <= node.key_count; i++) {
            if (node.children[i] == child_pos) {
                parent = node;
                child_index = i;
                return true;
            }
        }

        // Descend (need smarter logic here based on child_pos range)
        path[path_len++] = curr;
        curr = node.children[0];  // Simplified - needs proper descent
    }

    return false;
}
```

**Effort**: 2-4 hours + extensive testing
**Priority**: P0 - DO SECOND

---

## 🟡 P1 - HIGH PRIORITY

### Issue #3: Route Index Disk Overflow
**Problem**: Full route index = 9 GB, limit = 488 MB

**Current Design** (DON'T USE):
```cpp
// For each train, store all O(n²) station pairs
// 100 stations × 100 stations × 10K trains × 90 days = 9 GB
BPTree<RouteDateKey, RouteInfo> route_index;
```

**Recommended Design**:
```cpp
// Store only which trains pass through each station on each date
// 100 stations × 10K trains × 90 days = 90M entries × 80 bytes = 7.2 GB
// Still too large, but better

struct StationDateKey {
    char station[35];
    int date;
    char trainID[25];
};

struct StationInfo {
    int station_idx;     // Which station in this train's route
    int arrival_time;    // For filtering
    int departure_time;
};

BPTree<StationDateKey, StationInfo> station_index;
```

**Query Strategy**:
```cpp
// query_ticket -s A -t B -d 2021-06-15
// 1. Find all trains at station A on date
auto trains_at_A = station_index.range((A, date, ""), (A, date, "~"));

// 2. For each train, load metadata and check if goes to B
for (auto& entry : trains_at_A) {
    Train* train = trains.find(entry.trainID);
    if (train has station B after station A) {
        // Add to results
    }
}
```

**Optimization**: Build index only for next 30 days (~2.4 GB)

**Effort**: 4-8 hours design + implementation
**Priority**: P1 - DO THIRD

---

### Issue #4: B+ Tree Performance Without Cache
**Problem**: 1M queries × 4 disk reads each = 4M disk I/O = 400 seconds (FAIL)

**Target**: <100 seconds for SF operations

**Solution**: LRU cache for hot nodes

```cpp
template<int CACHE_SIZE = 16>  // 16 nodes × 64 KB = 1 MB
class CachedBPTree : public BPTree {
private:
    struct CacheEntry {
        std::streampos pos;
        Node node;
        bool dirty;
        CacheEntry* lru_prev;
        CacheEntry* lru_next;
    };

    CacheEntry cache[CACHE_SIZE];
    CacheEntry* lru_head;
    CacheEntry* lru_tail;

    bool loadNodeCached(Node& node, std::streampos pos) {
        // Check cache first
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (cache[i].pos == pos) {
                node = cache[i].node;
                moveToFront(&cache[i]);
                return true;
            }
        }

        // Cache miss - load from disk
        if (!loadNode(node, pos)) return false;

        // Evict LRU entry
        CacheEntry* victim = lru_tail;
        if (victim->dirty) {
            saveNode(victim->node);
        }

        victim->node = node;
        victim->pos = pos;
        victim->dirty = false;
        moveToFront(victim);

        return true;
    }
};
```

**Expected Impact**:
- Cache hit rate: 80-90% (root + hot internal nodes)
- Effective I/O: 0.4 disk reads per query
- Total time: 1M × 0.4 × 0.1ms = 40 seconds ✅ PASS

**Effort**: 4-6 hours
**Priority**: P1 - DO FOURTH

---

## 📋 ACTION ITEMS

**For Ares/Maya (Data Structure Team)**:
1. [ ] Fix B+ tree static memory bug (30 min)
2. [ ] Fix B+ tree multi-level split bug (4 hours)
3. [ ] Add comprehensive B+ tree unit tests (2 hours)
4. [ ] Implement LRU cache wrapper (6 hours)

**For Implementation Team**:
1. [ ] Use station index design (not route index) for query_ticket
2. [ ] Implement sparse seat storage with B+ tree
3. [ ] Design Train struct with proper memory layout

**Testing**:
1. [ ] Test B+ tree with 10,000+ entries
2. [ ] Test B+ tree with trees of height 4+
3. [ ] Benchmark disk I/O with and without cache
4. [ ] Verify memory usage stays under 47 MiB

---

## ESTIMATED TIMELINE

| Task | Effort | Blocker? |
|------|--------|----------|
| Fix static memory bug | 0.5 hours | YES |
| Fix multi-level split | 4 hours | YES |
| Add B+ tree tests | 2 hours | NO |
| Implement LRU cache | 6 hours | NO |
| Design station index | 4 hours | NO |
| **TOTAL** | **16.5 hours** | **2 blockers** |

**Critical Path**: Fix B+ tree bugs (4.5 hours) → Can start train implementation

---

## REFERENCES

- Full evaluation: `architecture_evaluation.md`
- B+ tree source: `../../bptree.hpp`
- Test files: `../../test_bptree_*.cpp`
- Iris requirements: `../iris/requirements_analysis.md`
