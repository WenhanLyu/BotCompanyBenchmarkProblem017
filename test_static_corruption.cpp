#include "bptree.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Testing Static Memory Corruption ===" << endl;

    BPTree<int, int, 4> tree("test_corruption.dat");
    tree.clear();

    // Insert values
    tree.insert(1, 100);
    tree.insert(2, 200);
    tree.insert(3, 300);

    // Get pointer to first value
    int* ptr1 = tree.find(1);
    cout << "After find(1): *ptr1 = " << (ptr1 ? *ptr1 : -1) << " (expected: 100)" << endl;

    // Get pointer to second value (this overwrites static Node if bug exists)
    int* ptr2 = tree.find(2);
    cout << "After find(2): *ptr2 = " << (ptr2 ? *ptr2 : -1) << " (expected: 200)" << endl;

    // Check if ptr1 still points to correct value
    cout << "After find(2): *ptr1 = " << (ptr1 ? *ptr1 : -1) << " (should still be 100)" << endl;

    if (ptr1 && *ptr1 != 100) {
        cout << "\nBUG CONFIRMED: ptr1 was corrupted!" << endl;
        cout << "This happens because find() returns a pointer to a static Node." << endl;
        cout << "When find() is called again, it overwrites the same static Node." << endl;
        return 1;
    } else if (ptr1 && *ptr1 == 100) {
        cout << "\nNo corruption detected in this test." << endl;
        cout << "However, the bug still exists if keys are in the same leaf!" << endl;

        // Additional test: keys in same leaf
        tree.clear();
        for (int i = 0; i < 3; i++) {
            tree.insert(i, i * 10);
        }

        int* p0 = tree.find(0);
        int* p1 = tree.find(1);

        cout << "\nSame-leaf test:" << endl;
        cout << "p0 address: " << p0 << ", value: " << (p0 ? *p0 : -1) << endl;
        cout << "p1 address: " << p1 << ", value: " << (p1 ? *p1 : -1) << endl;

        if (p0 == p1 || (p0 && p1 && abs(p0 - p1) < 100)) {
            cout << "\nBUG CONFIRMED: Both pointers point to the same static Node!" << endl;
            return 1;
        }
    }

    tree.close();
    return 0;
}
