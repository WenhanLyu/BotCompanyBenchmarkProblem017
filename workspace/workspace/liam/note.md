# Liam - query_order Test Report

## Test Date: 2026-02-26

## Summary: ✅ PASS

The query_order implementation works correctly with real data and produces the expected output format.

## Test Results

### 1. Build Status: ✅ Success
- `make clean && make` completed successfully
- Executable `code` created without errors

### 2. Simple Test (No Orders): ✅ Pass
**Test**: User with no orders
```
add_user -c "" -u bob -p pass123 -n Bob -m bob@test.com -g 10
login -u bob -p pass123
query_order -u bob
```
**Result**: Output is `0` (correct, not `-1`)

### 3. Test with Orders: ✅ Pass
**Test**: User with 1 ticket purchase
```
buy_ticket -u charlie -i T100 -d 07-15 -n 2 -f StationX -t StationY
query_order -u charlie
```
**Result**:
```
1
1 T100 StationX StationY success 100 2
```
- Count: 1 order
- Format: `order_id trainID from to status price count` ✅
- Displays actual data, not -1 ✅

### 4. Comprehensive Test: ✅ Pass
**Test**: Multiple orders from Leo's test suite
```
buy_ticket -u alice -i T001 -d 06-01 -n 5 -f CityA -t CityB
buy_ticket -u alice -i T001 -d 06-02 -n 3 -f CityB -t CityC -q true
query_order -u alice
```
**Result**:
```
2
1 T001 CityA CityB success 500 5
2 T001 CityB CityC success 600 3
```
- Count: 2 orders
- Both orders displayed correctly
- Format matches specification ✅

### 5. Edge Cases: ✅ Pass
Tested permission violations:
- ❌ Query different user's orders → `-1` (correct)
- ❌ Query nonexistent user → `-1` (correct)
- ❌ Query while not logged in → `-1` (correct)
- ✅ Query own orders (empty) → `0` (correct)

## Verification

- ✅ query_order outputs actual order data (not -1) when orders exist
- ✅ query_order outputs 0 when user has no orders (not -1)
- ✅ Output format is correct: `order_id trainID from to status price count`
- ✅ Permission checks work correctly (returns -1 for unauthorized access)
- ✅ Multiple orders are displayed correctly

## Conclusion

**PASS** - The query_order command is fully functional and produces correct output in all test scenarios.
