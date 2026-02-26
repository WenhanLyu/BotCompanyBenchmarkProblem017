#include "../../../bptree.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Testing Multi-Instance Scenario ===" << endl;

    // Create two separate B+ trees
    BPTree<int, int, 4> tree1("test1.dat");
    BPTree<int, int, 4> tree2("test2.dat");

    tree1.clear();
    tree2.clear();

    // Insert different values
    tree1.insert(1, 100);
    tree2.insert(1, 200);

    // Get pointers from both trees
    int* ptr1 = tree1.find(1);
    int* ptr2 = tree2.find(1);

    cout << "tree1.find(1) = " << (ptr1 ? *ptr1 : -1) << " (expected: 100)" << endl;
    cout << "tree2.find(1) = " << (ptr2 ? *ptr2 : -1) << " (expected: 200)" << endl;

    // With static variable, ptr1 would be corrupted by tree2.find()
    // With instance variable, both should be correct
    if (ptr1 && ptr2 && *ptr1 == 100 && *ptr2 == 200) {
        cout << "\nSUCCESS: Multi-instance fix works!" << endl;
        cout << "Each BPTree instance has its own find_result_node." << endl;
        tree1.close();
        tree2.close();
        return 0;
    } else {
        cout << "\nFAILURE: Multi-instance corruption detected!" << endl;
        tree1.close();
        tree2.close();
        return 1;
    }
}
