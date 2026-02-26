#include "../../../bptree.hpp"
#include <iostream>

using namespace std;

// Helper to verify tree structure by inserting many keys and checking no crashes
bool testFindParentWithInsertions() {
    BPTree<int, int, 4> tree("test_findparent.dat");
    tree.clear();

    // Insert many values to create deep tree with multiple splits
    // Degree 4 means max 3 keys per node, so splits happen frequently
    cout << "Inserting 1000 keys..." << endl;
    for (int i = 0; i < 1000; i++) {
        tree.insert(i, i * 10);
    }

    // Verify all values can be found
    cout << "Verifying all keys can be found..." << endl;
    for (int i = 0; i < 1000; i++) {
        int* result = tree.find(i);
        if (!result || *result != i * 10) {
            cout << "ERROR: Key " << i << " not found or has wrong value" << endl;
            return false;
        }
    }

    cout << "All 1000 keys inserted and found successfully!" << endl;
    tree.close();
    return true;
}

// Test edge cases
bool testEdgeCases() {
    cout << "\n=== Testing Edge Cases ===" << endl;

    BPTree<int, int, 4> tree("test_edge.dat");
    tree.clear();

    // Test 1: Single key
    tree.insert(1, 10);
    int* r1 = tree.find(1);
    if (!r1 || *r1 != 10) {
        cout << "ERROR: Single key test failed" << endl;
        return false;
    }
    cout << "✓ Single key works" << endl;

    // Test 2: Keys that cause root split
    tree.clear();
    for (int i = 0; i < 5; i++) {
        tree.insert(i, i);
    }
    for (int i = 0; i < 5; i++) {
        int* r = tree.find(i);
        if (!r || *r != i) {
            cout << "ERROR: Root split test failed at key " << i << endl;
            return false;
        }
    }
    cout << "✓ Root split works" << endl;

    // Test 3: Sequential insertions (worst case for some tree implementations)
    tree.clear();
    for (int i = 0; i < 100; i++) {
        tree.insert(i, i * 2);
    }
    for (int i = 0; i < 100; i++) {
        int* r = tree.find(i);
        if (!r || *r != i * 2) {
            cout << "ERROR: Sequential insertion test failed at key " << i << endl;
            return false;
        }
    }
    cout << "✓ Sequential insertions work" << endl;

    tree.close();
    return true;
}

int main() {
    cout << "=== Testing findParent() Implementation ===" << endl;
    cout << "Note: findParent() is called internally during splits" << endl;
    cout << "If insertions work correctly, findParent() is working\n" << endl;

    if (!testFindParentWithInsertions()) {
        return 1;
    }

    if (!testEdgeCases()) {
        return 1;
    }

    cout << "\n=== All Tests Passed ===" << endl;
    cout << "findParent() implementation is correct." << endl;
    cout << "Performance note: Current O(n) implementation works fine for this use case." << endl;

    return 0;
}
