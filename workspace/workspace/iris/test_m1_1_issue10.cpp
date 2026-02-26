/**
 * M1.1 Verification - Issue #10
 *
 * Test: Multi-instance BPTree isolation
 * Validates that the fix for static Node in find() prevents
 * corruption between multiple BPTree instances.
 *
 * Expected: All tests pass, no interference between instances
 */

#include "../../../bptree.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <cstdio>

void test_multi_instance_basic() {
    std::cout << "Test 1: Basic multi-instance isolation..." << std::endl;

    // Create two separate BPTree instances
    BPTree<int, int> tree1("test_tree1.dat");
    BPTree<int, int> tree2("test_tree2.dat");

    // Insert different data into each tree
    tree1.insert(1, 100);
    tree1.insert(2, 200);
    tree1.insert(3, 300);

    tree2.insert(1, 111);
    tree2.insert(2, 222);
    tree2.insert(3, 333);

    // Find values from tree1
    int* val1 = tree1.find(1);
    assert(val1 != nullptr);
    assert(*val1 == 100); // Should be tree1's value

    // Find values from tree2
    int* val2 = tree2.find(1);
    assert(val2 != nullptr);
    assert(*val2 == 111); // Should be tree2's value, not corrupted by tree1

    // Verify tree1 still has correct values
    val1 = tree1.find(2);
    assert(val1 != nullptr);
    assert(*val1 == 200);

    // Verify tree2 still has correct values
    val2 = tree2.find(2);
    assert(val2 != nullptr);
    assert(*val2 == 222);

    std::cout << "✓ Basic multi-instance isolation PASSED" << std::endl;

    // Cleanup
    std::remove("test_tree1.dat");
    std::remove("test_tree2.dat");
}

void test_interleaved_operations() {
    std::cout << "\nTest 2: Interleaved find operations..." << std::endl;

    BPTree<int, std::string> treeA("test_treeA.dat");
    BPTree<int, std::string> treeB("test_treeB.dat");

    // Insert data
    treeA.insert(5, std::string("Alpha"));
    treeA.insert(10, std::string("Beta"));
    treeA.insert(15, std::string("Gamma"));

    treeB.insert(5, std::string("One"));
    treeB.insert(10, std::string("Two"));
    treeB.insert(15, std::string("Three"));

    // Perform rapid interleaved finds
    std::string* strA1 = treeA.find(5);
    std::string* strB1 = treeB.find(5);
    std::string* strA2 = treeA.find(10);
    std::string* strB2 = treeB.find(10);
    std::string* strA3 = treeA.find(15);
    std::string* strB3 = treeB.find(15);

    // Verify all pointers are valid and have correct values
    assert(strA1 != nullptr && *strA1 == "Alpha");
    assert(strB1 != nullptr && *strB1 == "One");
    assert(strA2 != nullptr && *strA2 == "Beta");
    assert(strB2 != nullptr && *strB2 == "Two");
    assert(strA3 != nullptr && *strA3 == "Gamma");
    assert(strB3 != nullptr && *strB3 == "Three");

    std::cout << "✓ Interleaved operations PASSED" << std::endl;

    // Cleanup
    std::remove("test_treeA.dat");
    std::remove("test_treeB.dat");
}

void test_rapid_successive_finds() {
    std::cout << "\nTest 3: Rapid successive finds on different instances..." << std::endl;

    BPTree<int, int> tree1("test_rapid1.dat");
    BPTree<int, int> tree2("test_rapid2.dat");
    BPTree<int, int> tree3("test_rapid3.dat");

    // Insert data into all three trees
    for (int i = 0; i < 20; i++) {
        tree1.insert(i, i * 10);
        tree2.insert(i, i * 100);
        tree3.insert(i, i * 1000);
    }

    // Perform rapid successive finds
    for (int i = 0; i < 20; i++) {
        int* v1 = tree1.find(i);
        int* v2 = tree2.find(i);
        int* v3 = tree3.find(i);

        assert(v1 != nullptr && *v1 == i * 10);
        assert(v2 != nullptr && *v2 == i * 100);
        assert(v3 != nullptr && *v3 == i * 1000);
    }

    std::cout << "✓ Rapid successive finds PASSED" << std::endl;

    // Cleanup
    std::remove("test_rapid1.dat");
    std::remove("test_rapid2.dat");
    std::remove("test_rapid3.dat");
}

void test_pointer_validity_same_instance() {
    std::cout << "\nTest 4: Pointer validity on same instance..." << std::endl;

    BPTree<int, int> tree("test_validity.dat");

    tree.insert(1, 100);
    tree.insert(2, 200);
    tree.insert(3, 300);

    // Get pointer from first find
    int* ptr1 = tree.find(1);
    assert(ptr1 != nullptr);
    assert(*ptr1 == 100);

    // Second find should invalidate ptr1 (expected behavior)
    int* ptr2 = tree.find(2);
    assert(ptr2 != nullptr);
    assert(*ptr2 == 200);

    // Note: ptr1 is now invalid - this is expected behavior
    // We document but don't test dereferencing invalid pointers

    std::cout << "✓ Pointer validity behavior documented" << std::endl;

    // Cleanup
    std::remove("test_validity.dat");
}

void test_persistence_between_instances() {
    std::cout << "\nTest 5: Persistence with multiple instances..." << std::endl;

    const char* filename = "test_persist_multi.dat";

    // Create first instance and insert data
    {
        BPTree<int, int> tree1(filename);
        tree1.insert(42, 999);
        tree1.insert(43, 888);
    }

    // Create second instance and verify data
    {
        BPTree<int, int> tree2(filename);
        int* val1 = tree2.find(42);
        int* val2 = tree2.find(43);

        assert(val1 != nullptr && *val1 == 999);
        assert(val2 != nullptr && *val2 == 888);
    }

    // Create third instance concurrently with fourth
    BPTree<int, int> tree3(filename);
    BPTree<int, int> tree4(filename);

    int* val3 = tree3.find(42);
    int* val4 = tree4.find(42);

    assert(val3 != nullptr && *val3 == 999);
    assert(val4 != nullptr && *val4 == 999);

    std::cout << "✓ Persistence with multiple instances PASSED" << std::endl;

    // Cleanup
    std::remove(filename);
}

int main() {
    std::cout << "=== M1.1 Issue #10 Verification Tests ===" << std::endl;
    std::cout << "Testing: Multi-instance BPTree isolation" << std::endl;
    std::cout << "Fix: Replaced static Node with instance member\n" << std::endl;

    try {
        test_multi_instance_basic();
        test_interleaved_operations();
        test_rapid_successive_finds();
        test_pointer_validity_same_instance();
        test_persistence_between_instances();

        std::cout << "\n=== ALL TESTS PASSED ✓ ===" << std::endl;
        std::cout << "Issue #10 fix is verified correct." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ TEST FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ TEST FAILED: Unknown error" << std::endl;
        return 1;
    }
}
