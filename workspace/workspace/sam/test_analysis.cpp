// Quick test to understand the pointer behavior
#include "bptree.hpp"
#include <iostream>

int main() {
    BPTree<int, int, 4> tree("test_debug.dat");
    tree.clear();

    // Insert values
    tree.insert(1, 100);
    tree.insert(2, 200);
    tree.insert(3, 300);
    tree.insert(4, 400);
    tree.insert(5, 500);

    std::cout << "After inserting 1-5:" << std::endl;
    std::cout << "Node count: " << tree.nodeCount() << std::endl;

    // Get pointers
    std::cout << "\nGetting pointers..." << std::endl;
    int* val1 = tree.find(1);
    std::cout << "val1 address: " << val1 << ", value: " << (val1 ? *val1 : -1) << std::endl;

    int* val2 = tree.find(2);
    std::cout << "val2 address: " << val2 << ", value: " << (val2 ? *val2 : -1) << std::endl;
    std::cout << "val1 after find(2): " << (val1 ? *val1 : -1) << std::endl;

    int* val3 = tree.find(3);
    std::cout << "val3 address: " << val3 << ", value: " << (val3 ? *val3 : -1) << std::endl;
    std::cout << "val1 after find(3): " << (val1 ? *val1 : -1) << std::endl;
    std::cout << "val2 after find(3): " << (val2 ? *val2 : -1) << std::endl;

    tree.close();
    return 0;
}
