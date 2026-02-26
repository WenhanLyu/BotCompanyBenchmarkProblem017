#include "bptree.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

// Test 1: Static memory bug in find()
void test_find_static_bug() {
    cout << "Test 1: Static memory corruption in find()" << endl;

    BPTree<int, int, 4> tree("test_static_bug.dat");
    tree.clear();

    // Insert some values
    tree.insert(1, 100);
    tree.insert(2, 200);
    tree.insert(3, 300);

    // Get two pointers from find()
    int* val1 = tree.find(1);
    int* val2 = tree.find(2);

    // If they point to static memory, val1 might be corrupted
    cout << "  val1 (key=1): " << (val1 ? *val1 : -1) << " (expected: 100)" << endl;
    cout << "  val2 (key=2): " << (val2 ? *val2 : -1) << " (expected: 200)" << endl;

    if (val1 && *val1 != 100) {
        cout << "  FAIL: Static memory bug detected! val1 was corrupted." << endl;
    } else {
        cout << "  PASS: No corruption detected (but bug may exist in other scenarios)" << endl;
    }

    tree.close();
    cout << endl;
}

// Test 2: Deep tree (depth > 3)
void test_deep_tree() {
    cout << "Test 2: Deep tree with depth > 3" << endl;

    BPTree<int, int, 4> tree("test_deep_tree.dat");
    tree.clear();

    // Insert many values to force multiple levels
    // With Degree=4, MAX_KEYS=3, so we need > 3^3 = 27 keys for depth > 3
    cout << "  Inserting 100 values..." << endl;
    for (int i = 0; i < 100; i++) {
        if (!tree.insert(i, i * 10)) {
            cout << "  FAIL: Insert failed at i=" << i << endl;
            tree.close();
            return;
        }
    }

    cout << "  Verifying all 100 values..." << endl;
    bool all_found = true;
    for (int i = 0; i < 100; i++) {
        int* val = tree.find(i);
        if (!val || *val != i * 10) {
            cout << "  FAIL: Key " << i << " not found or wrong value" << endl;
            all_found = false;
            break;
        }
    }

    if (all_found) {
        cout << "  PASS: All 100 values found correctly" << endl;
    }

    tree.close();
    cout << endl;
}

// Test 3: Large scale (10,000+ insertions)
void test_large_scale() {
    cout << "Test 3: Large scale with 10,000+ insertions" << endl;

    BPTree<int, int, 128> tree("test_large_scale.dat");
    tree.clear();

    const int N = 12000;
    cout << "  Inserting " << N << " values..." << endl;
    for (int i = 0; i < N; i++) {
        if (!tree.insert(i, i * 2)) {
            cout << "  FAIL: Insert failed at i=" << i << endl;
            tree.close();
            return;
        }
        if (i % 2000 == 0) {
            cout << "    Progress: " << i << "/" << N << endl;
        }
    }

    cout << "  Verifying random samples..." << endl;
    vector<int> samples = {0, 1000, 5000, 9999, 11999};
    bool all_correct = true;
    for (int key : samples) {
        int* val = tree.find(key);
        if (!val || *val != key * 2) {
            cout << "  FAIL: Key " << key << " verification failed" << endl;
            all_correct = false;
        }
    }

    if (all_correct) {
        cout << "  PASS: Random samples verified correctly" << endl;
    }

    tree.close();
    cout << endl;
}

// Test 4: Move constructor (should not compile if properly deleted)
// Commented out because it should fail to compile
/*
void test_move_constructor() {
    cout << "Test 4: Move constructor" << endl;

    BPTree<int, int, 4> tree1("test1.dat");
    BPTree<int, int, 4> tree2 = std::move(tree1);  // Should not compile

    cout << endl;
}
*/

int main() {
    cout << "=== B+ Tree Bug Tests ===" << endl << endl;

    test_find_static_bug();
    test_deep_tree();
    test_large_scale();

    cout << "=== Tests Complete ===" << endl;
    return 0;
}
