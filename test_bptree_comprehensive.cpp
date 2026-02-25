#include "bptree.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

int main() {
    const char* test_file = "test_bptree_comprehensive.dat";
    std::remove(test_file);

    BPTree<int, int> tree(test_file);

    // Test 1: Sequential inserts (stress test with 300 keys)
    std::cout << "Test 1: Sequential insert of 300 keys...\n";
    const int N = 300;
    for (int i = 0; i < N; i++) {
        if (!tree.insert(i, i * 100)) {
            std::cout << "FAILED at key " << i << "\n";
            return 1;
        }
    }

    // Verify all
    for (int i = 0; i < N; i++) {
        int* val = tree.find(i);
        if (!val || *val != i * 100) {
            std::cout << "FAILED: Key " << i << " not found or incorrect\n";
            return 1;
        }
    }
    std::cout << "  PASSED (Node count: " << tree.nodeCount() << ")\n";

    // Test 2: Updates
    std::cout << "\nTest 2: Update existing keys...\n";
    for (int i = 0; i < N; i += 10) {
        if (!tree.insert(i, -i)) {
            std::cout << "FAILED to update key " << i << "\n";
            return 1;
        }
    }

    // Verify updates
    for (int i = 0; i < N; i++) {
        int* val = tree.find(i);
        int expected = (i % 10 == 0) ? -i : i * 100;
        if (!val || *val != expected) {
            std::cout << "FAILED: Key " << i << " has value " << (val ? *val : -999)
                      << " (expected " << expected << ")\n";
            return 1;
        }
    }
    std::cout << "  PASSED\n";

    // Test 3: Deletions
    std::cout << "\nTest 3: Delete some keys...\n";
    for (int i = 0; i < N; i += 7) {
        if (!tree.remove(i)) {
            std::cout << "FAILED to remove key " << i << "\n";
            return 1;
        }
    }

    // Verify deletions
    for (int i = 0; i < N; i++) {
        int* val = tree.find(i);
        if (i % 7 == 0) {
            if (val != nullptr) {
                std::cout << "FAILED: Key " << i << " should be deleted but still exists\n";
                return 1;
            }
        } else {
            if (!val) {
                std::cout << "FAILED: Key " << i << " should exist but was deleted\n";
                return 1;
            }
        }
    }
    std::cout << "  PASSED\n";

    // Test 4: Re-insert deleted keys
    std::cout << "\nTest 4: Re-insert deleted keys...\n";
    for (int i = 0; i < N; i += 7) {
        if (!tree.insert(i, i * 999)) {
            std::cout << "FAILED to re-insert key " << i << "\n";
            return 1;
        }
    }

    // Verify all again
    for (int i = 0; i < N; i++) {
        int* val = tree.find(i);
        if (!val) {
            std::cout << "FAILED: Key " << i << " not found after re-insert\n";
            return 1;
        }
    }
    std::cout << "  PASSED\n";

    // Test 5: Persistence
    std::cout << "\nTest 5: Close and reopen...\n";
    tree.close();

    BPTree<int, int> tree2(test_file);

    // Verify random sample
    srand(42);
    for (int i = 0; i < 50; i++) {
        int key = rand() % N;
        int* val = tree2.find(key);
        if (!val) {
            std::cout << "FAILED: Key " << key << " not found after reopen\n";
            return 1;
        }
    }
    std::cout << "  PASSED\n";

    // Test 6: Verify non-existent keys
    std::cout << "\nTest 6: Check non-existent keys...\n";
    for (int i = N; i < N + 100; i++) {
        int* val = tree2.find(i);
        if (val != nullptr) {
            std::cout << "FAILED: Non-existent key " << i << " found\n";
            return 1;
        }
    }
    std::cout << "  PASSED\n";

    std::cout << "\n========================================\n";
    std::cout << "ALL COMPREHENSIVE TESTS PASSED!\n";
    std::cout << "Final node count: " << tree2.nodeCount() << "\n";
    std::cout << "========================================\n";

    tree2.close();
    std::remove(test_file);
    return 0;
}
