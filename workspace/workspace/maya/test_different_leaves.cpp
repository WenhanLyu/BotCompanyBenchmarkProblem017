#include "../../../bptree.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Testing Different Leaves Scenario ===" << endl;

    BPTree<int, int, 4> tree("test_leaves.dat");
    tree.clear();

    // Insert enough values to create multiple leaves (degree=4 means 3 keys per leaf)
    for (int i = 0; i < 10; i++) {
        tree.insert(i, i * 100);
    }

    // Get two pointers from different leaves
    int* ptr_first = tree.find(0);  // In first leaf
    cout << "find(0) = " << (ptr_first ? *ptr_first : -1) << " (expected: 0)" << endl;

    int* ptr_last = tree.find(9);   // Likely in different leaf
    cout << "find(9) = " << (ptr_last ? *ptr_last : -1) << " (expected: 900)" << endl;

    // Check if first pointer is still valid
    cout << "Checking ptr_first after find(9): " << (ptr_first ? *ptr_first : -1) << endl;

    if (ptr_first && *ptr_first != 0) {
        cout << "\nLIMITATION: Pointer invalidated by second find() call" << endl;
        cout << "This is expected behavior - pointers are only valid until next find()" << endl;
    } else {
        cout << "\nBoth pointers still valid (keys likely in same leaf)" << endl;
    }

    tree.close();
    return 0;
}
