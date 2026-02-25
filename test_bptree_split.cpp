#include "bptree.hpp"
#include <iostream>
#include <cstdio>

int main() {
    const char* test_file = "test_bptree_split.dat";

    // Remove old test file if exists
    std::remove(test_file);

    // Create B+ tree with degree 128 (MAX_KEYS = 127)
    BPTree<int, int> tree(test_file);

    std::cout << "Testing B+ tree with >127 insertions...\n";
    std::cout << "MAX_KEYS should be 127 (Degree - 1)\n\n";

    // Insert 200 values to force multiple splits
    const int NUM_INSERTS = 200;
    std::cout << "Inserting " << NUM_INSERTS << " key-value pairs...\n";

    for (int i = 0; i < NUM_INSERTS; i++) {
        if (!tree.insert(i, i * 10)) {
            std::cout << "ERROR: Insert failed at key " << i << "\n";
            return 1;
        }

        // Progress indicator
        if ((i + 1) % 50 == 0) {
            std::cout << "  Inserted " << (i + 1) << " keys\n";
        }
    }

    std::cout << "\nVerifying all inserted values...\n";

    // Verify all values were inserted correctly
    int errors = 0;
    for (int i = 0; i < NUM_INSERTS; i++) {
        int* val = tree.find(i);
        if (val == nullptr) {
            std::cout << "ERROR: Key " << i << " not found\n";
            errors++;
        } else if (*val != i * 10) {
            std::cout << "ERROR: Key " << i << " has wrong value: " << *val << " (expected " << (i * 10) << ")\n";
            errors++;
        }

        if (errors > 10) {
            std::cout << "Too many errors, stopping verification\n";
            break;
        }
    }

    if (errors == 0) {
        std::cout << "SUCCESS: All " << NUM_INSERTS << " values verified correctly\n";
    } else {
        std::cout << "FAILED: " << errors << " errors found\n";
        return 1;
    }

    std::cout << "\nNode count: " << tree.nodeCount() << "\n";
    std::cout << "(Should be > 1 due to splits)\n";

    // Test update of existing key
    std::cout << "\nTesting update of existing key...\n";
    if (!tree.insert(50, 999)) {
        std::cout << "ERROR: Update failed\n";
        return 1;
    }

    int* val = tree.find(50);
    if (val == nullptr || *val != 999) {
        std::cout << "ERROR: Update verification failed\n";
        return 1;
    }
    std::cout << "Update successful\n";

    // Close and reopen to test persistence
    std::cout << "\nTesting persistence...\n";
    tree.close();

    BPTree<int, int> tree2(test_file);

    // Verify a few random values
    int test_keys[] = {0, 50, 100, 150, 199};
    int expected[] = {0, 999, 1000, 1500, 1990};

    for (int i = 0; i < 5; i++) {
        int* v = tree2.find(test_keys[i]);
        if (v == nullptr) {
            std::cout << "ERROR: Key " << test_keys[i] << " not found after reload\n";
            return 1;
        }
        if (*v != expected[i]) {
            std::cout << "ERROR: Key " << test_keys[i] << " has wrong value: " << *v << " (expected " << expected[i] << ")\n";
            return 1;
        }
    }

    std::cout << "Persistence test passed\n";

    std::cout << "\n=== ALL TESTS PASSED ===\n";

    // Cleanup
    tree2.close();
    std::remove(test_file);

    return 0;
}
