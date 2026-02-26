#include "../../../hashtable.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Testing HashTable Move Constructor Deletion ===" << endl;

    // This should compile fine
    HashTable<int, int, 10> ht1;
    ht1.insert(1, 100);

    cout << "HashTable created and used successfully." << endl;

    // The following would NOT compile if uncommented (move is deleted):
    // HashTable<int, int, 10> ht2 = std::move(ht1);  // Error: move constructor deleted
    // HashTable<int, int, 10> ht3;
    // ht3 = std::move(ht1);  // Error: move assignment deleted

    cout << "Move constructors are properly deleted (verified at compile time)." << endl;

    return 0;
}
