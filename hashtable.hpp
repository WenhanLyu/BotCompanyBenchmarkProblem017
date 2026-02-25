#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstring>

/**
 * Generic hash table implementation for in-memory key-value storage
 * Uses separate chaining (linked list) for collision resolution
 * Template parameters:
 *   Key: Key type (must support ==, =, and hash function)
 *   Value: Value type
 *   TableSize: Size of hash table (should be prime for better distribution)
 */
template<typename Key, typename Value, int TableSize = 10007>
class HashTable {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;

        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };

    Node* table[TableSize];
    int count;

    // Hash function for integer types
    int hash(int key) const {
        return (key % TableSize + TableSize) % TableSize;
    }

    // Hash function for long long types
    int hash(long long key) const {
        return ((key % TableSize) + TableSize) % TableSize;
    }

    // Hash function for string types
    int hash(const char* str) const {
        unsigned long h = 5381;
        int c;
        while ((c = *str++)) {
            h = ((h << 5) + h) + c; // h * 33 + c
        }
        return h % TableSize;
    }

    // Generic hash function wrapper
    template<typename K>
    int getHash(const K& key) const {
        return hash(key);
    }

public:
    HashTable() : count(0) {
        for (int i = 0; i < TableSize; i++) {
            table[i] = nullptr;
        }
    }

    ~HashTable() {
        clear();
    }

    // Insert or update key-value pair
    // Returns true if new key inserted, false if existing key updated
    bool insert(const Key& key, const Value& value) {
        int idx = getHash(key);
        Node* curr = table[idx];

        // Check if key already exists
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return false;
            }
            curr = curr->next;
        }

        // Insert new node at beginning of chain
        Node* newNode = new Node(key, value);
        newNode->next = table[idx];
        table[idx] = newNode;
        count++;
        return true;
    }

    // Find value by key
    // Returns pointer to value if found, nullptr otherwise
    Value* find(const Key& key) {
        int idx = getHash(key);
        Node* curr = table[idx];

        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }

        return nullptr;
    }

    // Const version of find
    const Value* find(const Key& key) const {
        int idx = getHash(key);
        Node* curr = table[idx];

        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }

        return nullptr;
    }

    // Remove key-value pair
    // Returns true if key found and removed, false otherwise
    bool remove(const Key& key) {
        int idx = getHash(key);
        Node* curr = table[idx];
        Node* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) {
                    table[idx] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }

        return false;
    }

    // Check if key exists
    bool contains(const Key& key) const {
        return find(key) != nullptr;
    }

    // Get number of elements
    int size() const {
        return count;
    }

    // Check if empty
    bool empty() const {
        return count == 0;
    }

    // Clear all elements
    void clear() {
        for (int i = 0; i < TableSize; i++) {
            Node* curr = table[i];
            while (curr != nullptr) {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
            table[i] = nullptr;
        }
        count = 0;
    }

    // Iterator support for traversing all elements
    class Iterator {
    private:
        HashTable* ht;
        int bucket_idx;
        Node* current;

        void advance() {
            if (current && current->next) {
                current = current->next;
                return;
            }

            bucket_idx++;
            while (bucket_idx < TableSize && ht->table[bucket_idx] == nullptr) {
                bucket_idx++;
            }

            if (bucket_idx < TableSize) {
                current = ht->table[bucket_idx];
            } else {
                current = nullptr;
            }
        }

    public:
        Iterator(HashTable* h, int idx, Node* node)
            : ht(h), bucket_idx(idx), current(node) {}

        bool operator!=(const Iterator& other) const {
            return current != other.current || bucket_idx != other.bucket_idx;
        }

        Iterator& operator++() {
            advance();
            return *this;
        }

        Node* operator->() {
            return current;
        }

        Node& operator*() {
            return *current;
        }
    };

    Iterator begin() {
        for (int i = 0; i < TableSize; i++) {
            if (table[i] != nullptr) {
                return Iterator(this, i, table[i]);
            }
        }
        return end();
    }

    Iterator end() {
        return Iterator(this, TableSize, nullptr);
    }
};

/**
 * Specialized hash table for string keys
 * Optimized for username/trainID lookups
 */
template<typename Value, int TableSize = 10007>
class StringHashTable {
private:
    struct Node {
        char key[25];  // Max length 20 + null terminator + padding
        Value value;
        Node* next;

        Node(const char* k, const Value& v) : value(v), next(nullptr) {
            strncpy(key, k, 24);
            key[24] = '\0';
        }
    };

    Node* table[TableSize];
    int count;

    int hash(const char* str) const {
        unsigned long h = 5381;
        int c;
        while ((c = *str++)) {
            h = ((h << 5) + h) + c;
        }
        return h % TableSize;
    }

public:
    StringHashTable() : count(0) {
        for (int i = 0; i < TableSize; i++) {
            table[i] = nullptr;
        }
    }

    ~StringHashTable() {
        clear();
    }

    bool insert(const char* key, const Value& value) {
        int idx = hash(key);
        Node* curr = table[idx];

        while (curr != nullptr) {
            if (strcmp(curr->key, key) == 0) {
                curr->value = value;
                return false;
            }
            curr = curr->next;
        }

        Node* newNode = new Node(key, value);
        newNode->next = table[idx];
        table[idx] = newNode;
        count++;
        return true;
    }

    Value* find(const char* key) {
        int idx = hash(key);
        Node* curr = table[idx];

        while (curr != nullptr) {
            if (strcmp(curr->key, key) == 0) {
                return &(curr->value);
            }
            curr = curr->next;
        }

        return nullptr;
    }

    const Value* find(const char* key) const {
        int idx = hash(key);
        Node* curr = table[idx];

        while (curr != nullptr) {
            if (strcmp(curr->key, key) == 0) {
                return &(curr->value);
            }
            curr = curr->next;
        }

        return nullptr;
    }

    bool remove(const char* key) {
        int idx = hash(key);
        Node* curr = table[idx];
        Node* prev = nullptr;

        while (curr != nullptr) {
            if (strcmp(curr->key, key) == 0) {
                if (prev == nullptr) {
                    table[idx] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }

        return false;
    }

    bool contains(const char* key) const {
        return find(key) != nullptr;
    }

    int size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    void clear() {
        for (int i = 0; i < TableSize; i++) {
            Node* curr = table[i];
            while (curr != nullptr) {
                Node* next = curr->next;
                delete curr;
                curr = next;
            }
            table[i] = nullptr;
        }
        count = 0;
    }
};

#endif // HASHTABLE_HPP
