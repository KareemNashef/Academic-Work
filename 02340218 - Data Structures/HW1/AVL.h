#ifndef AVL_H
#define AVL_H

// ===== Included headers & defines =====

#include <iostream>

#include "library1.h"

typedef enum { KeySmaller, KeyBigger, BackupSmaller, BackupBigger, Equal, NotEqual } Comparison;

#define FOUND 1
#define NOTFOUND -1
#define DONE 1

// ===== AVL Tree class =====

template <class Data>
class AVL {
    // Subclass for nodes:
    class Node {
       public:
        int key;     // Holds the primary identifier
        int backup;  // Holds a backup identifier for when the primaries are equal
        Data* data;  // Holds a pointer to the data

        // Pointers to neighboring nodes:
        Node* left;
        Node* parent;
        Node* right;

        // Internal info about the node:
        int height;
        int subNodes;  // Holds the number of nodes on the left subtree (including current)
    };

    int size;    // Holds how many nodes are in the tree
    Node* root;  // Holds a pointer to the head of the tree

    // ===== Rotation functions =====

    // RotationCaller - Checks wether a rotation is needed and calls the proper one:
    Node* rotationCaller(Node* parent) {
        if (getBF(parent) == 2)  // Case: Left rotation
        {
            if (getBF(parent->left) == -1)
                return LRrotation(parent);
            else
                return LLrotation(parent);
        } else if (getBF(parent) == -2) {  // Case: Right rotation
            if (getBF(parent->right) == 1)
                return RLrotation(parent);
            else
                return RRrotation(parent);
        }
        return parent;
    }

    // Left Left Rotation:
    Node* LLrotation(Node* parent) {
        // New variables for easier readability:
        Node* oldTop;
        Node* newTop;

        // Moving the values to the variables:
        oldTop = parent;
        newTop = oldTop->left;

        // LL Rotating:
        oldTop->left = newTop->right;
        newTop->right = oldTop;

        // Adjusting height:
        oldTop->height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
        newTop->height = getMax(getHeight(newTop->left), getHeight(newTop->right)) + 1;

        // Adjusting subnode count:
        oldTop->subNodes -= newTop->subNodes;

        return newTop;
    }

    // Left Right Rotation:
    Node* LRrotation(Node* parent) {
        // New variables for easier readability:
        Node* oldTop;     // Now new left
        Node* oldMiddle;  // Now new right
        Node* oldBottom;  // now new top

        // Moving the values to the variables:
        oldTop = parent;
        oldMiddle = oldTop->left;
        oldBottom = oldMiddle->right;

        // RL Rotating
        oldTop->left = oldBottom->right;
        oldMiddle->right = oldBottom->left;
        oldBottom->right = oldTop;
        oldBottom->left = oldMiddle;

        // Adjusting height:
        oldTop->height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
        oldMiddle->height = getMax(getHeight(oldMiddle->left), getHeight(oldMiddle->right)) + 1;
        oldBottom->height = getMax(getHeight(oldBottom->left), getHeight(oldBottom->right)) + 1;

        // Adjusting subnode count:
        oldBottom->subNodes += oldMiddle->subNodes;
        oldTop->subNodes -= oldBottom->subNodes;

        return oldBottom;
    }

    // Right Left Rotation:
    Node* RLrotation(Node* parent) {
        // New variables for easier readability:
        Node* oldTop;     // Now new left
        Node* oldMiddle;  // Now new right
        Node* oldBottom;  // now new top

        // Moving the values to the variables:
        oldTop = parent;
        oldMiddle = oldTop->right;
        oldBottom = oldMiddle->left;

        // RL Rotating
        oldTop->right = oldBottom->left;
        oldMiddle->left = oldBottom->right;
        oldBottom->left = oldTop;
        oldBottom->right = oldMiddle;

        // Adjusting height:
        oldTop->height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
        oldMiddle->height = getMax(getHeight(oldMiddle->left), getHeight(oldMiddle->right)) + 1;
        oldBottom->height = getMax(getHeight(oldBottom->left), getHeight(oldBottom->right)) + 1;

        // Adjusting subnode count:
        oldMiddle->subNodes -= oldBottom->subNodes;
        oldBottom->subNodes += oldTop->subNodes;

        return oldBottom;
    }

    // Right Right Rotation:
    Node* RRrotation(Node* parent) {
        // New variables for easier readability:
        Node* oldTop;
        Node* newTop;

        // Moving the values to the variables:
        oldTop = parent;
        newTop = oldTop->right;

        // RR Rotating
        oldTop->right = newTop->left;
        newTop->left = oldTop;

        // Adjusting height:
        oldTop->height = getMax(getHeight(oldTop->left), getHeight(oldTop->right)) + 1;
        newTop->height = getMax(getHeight(newTop->left), getHeight(newTop->right)) + 1;

        // Adjusting subnode count:
        newTop->subNodes += oldTop->subNodes;

        return newTop;
    }

    // ===== Helping functions =====

    // GetHeight - Returns the height of the node:
    int getHeight(Node* parent) {
        // Check if the node exists:
        if (parent == nullptr) return 0;

        // Return the node height:
        return parent->height;
    }

    // GetMax - Returns the maximum between two numbers:
    int getMax(int in_a, int in_b) {
        if (in_a > in_b)
            return in_a;
        else
            return in_b;
    }

    // GetBF - Returns the balance factor of the node:
    int getBF(Node* parent) {
        if (parent == nullptr) return 0;

        return getHeight(parent->left) - getHeight(parent->right);
    }

    // Compare - Compares keys and backups if needed:
    Comparison compare(int key1, int backup1, int key2, int backup2, bool useBackup) {
        if (key1 < key2) return KeyBigger;
        if (key1 > key2) return KeySmaller;
        if (key1 == key2 && useBackup == true) {
            if (backup1 < backup2) return BackupBigger;
            if (backup1 > backup2) return BackupSmaller;
            if (backup1 == backup2) return Equal;
        }
        if (key1 == key2 && useBackup == false) return Equal;

        return NotEqual;
    }

    // ===== Auxiliary functions =====

    void destructorAux(Node* current) {
        if (current == nullptr) return;

        destructorAux(current->left);
        destructorAux(current->right);
        delete current;
    }

    bool nodeExistsAux(Node* current, int key, int backup, bool useBackup) {
        // Checking if we reached the end of the tree:
        if (current == nullptr) return false;

        if (current->key < key)
            return nodeExistsAux(current->right, key, backup, useBackup);
        else if (current->key > key)
            return nodeExistsAux(current->left, key, backup, useBackup);
        else if (current->key == key && useBackup == true) {
            if (current->backup < backup)
                return nodeExistsAux(current->left, key, backup, useBackup);
            else if (current->backup > backup)
                return nodeExistsAux(current->right, key, backup, useBackup);
            else if (current->backup == backup)
                return true;
        } else if (current->key == key && useBackup == false)
            return true;

        return false;
    }

    Node* AddNodeAux(Node* in_node, Node* in_parent, int key, int backup, Data* in_data) {
        // Checking if the current node is the last:
        if (in_node == nullptr) {
            // Allocating a new node:
            Node* newNode = new Node;
            if (newNode == nullptr) throw ALLOCATION_ERROR;

            // Moving the data to the new node:
            newNode->key = key;
            newNode->backup = backup;
            newNode->data = in_data;

            // Setting the new node:
            in_node = newNode;

            in_node->left = nullptr;
            in_node->parent = in_parent;
            in_node->right = nullptr;
            in_node->height = 1;
            in_node->subNodes = 1;
            size++;
            return in_node;
        } else {
            switch (compare(in_node->key, in_node->backup, key, backup, true)) {
                case KeySmaller: {
                    in_node->subNodes++;
                    in_node->left = AddNodeAux(in_node->left, in_node, key, backup, in_data);
                    break;
                }
                case KeyBigger: {
                    in_node->right = AddNodeAux(in_node->right, in_node, key, backup, in_data);
                    break;
                }
                case BackupSmaller: {
                    in_node->right = AddNodeAux(in_node->right, in_node, key, backup, in_data);
                    break;
                }
                case BackupBigger: {
                    in_node->subNodes++;
                    in_node->left = AddNodeAux(in_node->left, in_node, key, backup, in_data);
                    break;
                }
                case Equal:
                    break;
                case NotEqual:
                    break;
            }
        }

        in_node->height = getMax(getHeight(in_node->left), getHeight(in_node->right)) + 1;

        in_node = rotationCaller(in_node);

        return in_node;
    }

    Node* DeleteNodeAux(Node* to_delete, int key, int backup, bool useBackup) {
        // Check if the current node exists:
        if (to_delete == nullptr) return to_delete;

        switch (compare(to_delete->key, to_delete->backup, key, backup, useBackup)) {
            case KeySmaller: {
                to_delete->subNodes--;
                to_delete->left = DeleteNodeAux(to_delete->left, key, backup, useBackup);
                break;
            }
            case KeyBigger:
                to_delete->right = DeleteNodeAux(to_delete->right, key, backup, useBackup);
                break;
            case BackupSmaller:
                to_delete->right = DeleteNodeAux(to_delete->right, key, backup, useBackup);
                break;
            case BackupBigger: {
                to_delete->subNodes--;
                to_delete->left = DeleteNodeAux(to_delete->left, key, backup, useBackup);
                break;
            }
            case Equal: {
                // Found the node:

                // Checking for a suitable replacement node:
                if (to_delete->left == nullptr || to_delete->right == nullptr) {
                    // Moving a son of the deleted node to a temporary one:
                    Node* temp_delete = nullptr;
                    if (to_delete->left == nullptr)
                        temp_delete = to_delete->right;
                    else if (to_delete->right == nullptr)
                        temp_delete = to_delete->left;

                    // Checking if both of the sons are null:
                    if (temp_delete == nullptr) {
                        temp_delete = to_delete;
                        delete temp_delete;  // The node does not need a replacement
                        to_delete = nullptr;
                    } else {
                        *to_delete = *temp_delete;  // Moving the replacement to the deleted location &
                                                    // replacing the content

                        // Freeing the node that was used to replace:
                        delete temp_delete;
                    }

                    size--;
                } else {
                    // Looking for a suitable replacement:
                    Node* temp_delete = to_delete->right;  // Moving one node to the right
                    while (temp_delete->left != nullptr)   // Moving all the way to the left
                        temp_delete = temp_delete->left;

                    // Moving the data from the replacement node:
                    to_delete->key = temp_delete->key;
                    to_delete->backup = temp_delete->backup;
                    to_delete->data = temp_delete->data;
                    to_delete->right =
                        DeleteNodeAux(to_delete->right, temp_delete->key, temp_delete->backup, useBackup);
                }
            }
            case NotEqual:
                break;
        }

        // Checking if the node got completly deleted:
        if (to_delete == nullptr) return to_delete;

        to_delete->height = getMax(getHeight(to_delete->left), getHeight(to_delete->right)) + 1;

        return rotationCaller(to_delete);
    }

    void inOrderAux(Data** dataArray, Node* current, int* count) {
        if (current == nullptr) return;

        inOrderAux(dataArray, current->left, count);
        dataArray[*count] = current->data;
        (*count)++;
        inOrderAux(dataArray, current->right, count);
    }

    void inOrderBoundedNumAux(Data** dataArray, Node* current, int* count, int num) {
        if (current == nullptr) return;
        inOrderBoundedNumAux(dataArray, current->left, count, num);

        dataArray[*count] = current->data;
        (*count)++;
        if (num == (*count)) throw DONE;

        inOrderBoundedNumAux(dataArray, current->right, count, num);
    }

    Node* replaceAux(int* keys, int* backups, Data** data, int start, int end, Node* prev, int depth) {
        // End condition:
        if (start > end) return nullptr;

        // Get the middle and make it the root:
        int middle = (start + end) / 2;
        Node* head = new Node;
        if (head == nullptr) throw ALLOCATION_ERROR;

        head->key = keys[middle];
        head->backup = backups[middle];
        head->data = data[middle];

        head->parent = prev;

        if (depth == 0)
            head->subNodes = middle;
        else
            head->subNodes = prev->subNodes / 2;

        // Building the left side:
        head->left = replaceAux(keys, backups, data, start, middle - 1, head, depth + 1);
        // Building the right side:
        head->right = replaceAux(keys, backups, data, middle + 1, end, head, depth + 1);

        return head;
    }
    int fixSubnodes(Node* node, int count) {
        if (node == NULL) {
            return 0;
        }

        count = fixSubnodes(node->left, count) + 1;

        node->subNodes = count;

        return count + fixSubnodes(node->right, count);
    }
    int fixHeight(Node* node) {
        if (node == NULL) return 0;

        node->height = getMax(fixHeight(node->left), fixHeight(node->right)) + 1;

        return node->height;
    }

    void getRangeAux(Data** dataArray, Node* current, int min, int max, int* count, int total) {
        if (current == NULL) return;

        if (current->key >= min) getRangeAux(dataArray, current->left, min, max, count, total);
        if (current->key >= min && current->key <= max) {
            dataArray[*count] = current->data;
            (*count)++;
            if ((*count) == total) throw DONE;
        }
        if (current->key <= max) getRangeAux(dataArray, current->right, min, max, count, total);

        return;
    }

   public:
    // Constructor - Initializes an empty tree:
    AVL() : size(0), root(nullptr) {}

    // Destructor - Destroys the tree:
    ~AVL() {
        // Using a recursive function that goes over the tree postorder to help deleting:
        destructorAux(root);
    }

    // ===== Helping functions =====

    // GetSize - Returns the total ammount of nodes in the tree:
    int getSize() { return size; }

    // NodeExists - Checks if a node with the given key&backup exists:
    bool nodeExists(int key, int backup, bool useBackup) {
        // Using a recursive function that searches for the node in a binary algo:
        return nodeExistsAux(root, key, backup, useBackup);
    }

    // GetRank - Calculates the rank of a node and returns it:
    int getRank(int key) {
        // Getting to the node that holds the given key:
        Node* iter = root;
        int rank = 0;

        while (iter->key != key) {
            if (iter->key > key)
                iter = iter->left;
            else if (iter->key < key) {
                rank += iter->subNodes;
                iter = iter->right;
            }
        }

        return rank + iter->subNodes;
    }

    // GetIndex - Returns the data of a given node using search by rank:
    Data* getByIndex(int index) {
        // Getting to the node that holds the given index:
        Node* iter = root;
        index++;  // Incrementing the index as subnodes also include the nodes themselves

        while (iter != nullptr) {
            if (iter->subNodes > index)
                iter = iter->left;
            else if (iter->subNodes < index) {
                index -= iter->subNodes;
                iter = iter->right;
            } else if (iter->subNodes == index)
                break;
        }

        return iter->data;
    }

    // GetData - Saves the data from a specific key into the output:
    int getData(int key, Data** output) {
        // Create a temporary pointer to lead to the given index :
        Node* iter = root;
        while (iter != nullptr) {
            if (iter->key > key) {  // Case: Input is lower
                if (iter->left == nullptr) return NOTFOUND;

                iter = iter->left;
            } else if (iter->key < key) {  // Case: Input is higher
                if (iter->right == nullptr) return NOTFOUND;

                iter = iter->right;
            } else  // Case: Input was found
                break;
        }

        if (iter == nullptr) return NOTFOUND;

        // Return the data from the node :
        *output = iter->data;
        return FOUND;
    }

    // GetFirst - Returns a pointer to the data of the lowest node:
    Data* getFirst() {
        // Create a temporary pointer to lead to the first node from the left :
        Node* iter = root;

        while (iter != nullptr) {
            if (iter->left == nullptr) break;

            iter = iter->left;
        }

        // Return the data from the node :
        return iter->data;
    }

    // GetClosest - Returns a pointer to the data of the node with a key higher or equal to the input:
    Data* getClosest(int key) {
        Node* iter = root;
        while (iter != nullptr) {
            if (iter->key > key) {  // Case: Input is lower
                if (iter->left == nullptr) break;

                iter = iter->left;
            } else if (iter->key < key) {  // Case: Input is higher
                if (iter->right == nullptr) break;

                iter = iter->right;
            } else  // Case: Input was found
                break;
        }

        // Return the data from the node :
        return iter->data;
    }

    // GetLast - Returns a pointer to the data of the highest node:
    Data* getLast() {
        // Create a temporary pointer to lead to the last node on the right :
        Node* iter = root;

        while (iter != nullptr) {
            if (iter->right == nullptr) break;

            iter = iter->right;
        }

        // Return the data from the node :
        return iter->data;
    }

    // GetRange - Fills an array with the data from a specific range of keys:
    StatusType getRange(Data** dataArray, int min, int max, int total) {
        int* counter = new int;
        if (counter == nullptr) return ALLOCATION_ERROR;

        *counter = 0;

        try {
            getRangeAux(dataArray, root, min, max, counter, total);
        } catch (int done) {
            delete counter;
            if (done == DONE) return SUCCESS;
        }

        return SUCCESS;
    }

    // ===== AVL functions =====

    // AddNode - Adds a node with the given data:
    StatusType AddNode(int key, int backup, bool useBackup, Data* in_data) {
        // Returns FAILURE on Existing items
        // Returns ALLOCATION_ERROR on malloc issues
        // Returns SUCCESS on successful add

        // Checking if the key already exits:
        if (nodeExists(key, backup, useBackup) == true) return FAILURE;

        // Using a recursive function that starts in root to delete the node:
        try {
            root = AddNodeAux(root, root, key, backup, in_data);
        } catch (StatusType result) {
            if (result != SUCCESS) return result;
        }
        return SUCCESS;
    }

    // DeleteNode - Deletes the node with the given data:
    StatusType DeleteNode(int key, int backup, bool useBackup) {
        // Returns FAILURE on Non-Existing items
        // Returns ALLOCATION_ERROR on malloc issues
        // Returns SUCCESS on successful add

        // Checking if the node exits:
        if (nodeExists(key, backup, useBackup) == false) return FAILURE;

        // Using a recursive function that starts in root to delete the node:
        try {
            root = DeleteNodeAux(root, key, backup, useBackup);
        } catch (StatusType result) {
            if (result != SUCCESS) return result;
        }
        return SUCCESS;
    }

    // ===== Traversal functions =====

    // InOrder - sorts the data into the given array:
    StatusType inOrder(Data** dataArray) {
        int* counter = new int;
        if (counter == nullptr) return ALLOCATION_ERROR;

        *counter = 0;
        inOrderAux(dataArray, root, counter);
        delete counter;

        return SUCCESS;
    }

    int inOrderBoundedNum(Data** dataArray, int num) {
        int* counter = new int;
        if (counter == nullptr) return ALLOCATION_ERROR;

        *counter = 0;
        try {
            inOrderBoundedNumAux(dataArray, root, counter, num);
        } catch (int done) {
            int ret = *counter;
            delete counter;
            return ret;
        }

        return 0;
    }

    // ===== Misc functions =====

    // Replace - Replaces the whole tree with a given SORTED array:
    StatusType replace(int* keys, int* backups, Data** data, int size) {
        // Deleting whatever was in the tree before:
        destructorAux(root);
        root = nullptr;

        // Using a recursive function that adds each node with O(1) complexity:
        try {
            root = replaceAux(keys, backups, data, 0, size - 1, nullptr, 0);
            fixSubnodes(root, 0);
            fixHeight(root);
        } catch (StatusType result) {
            if (result != SUCCESS) return result;
        }

        return SUCCESS;
    }
};

#endif