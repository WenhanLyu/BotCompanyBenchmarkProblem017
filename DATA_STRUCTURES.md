# Data Structures Documentation

This document describes the core data structures implemented for the train ticket management system.

## Overview

The system uses a combination of in-memory and disk-based data structures to achieve both performance and persistence requirements:

- **Hash Table**: Fast in-memory lookups (O(1) average)
- **B+ Tree**: Disk-based persistent storage with efficient queries
- **File I/O**: Low-level binary file operations

## File I/O Utilities (fileio.hpp)

### FileIO Class

Basic file operations for reading/writing binary data at specific positions.

```cpp
FileIO file("data.dat");

// Read data at position
MyStruct data;
file.read(data, pos);

// Write data at position
file.write(data, pos);

// Append to end
file.append(data);
```

### BufferedFileIO Class

Buffered version for batch operations (reduces disk I/O overhead).

```cpp
BufferedFileIO<MyStruct, 1024> buffer("data.dat");

// Write with automatic buffering
buffer.write(item1);
buffer.write(item2);
// ... automatically flushes when buffer full

buffer.flush();  // Manual flush
buffer.close();
```

## Hash Table (hashtable.hpp)

### Generic HashTable

Template-based hash table for any key-value types.

```cpp
HashTable<int, MyValue, 10007> table;

// Insert or update
table.insert(key, value);

// Find (returns pointer or nullptr)
MyValue* val = table.find(key);
if (val) {
    // Use *val
}

// Remove
table.remove(key);

// Check existence
if (table.contains(key)) { ... }

// Size
int n = table.size();
```

### StringHashTable

Optimized for string keys (e.g., username, trainID).

```cpp
StringHashTable<int, 10007> users;

users.insert("alice", 100);
int* privilege = users.find("alice");

users.remove("alice");
```

**Key Features:**
- Separate chaining for collision resolution
- Generic template supports any comparable key type
- O(1) average case insert/find/remove
- Iterator support for traversal

## B+ Tree (bptree.hpp)

Disk-based B+ tree for persistent storage with efficient queries.

```cpp
BPTree<int, MyValue, 128> tree("users.dat");

// Insert
tree.insert(key, value);

// Find
MyValue* val = tree.find(key);

// Remove
tree.remove(key);

// Check existence
if (tree.contains(key)) { ... }

// Clear all data
tree.clear();

// Close (saves changes)
tree.close();
```

**Key Features:**
- All data stored on disk (memory efficient)
- Leaf nodes linked for range queries
- Persists across program restarts
- Template parameters: Key type, Value type, Degree (default 128)

**Performance:**
- Insert: O(log n)
- Find: O(log n)
- Remove: O(log n)
- Range query: O(log n + k) where k is result count

## Data Types (types.hpp)

### User

```cpp
User user("alice", "pass123", "Alice", "alice@example.com", 5);

// Check password
if (user.checkPassword("pass123")) { ... }

// Update fields
user.setPassword("newpass");
user.setName("Alice Smith");
user.setEmail("new@example.com");
```

### Session

```cpp
Session session("alice");  // Create logged-in session

if (session.logged_in) {
    // User is logged in
}
```

### Date

```cpp
Date date("06-15");  // June 15

// Comparison
if (date1 < date2) { ... }

// Format back to string
char buffer[10];
date.format(buffer);  // "06-15"

// Convert to day number (for calculations)
int day_num = date.toDayNumber();
```

### Time

```cpp
Time time("14:30");  // 2:30 PM

// Convert to minutes
int mins = time.toMinutes();  // 870

// Add minutes (returns days overflow)
int days = time.addMinutes(150);  // Add 2.5 hours

// Format back
char buffer[10];
time.format(buffer);  // "17:00"
```

### DateTime

```cpp
DateTime dt(date, time);

// Comparison
if (dt1 < dt2) { ... }

// Add minutes (handles day overflow)
dt.addMinutes(600);  // Add 10 hours
```

## Memory Considerations

**Memory Limits**: 42-47 MiB per test case

**Strategy:**
- Use B+ tree for bulk data storage (users, trains, orders)
- Use hash table only for active sessions and frequently accessed indices
- B+ tree keeps most data on disk, loading nodes only when needed

**File Limits:** Maximum 20 files

**Recommendations:**
- One B+ tree file per major entity type (users, trains, orders)
- Use appropriate degree parameter (64-256) to balance I/O and memory
- Close files when not in use

## Testing

Run the test suite to verify all data structures:

```bash
g++ -std=c++17 -O2 test_datastructures.cpp -o test_ds
./test_ds
```

All tests should pass with ✓ marks.

## Integration Notes

1. **User Management**: Use StringHashTable for logged-in sessions, B+ tree for user data persistence
2. **Train Data**: Use B+ tree for train storage, hash table for quick train ID lookups during queries
3. **Order Management**: Use B+ tree for order storage (persistence required)

## Performance Tips

1. **Hash Table Size**: Use prime numbers close to expected size (default 10007 is good for ~10000 items)
2. **B+ Tree Degree**: Larger degree = fewer disk I/O but more memory per node. Default 128 is balanced.
3. **Batch Operations**: Use BufferedFileIO when writing many records sequentially
4. **Cleanup**: Call close() on B+ trees to ensure data is flushed to disk

## Next Steps

These data structures are ready for integration into the main system. Next tasks:
- Implement user management commands (add_user, login, logout, query_profile)
- Integrate hash table for session tracking
- Use B+ tree for persistent user storage
- Add proper error handling and edge case handling
