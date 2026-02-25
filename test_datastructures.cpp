#include <iostream>
#include "hashtable.hpp"
#include "bptree.hpp"
#include "types.hpp"
#include "fileio.hpp"

void testHashTable() {
    std::cout << "Testing HashTable..." << std::endl;

    // Test with string keys and int values
    StringHashTable<int> table;

    // Insert
    table.insert("user1", 100);
    table.insert("user2", 200);
    table.insert("user3", 300);

    // Find
    int* val = table.find("user1");
    if (val && *val == 100) {
        std::cout << "  ✓ Insert and find working" << std::endl;
    } else {
        std::cout << "  ✗ Insert/find failed" << std::endl;
    }

    // Update
    table.insert("user1", 150);
    val = table.find("user1");
    if (val && *val == 150) {
        std::cout << "  ✓ Update working" << std::endl;
    } else {
        std::cout << "  ✗ Update failed" << std::endl;
    }

    // Remove
    table.remove("user2");
    if (!table.contains("user2")) {
        std::cout << "  ✓ Remove working" << std::endl;
    } else {
        std::cout << "  ✗ Remove failed" << std::endl;
    }

    // Size
    if (table.size() == 2) {
        std::cout << "  ✓ Size tracking working" << std::endl;
    } else {
        std::cout << "  ✗ Size tracking failed (expected 2, got " << table.size() << ")" << std::endl;
    }

    std::cout << std::endl;
}

void testBPTree() {
    std::cout << "Testing B+ Tree..." << std::endl;

    // Test with int keys and int values
    BPTree<int, int, 8> tree("test_tree.dat");

    // Insert
    tree.insert(10, 100);
    tree.insert(20, 200);
    tree.insert(30, 300);
    tree.insert(5, 50);

    // Find
    int* val = tree.find(10);
    if (val && *val == 100) {
        std::cout << "  ✓ Insert and find working" << std::endl;
    } else {
        std::cout << "  ✗ Insert/find failed" << std::endl;
    }

    // Find another
    val = tree.find(5);
    if (val && *val == 50) {
        std::cout << "  ✓ Multiple inserts working" << std::endl;
    } else {
        std::cout << "  ✗ Multiple inserts failed" << std::endl;
    }

    // Close and reopen (test persistence)
    tree.close();
    BPTree<int, int, 8> tree2("test_tree.dat");

    val = tree2.find(20);
    if (val && *val == 200) {
        std::cout << "  ✓ Persistence working" << std::endl;
    } else {
        std::cout << "  ✗ Persistence failed" << std::endl;
    }

    // Remove
    tree2.remove(20);
    if (!tree2.contains(20)) {
        std::cout << "  ✓ Remove working" << std::endl;
    } else {
        std::cout << "  ✗ Remove failed" << std::endl;
    }

    tree2.close();
    std::cout << std::endl;
}

void testTypes() {
    std::cout << "Testing Data Types..." << std::endl;

    // Test User
    User user("alice", "pass123", "Alice", "alice@example.com", 5);
    if (strcmp(user.username, "alice") == 0 && user.privilege == 5) {
        std::cout << "  ✓ User creation working" << std::endl;
    } else {
        std::cout << "  ✗ User creation failed" << std::endl;
    }

    if (user.checkPassword("pass123")) {
        std::cout << "  ✓ Password check working" << std::endl;
    } else {
        std::cout << "  ✗ Password check failed" << std::endl;
    }

    // Test Date
    Date date("06-15");
    if (date.month == 6 && date.day == 15) {
        std::cout << "  ✓ Date parsing working" << std::endl;
    } else {
        std::cout << "  ✗ Date parsing failed" << std::endl;
    }

    Date date1("06-15");
    Date date2("07-20");
    if (date1 < date2) {
        std::cout << "  ✓ Date comparison working" << std::endl;
    } else {
        std::cout << "  ✗ Date comparison failed" << std::endl;
    }

    // Test Time
    Time time("14:30");
    if (time.hour == 14 && time.minute == 30) {
        std::cout << "  ✓ Time parsing working" << std::endl;
    } else {
        std::cout << "  ✗ Time parsing failed" << std::endl;
    }

    int mins = time.toMinutes();
    if (mins == 14 * 60 + 30) {
        std::cout << "  ✓ Time to minutes working" << std::endl;
    } else {
        std::cout << "  ✗ Time to minutes failed" << std::endl;
    }

    std::cout << std::endl;
}

void testFileIO() {
    std::cout << "Testing File I/O..." << std::endl;

    // Test basic file operations
    FileIO file("test_file.dat");

    struct TestData {
        int id;
        char name[20];
        double value;
    };

    TestData data1 = {1, "test", 3.14};

    // Write
    if (file.write(data1, 0)) {
        std::cout << "  ✓ Write working" << std::endl;
    } else {
        std::cout << "  ✗ Write failed" << std::endl;
    }

    // Read
    TestData data2;
    if (file.read(data2, 0)) {
        if (data2.id == 1 && strcmp(data2.name, "test") == 0) {
            std::cout << "  ✓ Read working" << std::endl;
        } else {
            std::cout << "  ✗ Read data mismatch" << std::endl;
        }
    } else {
        std::cout << "  ✗ Read failed" << std::endl;
    }

    file.close();
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Data Structure Tests ===" << std::endl << std::endl;

    testHashTable();
    testBPTree();
    testTypes();
    testFileIO();

    std::cout << "=== Tests Complete ===" << std::endl;

    return 0;
}
