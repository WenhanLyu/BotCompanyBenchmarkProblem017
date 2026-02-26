# Train Persistence Verification - Henry

## Date: 2026-02-26

## Test Result: ✅ PASS

Train data persists correctly across program restarts using BPTree storage.

## BPTree Implementation Verification

**File**: `bptree.hpp`
- **Line 8-18**: Documented as "Persistent across program restarts"
- **Line 49**: Uses FileIO for disk operations
- **Line 393-405**: `open()` method loads existing header from disk, preserves tree structure
- **Architecture**: Disk-based B+ tree with header storing root position and node count

**Usage in main.cpp**:
- **Line 17**: `BPTree<TrainKey, Train, 16> trains;`
- **Line 676**: `trains.open("trains.dat");`

## Persistence Test Execution

### Test Setup
- Clean start: removed trains.dat and users.dat
- Rebuilt project successfully

### Part 1: Add and Release Train
```
add_train -i TEST_PERSIST -n 3 -m 100 -s StationA|StationB|StationC -p 50|30 -x 08:00 -t 60|90 -o 10 -d 06-01|08-31 -y G
release_train -i TEST_PERSIST
query_train -i TEST_PERSIST -d 06-15
```

**Output**:
```
0
0
TEST_PERSIST G
StationA xx-xx xx:xx -> 06-15 08:00 0 100
StationB 06-15 09:00 -> 06-15 09:10 50 100
StationC 06-15 10:40 -> xx-xx xx:xx 80 x
bye
```

### Part 2: Restart and Query Again
```
query_train -i TEST_PERSIST -d 06-15
```

**Output**:
```
TEST_PERSIST G
StationA xx-xx xx:xx -> 06-15 08:00 0 100
StationB 06-15 09:00 -> 06-15 09:10 50 100
StationC 06-15 10:40 -> xx-xx xx:xx 80 x
bye
```

### Comparison Result
```bash
$ diff test_persist_part1.txt test_persist_part2.txt
1,2d0
< 0
< 0
```

**Analysis**: Only difference is the command acknowledgments (0) from add_train and release_train in Part 1. The actual train query data is **IDENTICAL** in both outputs.

## trains.dat File Information

```bash
$ ls -lh trains.dat
-rw-r--r--  1 wenhanlyu  staff    72K Feb 26 00:02 trains.dat
```

- **Exists**: Yes
- **Size**: 72 KB
- **Contains data**: Yes (non-zero size confirms data written)

## Conclusion

✅ **Train persistence VERIFIED**:
1. BPTree correctly implements disk-based storage
2. Train data persists to trains.dat file
3. Program restart successfully loads existing data
4. Query results are identical before and after restart
5. No data loss or corruption detected

The BPTree implementation fulfills its documented promise of persistence across program restarts.
