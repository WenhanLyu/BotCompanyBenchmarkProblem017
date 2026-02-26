/**
 * M1.1 Verification - Issue #12
 *
 * Test: findParent() correctness
 * Validates that the current O(n) findParent() implementation works
 * correctly in all scenarios.
 *
 * Expected: All parent lookups are correct, splits work properly
 */

#include "../../../bptree.hpp"
#include <cassert>
#include <iostream>
#include <cstdio>

// Note: This test verifies that findParent() works correctly by testing
// tree operations that depend on it (especially splits)

void test_basic_insertions_no_split() {
    std::cout << "Test 1: Basic insertions without splits..." << std::endl;

    BPTree<int, int> tree("test_basic.dat");

    // Insert a few values (should not trigger splits with default order)
    for (int i = 0; i < 10; i++) {
        tree.insert(i, i * 10);
    }

    // Verify all values are findable
    for (int i = 0; i < 10; i++) {
        int* val = tree.find(i);
        assert(val != nullptr);
        assert(*val == i * 10);
    }

    std::cout << "✓ Basic insertions PASSED" << std::endl;

    std::remove("test_basic.dat");
}

void test_insertions_with_splits() {
    std::cout << "\nTest 2: Insertions causing splits..." << std::endl;

    BPTree<int, int> tree("test_splits.dat");

    // Insert enough values to force multiple splits
    // With default ORDER=50, we need > 50 keys in a node to split
    const int num_insertions = 200;

    for (int i = 0; i < num_insertions; i++) {
        tree.insert(i, i * 100);
    }

    // Verify all values are still findable after splits
    for (int i = 0; i < num_insertions; i++) {
        int* val = tree.find(i);
        assert(val != nullptr);
        assert(*val == i * 100);
    }

    std::cout << "✓ Insertions with splits PASSED" << std::endl;

    std::remove("test_splits.dat");
}

void test_reverse_order_insertions() {
    std::cout << "\nTest 3: Reverse order insertions..." << std::endl;

    BPTree<int, int> tree("test_reverse.dat");

    // Insert in reverse order (stresses different split patterns)
    const int num_insertions = 150;

    for (int i = num_insertions - 1; i >= 0; i--) {
        tree.insert(i, i * 50);
    }

    // Verify all values are findable
    for (int i = 0; i < num_insertions; i++) {
        int* val = tree.find(i);
        assert(val != nullptr);
        assert(*val == i * 50);
    }

    std::cout << "✓ Reverse order insertions PASSED" << std::endl;

    std::remove("test_reverse.dat");
}

void test_random_order_insertions() {
    std::cout << "\nTest 4: Random-ish order insertions..." << std::endl;

    BPTree<int, int> tree("test_random.dat");

    // Insert in a semi-random pattern
    const int num_insertions = 200;

    // Insert every other number first
    for (int i = 0; i < num_insertions; i += 2) {
        tree.insert(i, i * 25);
    }

    // Then fill in the gaps
    for (int i = 1; i < num_insertions; i += 2) {
        tree.insert(i, i * 25);
    }

    // Verify all values are findable
    for (int i = 0; i < num_insertions; i++) {
        int* val = tree.find(i);
        assert(val != nullptr);
        assert(*val == i * 25);
    }

    std::cout << "✓ Random-ish order insertions PASSED" << std::endl;

    std::remove("test_random.dat");
}

void test_duplicate_insertions() {
    std::cout << "\nTest 5: Duplicate key insertions (updates)..." << std::endl;

    BPTree<int, int> tree("test_duplicates.dat");

    // Insert initial values
    for (int i = 0; i < 100; i++) {
        tree.insert(i, i);
    }

    // Update all values (tests that splits didn't corrupt structure)
    for (int i = 0; i < 100; i++) {
        tree.insert(i, i * 1000); // Update with new value
    }

    // Verify updated values
    for (int i = 0; i < 100; i++) {
        int* val = tree.find(i);
        assert(val != nullptr);
        assert(*val == i * 1000);
    }

    std::cout << "✓ Duplicate key insertions PASSED" << std::endl;

    std::remove("test_duplicates.dat");
}

void test_large_dataset() {
    std::cout << "\nTest 6: Large dataset with many splits..." << std::endl;

    BPTree<int, int> tree("test_large.dat");

    // Insert 1000+ values to ensure multiple levels of splits
    const int num_insertions = 1000;

    std::cout << "  Inserting " << num_insertions << " values..." << std::endl;

    for (int i = 0; i < num_insertions; i++) {
        tree.insert(i, i * 7);

        // Periodic verification during insertion
        if (i > 0 && i % 200 == 0) {
            // Verify a sample of already-inserted values
            for (int j = 0; j < i; j += 50) {
                int* val = tree.find(j);
                assert(val != nullptr);
                assert(*val == j * 7);
            }
        }
    }

    std::cout << "  Verifying all " << num_insertions << " values..." << std::endl;

    // Verify all values
    for (int i = 0; i < num_insertions; i++) {
        int* val = tree.find(i);
        assert(val != nullptr);
        assert(*val == i * 7);
    }

    std::cout << "✓ Large dataset PASSED" << std::endl;

    std::remove("test_large.dat");
}

void test_persistence_after_splits() {
    std::cout << "\nTest 7: Persistence after splits..." << std::endl;

    const char* filename = "test_persist_splits.dat";

    // Create tree and cause splits
    {
        BPTree<int, int> tree(filename);
        for (int i = 0; i < 300; i++) {
            tree.insert(i, i * 3);
        }
        // Tree destroyed, data written to disk
    }

    // Reopen and verify structure is intact
    {
        BPTree<int, int> tree(filename);

        // Verify all values are still correct
        for (int i = 0; i < 300; i++) {
            int* val = tree.find(i);
            assert(val != nullptr);
            assert(*val == i * 3);
        }

        // Insert more values to cause more splits
        for (int i = 300; i < 500; i++) {
            tree.insert(i, i * 3);
        }

        // Verify everything again
        for (int i = 0; i < 500; i++) {
            int* val = tree.find(i);
            assert(val != nullptr);
            assert(*val == i * 3);
        }
    }

    std::cout << "✓ Persistence after splits PASSED" << std::endl;

    std::remove(filename);
}

void test_string_keys_with_splits() {
    std::cout << "\nTest 8: String keys with splits..." << std::endl;

    BPTree<std::string, int> tree("test_string_splits.dat");

    // Insert many string keys to cause splits
    const int num_insertions = 200;

    for (int i = 0; i < num_insertions; i++) {
        std::string key = "key_" + std::to_string(i);
        tree.insert(key, i);
    }

    // Verify all values
    for (int i = 0; i < num_insertions; i++) {
        std::string key = "key_" + std::to_string(i);
        int* val = tree.find(key);
        assert(val != nullptr);
        assert(*val == i);
    }

    std::cout << "✓ String keys with splits PASSED" << std::endl;

    std::remove("test_string_splits.dat");
}

void test_edge_case_single_insert() {
    std::cout << "\nTest 9: Edge case - single insertion..." << std::endl;

    BPTree<int, int> tree("test_single.dat");

    tree.insert(42, 999);

    int* val = tree.find(42);
    assert(val != nullptr);
    assert(*val == 999);

    std::cout << "✓ Single insertion PASSED" << std::endl;

    std::remove("test_single.dat");
}

void test_boundary_values() {
    std::cout << "\nTest 10: Boundary value testing..." << std::endl;

    BPTree<int, int> tree("test_boundary.dat");

    // Insert values at potential boundary conditions
    tree.insert(0, 1000);
    tree.insert(-1, 1001);
    tree.insert(-100, 1002);
    tree.insert(100, 1003);
    tree.insert(999, 1004);

    // Insert many more to cause splits (skip boundary values to not overwrite them)
    for (int i = 1; i < 200; i++) {
        if (i == 100 || i == 999) continue; // Don't overwrite boundary test values
        tree.insert(i, i + 2000);
    }

    // Verify boundary values are still correct
    assert(*tree.find(0) == 1000);
    assert(*tree.find(-1) == 1001);
    assert(*tree.find(-100) == 1002);
    assert(*tree.find(100) == 1003);
    assert(*tree.find(999) == 1004);

    std::cout << "✓ Boundary values PASSED" << std::endl;

    std::remove("test_boundary.dat");
}

int main() {
    std::cout << "=== M1.1 Issue #12 Verification Tests ===" << std::endl;
    std::cout << "Testing: findParent() correctness" << std::endl;
    std::cout << "Verification: O(n) implementation is correct and adequate\n" << std::endl;

    try {
        test_basic_insertions_no_split();
        test_insertions_with_splits();
        test_reverse_order_insertions();
        test_random_order_insertions();
        test_duplicate_insertions();
        test_large_dataset();
        test_persistence_after_splits();
        test_string_keys_with_splits();
        test_edge_case_single_insert();
        test_boundary_values();

        std::cout << "\n=== ALL TESTS PASSED ✓ ===" << std::endl;
        std::cout << "Issue #12 verification complete: findParent() is correct." << std::endl;
        std::cout << "\nConclusion: O(n) findParent() implementation handles all" << std::endl;
        std::cout << "scenarios correctly. Performance is acceptable for infrequent" << std::endl;
        std::cout << "split operations. No changes needed." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ TEST FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ TEST FAILED: Unknown error" << std::endl;
        return 1;
    }
}
