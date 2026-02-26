#ifndef BPTREE_HPP
#define BPTREE_HPP

#include "fileio.hpp"
#include <cstring>

/**
 * Disk-based B+ Tree for persistent storage
 * Template parameters:
 *   Key: Key type (must support <, >, ==, =)
 *   Value: Value type
 *   Degree: Maximum number of children per node (typical: 64-256)
 *
 * Features:
 * - All data stored on disk for memory efficiency
 * - Leaf nodes linked for efficient range queries
 * - Supports insert, find, remove, and range query operations
 * - Persistent across program restarts
 */
template<typename Key, typename Value, int Degree = 128>
class BPTree {
private:
    static const int MIN_KEYS = Degree / 2;
    static const int MAX_KEYS = Degree - 1;

    struct Node {
        bool is_leaf;
        int key_count;
        Key keys[MAX_KEYS];
        std::streampos children[Degree];  // For internal nodes: child positions
        Value values[MAX_KEYS];           // For leaf nodes: values
        std::streampos next_leaf;         // For leaf nodes: next leaf position
        std::streampos self_pos;          // Position of this node on disk

        Node() : is_leaf(true), key_count(0), next_leaf(-1), self_pos(-1) {
            for (int i = 0; i < Degree; i++) {
                children[i] = -1;
            }
        }
    };

    struct Header {
        std::streampos root_pos;
        int node_count;

        Header() : root_pos(-1), node_count(0) {}
    };

    FileIO file;
    std::string filename;
    Header header;
    std::streampos header_pos;
    mutable Node find_result_node;  // Cache for find() return value

    // Calculate position for node data (after header)
    std::streampos getNodePos(int node_id) const {
        return sizeof(Header) + static_cast<std::streampos>(node_id) * sizeof(Node);
    }

    // Load node from disk
    bool loadNode(Node& node, std::streampos pos) {
        if (pos < 0) return false;
        return file.read(node, pos);
    }

    // Save node to disk
    bool saveNode(const Node& node) {
        if (node.self_pos < 0) return false;
        return file.write(node, node.self_pos);
    }

    // Allocate new node on disk
    Node createNode(bool is_leaf) {
        Node node;
        node.is_leaf = is_leaf;
        node.self_pos = getNodePos(header.node_count++);
        return node;
    }

    // Find leaf node that should contain the key
    std::streampos findLeaf(const Key& key) {
        if (header.root_pos < 0) return -1;

        std::streampos current_pos = header.root_pos;
        Node current;

        while (true) {
            if (!loadNode(current, current_pos)) return -1;

            if (current.is_leaf) {
                return current_pos;
            }

            // Find child to descend to
            int i = 0;
            while (i < current.key_count && key >= current.keys[i]) {
                i++;
            }

            current_pos = current.children[i];
            if (current_pos < 0) return -1;
        }
    }

    // Insert into leaf node
    bool insertIntoLeaf(Node& leaf, const Key& key, const Value& value) {
        int i = 0;
        while (i < leaf.key_count && leaf.keys[i] < key) {
            i++;
        }

        // Check if key already exists
        if (i < leaf.key_count && leaf.keys[i] == key) {
            leaf.values[i] = value;
            return saveNode(leaf);
        }

        // Shift keys and values to make room
        for (int j = leaf.key_count; j > i; j--) {
            leaf.keys[j] = leaf.keys[j - 1];
            leaf.values[j] = leaf.values[j - 1];
        }

        leaf.keys[i] = key;
        leaf.values[i] = value;
        leaf.key_count++;

        return saveNode(leaf);
    }

    // Split leaf node
    std::streampos splitLeaf(Node& leaf) {
        Node new_leaf = createNode(true);
        int mid = (leaf.key_count + 1) / 2;

        // Move half of keys/values to new leaf
        new_leaf.key_count = leaf.key_count - mid;
        for (int i = 0; i < new_leaf.key_count; i++) {
            new_leaf.keys[i] = leaf.keys[mid + i];
            new_leaf.values[i] = leaf.values[mid + i];
        }

        leaf.key_count = mid;

        // Update leaf links
        new_leaf.next_leaf = leaf.next_leaf;
        leaf.next_leaf = new_leaf.self_pos;

        saveNode(leaf);
        saveNode(new_leaf);

        return new_leaf.self_pos;
    }

    // Find parent of a given node
    bool findParent(std::streampos child_pos, Node& parent, int& child_index) {
        if (child_pos == header.root_pos) return false;  // Root has no parent

        std::streampos current_pos = header.root_pos;
        Node current;

        while (true) {
            if (!loadNode(current, current_pos)) return false;
            if (current.is_leaf) return false;  // Shouldn't reach leaf without finding parent

            // Check if any child matches
            for (int i = 0; i <= current.key_count; i++) {
                if (current.children[i] == child_pos) {
                    parent = current;
                    child_index = i;
                    return true;
                }
            }

            // Find which child to descend to
            // Load first child to check if it's a leaf (would be parent level)
            Node first_child;
            if (loadNode(first_child, current.children[0])) {
                if (first_child.is_leaf) {
                    // Current is parent of leaves, but child not found
                    return false;
                }
            }

            // Descend to appropriate child (this is crude but works for finding parent)
            // We need to search all children at this level
            for (int i = 0; i <= current.key_count; i++) {
                if (current.children[i] >= 0) {
                    Node check_child;
                    if (loadNode(check_child, current.children[i])) {
                        // Check if child_pos is in subtree
                        if (!check_child.is_leaf) {
                            // Recursively search this subtree
                            if (findParentInSubtree(current.children[i], child_pos, parent, child_index)) {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }
    }

    // Helper to find parent in subtree
    bool findParentInSubtree(std::streampos subtree_root, std::streampos child_pos, Node& parent, int& child_index) {
        Node current;
        if (!loadNode(current, subtree_root)) return false;

        // Check if this node is the parent
        for (int i = 0; i <= current.key_count; i++) {
            if (current.children[i] == child_pos) {
                parent = current;
                child_index = i;
                return true;
            }
        }

        // If not leaf, check children
        if (!current.is_leaf) {
            for (int i = 0; i <= current.key_count; i++) {
                if (current.children[i] >= 0) {
                    if (findParentInSubtree(current.children[i], child_pos, parent, child_index)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // Split internal node
    std::streampos splitInternal(Node& node, Key& up_key) {
        Node new_node = createNode(false);
        int mid = node.key_count / 2;

        // Key that goes up to parent
        up_key = node.keys[mid];

        // Move keys and children to new node
        new_node.key_count = node.key_count - mid - 1;
        for (int i = 0; i < new_node.key_count; i++) {
            new_node.keys[i] = node.keys[mid + 1 + i];
            new_node.children[i] = node.children[mid + 1 + i];
        }
        new_node.children[new_node.key_count] = node.children[node.key_count];

        node.key_count = mid;

        saveNode(node);
        saveNode(new_node);

        return new_node.self_pos;
    }

    // Insert entry into parent
    bool insertIntoParent(std::streampos left_pos, const Key& key, std::streampos right_pos) {
        Node left;
        if (!loadNode(left, left_pos)) return false;

        // If left is root, create new root
        if (left_pos == header.root_pos) {
            Node new_root = createNode(false);
            new_root.keys[0] = key;
            new_root.children[0] = left_pos;
            new_root.children[1] = right_pos;
            new_root.key_count = 1;

            saveNode(new_root);
            header.root_pos = new_root.self_pos;
            saveHeader();
            return true;
        }

        // Find parent of left node
        Node parent;
        int child_index;
        if (!findParent(left_pos, parent, child_index)) return false;

        // Insert key and right_pos into parent
        // Find insertion position
        int i = parent.key_count;
        while (i > child_index && parent.keys[i - 1] > key) {
            i--;
        }

        // Check if parent has space
        if (parent.key_count < MAX_KEYS) {
            // Shift keys and children to make room
            for (int j = parent.key_count; j > i; j--) {
                parent.keys[j] = parent.keys[j - 1];
                parent.children[j + 1] = parent.children[j];
            }

            parent.keys[i] = key;
            parent.children[i + 1] = right_pos;
            parent.key_count++;

            return saveNode(parent);
        }

        // Parent is full, need to split
        // Create temporary arrays to hold all keys and children
        Key temp_keys[MAX_KEYS + 1];
        std::streampos temp_children[Degree + 1];

        // Copy all existing children first
        for (int k = 0; k <= parent.key_count; k++) {
            temp_children[k] = parent.children[k];
        }

        // Copy all existing keys
        for (int k = 0; k < parent.key_count; k++) {
            temp_keys[k] = parent.keys[k];
        }

        // Shift keys and children to make room for new entry at position i
        for (int k = parent.key_count; k > i; k--) {
            temp_keys[k] = temp_keys[k - 1];
            temp_children[k + 1] = temp_children[k];
        }

        // Insert new key and child pointer
        temp_keys[i] = key;
        temp_children[i + 1] = right_pos;

        // Now temp_keys has (parent.key_count + 1) keys
        // and temp_children has (parent.key_count + 2) children
        int total_keys = parent.key_count + 1;

        // Split at middle
        int mid = total_keys / 2;
        Key up_key = temp_keys[mid];

        // Update left parent: keys [0..mid), children [0..mid]
        parent.key_count = mid;
        for (int k = 0; k < mid; k++) {
            parent.keys[k] = temp_keys[k];
            parent.children[k] = temp_children[k];
        }
        parent.children[mid] = temp_children[mid];

        // Create new right parent: keys [mid+1..total_keys), children [mid+1..total_keys]
        Node new_parent = createNode(false);
        new_parent.key_count = total_keys - mid - 1;
        for (int k = 0; k < new_parent.key_count; k++) {
            new_parent.keys[k] = temp_keys[mid + 1 + k];
            new_parent.children[k] = temp_children[mid + 1 + k];
        }
        new_parent.children[new_parent.key_count] = temp_children[total_keys];

        saveNode(parent);
        saveNode(new_parent);

        // Recursively insert into parent's parent
        return insertIntoParent(parent.self_pos, up_key, new_parent.self_pos);
    }

    // Save header to disk
    void saveHeader() {
        file.write(header, 0);
    }

    // Load header from disk
    void loadHeader() {
        if (file.size() >= sizeof(Header)) {
            file.read(header, 0);
        } else {
            header = Header();
        }
    }

public:
    BPTree() {}

    explicit BPTree(const std::string& fname) {
        open(fname);
    }

    ~BPTree() {
        close();
    }

    // Prevent copying (would corrupt file handles)
    BPTree(const BPTree&) = delete;
    BPTree& operator=(const BPTree&) = delete;
    BPTree(BPTree&&) = delete;
    BPTree& operator=(BPTree&&) = delete;

    // Open B+ tree file
    bool open(const std::string& fname) {
        filename = fname;
        if (!file.open(filename)) return false;

        loadHeader();

        // Initialize if new file
        if (header.root_pos < 0) {
            Node root = createNode(true);
            saveNode(root);
            header.root_pos = root.self_pos;
            saveHeader();
        }

        return true;
    }

    // Close file
    void close() {
        if (file.isOpen()) {
            saveHeader();
            file.close();
        }
    }

    // Insert key-value pair
    bool insert(const Key& key, const Value& value) {
        if (header.root_pos < 0) return false;

        std::streampos leaf_pos = findLeaf(key);
        if (leaf_pos < 0) return false;

        Node leaf;
        if (!loadNode(leaf, leaf_pos)) return false;

        // If leaf is not full, just insert
        if (leaf.key_count < MAX_KEYS) {
            return insertIntoLeaf(leaf, key, value);
        }

        // Leaf is full, need to split
        // Create temporary arrays to hold all keys and values including new one
        Key temp_keys[MAX_KEYS + 1];
        Value temp_values[MAX_KEYS + 1];

        // Find insertion position and copy data
        int insert_pos = 0;
        while (insert_pos < leaf.key_count && leaf.keys[insert_pos] < key) {
            insert_pos++;
        }

        // Check if key already exists (update case)
        if (insert_pos < leaf.key_count && leaf.keys[insert_pos] == key) {
            leaf.values[insert_pos] = value;
            return saveNode(leaf);
        }

        // Copy keys and values, inserting new one at correct position
        for (int i = 0; i < insert_pos; i++) {
            temp_keys[i] = leaf.keys[i];
            temp_values[i] = leaf.values[i];
        }
        temp_keys[insert_pos] = key;
        temp_values[insert_pos] = value;
        for (int i = insert_pos; i < leaf.key_count; i++) {
            temp_keys[i + 1] = leaf.keys[i];
            temp_values[i + 1] = leaf.values[i];
        }

        // Split point
        int original_count = leaf.key_count;
        int mid = (original_count + 1) / 2;

        // Update left leaf
        leaf.key_count = mid;
        for (int i = 0; i < mid; i++) {
            leaf.keys[i] = temp_keys[i];
            leaf.values[i] = temp_values[i];
        }

        // Create new right leaf
        Node new_leaf = createNode(true);
        new_leaf.key_count = (original_count + 1) - mid;
        for (int i = 0; i < new_leaf.key_count; i++) {
            new_leaf.keys[i] = temp_keys[mid + i];
            new_leaf.values[i] = temp_values[mid + i];
        }

        // Update leaf links
        new_leaf.next_leaf = leaf.next_leaf;
        leaf.next_leaf = new_leaf.self_pos;

        saveNode(leaf);
        saveNode(new_leaf);

        // Insert separator key (first key of right leaf) into parent
        return insertIntoParent(leaf.self_pos, new_leaf.keys[0], new_leaf.self_pos);
    }

    // Find value by key
    Value* find(const Key& key) {
        std::streampos leaf_pos = findLeaf(key);
        if (leaf_pos < 0) return nullptr;

        if (!loadNode(find_result_node, leaf_pos)) return nullptr;

        // Binary search in leaf
        for (int i = 0; i < find_result_node.key_count; i++) {
            if (find_result_node.keys[i] == key) {
                return &find_result_node.values[i];
            }
        }

        return nullptr;
    }

    // Check if key exists
    bool contains(const Key& key) {
        return find(key) != nullptr;
    }

    // Remove key
    bool remove(const Key& key) {
        std::streampos leaf_pos = findLeaf(key);
        if (leaf_pos < 0) return false;

        Node leaf;
        if (!loadNode(leaf, leaf_pos)) return false;

        // Find key in leaf
        int i = 0;
        while (i < leaf.key_count && leaf.keys[i] < key) {
            i++;
        }

        if (i >= leaf.key_count || !(leaf.keys[i] == key)) {
            return false;  // Key not found
        }

        // Shift keys and values
        for (int j = i; j < leaf.key_count - 1; j++) {
            leaf.keys[j] = leaf.keys[j + 1];
            leaf.values[j] = leaf.values[j + 1];
        }

        leaf.key_count--;
        return saveNode(leaf);
    }

    // Clear all data (truncate file)
    void clear() {
        file.truncate();
        header = Header();

        // Create new root
        Node root = createNode(true);
        saveNode(root);
        header.root_pos = root.self_pos;
        saveHeader();
    }

    // Get number of nodes (for debugging)
    int nodeCount() const {
        return header.node_count;
    }
};

#endif // BPTREE_HPP
