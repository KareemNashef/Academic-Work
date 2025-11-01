#ifndef AVL_H
#define AVL_H

// ===== Included headers & defines =====
#include <iostream>
#include "Employee.h"

typedef enum { KeySmaller, KeyBigger, BackupSmaller, BackupBigger, Equal, NotEqual } Comparison;
#define FOUND 1
#define NOTFOUND -1
#define DONE 1
// ======================================

// ===== AVL Tree class =====

// Subclass for nodes:
struct AVL_Node {
    int Salary; // Employee salary
    int ID; // Employee ID
    int Grade; // Employee grade

    Employee* Emp; // Employee pointer

    // Pointers to neighboring nodes:
    AVL_Node* left;
    AVL_Node* parent;
    AVL_Node* right;

    // Internal info about the node:
    int Height;
    int SubNodes;  // Holds the number of nodes on the left subtree (including current)
    long long SubGrades;  // Holds the total of the grades in the left subtree (including current)

};

class AVL {
    int TreeSize; // The number of employees in the tree
    long long TotalGrades; // The total of employee grades in the tree
    AVL_Node* root; // The head of the tree

    // ===== Rotation functions =====

    // RotationCaller - Checks wether a rotation is needed and calls the proper one:
    AVL_Node* rotationCaller(AVL_Node* parent);

    // Left Left Rotation:
    AVL_Node* LLrotation(AVL_Node* parent);

    // Left Right Rotation:
    AVL_Node* LRrotation(AVL_Node* parent);

    // Right Left Rotation:
    AVL_Node* RLrotation(AVL_Node* parent);

    // Right Right Rotation:
    AVL_Node* RRrotation(AVL_Node* parent);

    // ===== Helping functions =====

    // GetHeight - Returns the height of the node:
    int getHeight(AVL_Node* parent);

    // GetMax - Returns the maximum between two numbers:
    int getMax(int in_a, int in_b);

    // GetBF - Returns the balance factor of the node:
    int getBF(AVL_Node* parent);

    // ===== Auxiliary functions =====

    void destructorAux(AVL_Node* current);
    bool nodeExistsAux(AVL_Node* current, int inSal, int inID);
    AVL_Node* AddNodeAux(AVL_Node* inNode, AVL_Node* inParent, int inSal, int inID, int inGrade, Employee* inEmp);
    AVL_Node* DeleteNodeAux(AVL_Node* toDelete, int inSal, int inID, int inGrade);
    void inOrderAux(Employee** dataArray, AVL_Node* current, int* count);
    AVL_Node* replaceAux(Employee** data, int start, int end, AVL_Node* prev, int depth);
    int fixSubnodes(AVL_Node* node, int count);
    long long fixSubgrades(AVL_Node* node, long long grade);
    int fixHeight(AVL_Node* node);
    void printBT(const std::string& prefix, const AVL_Node* node, bool isLeft);

public:

    // Constructor - Initializes an empty tree:
    AVL();

    // Destructor - Destroys the tree:
    ~AVL();

    // ===== Helping functions =====

    // GetSize - Returns the total number of nodes in the tree:
    int getSize();

    // GetTotal - Returns the total sum of grades in the tree:
    long long getTotal();

    // NodeExists - Checks if a node with the given key&backup exists:
    bool nodeExists(int inSal, int inID);

    // GetRank - Calculates the rank of a node and returns it:
    int getRank(int inSal, int inID);

    // GetLowerSum - Calculates the sum of the lower nodes and returns it:
    long long getLowerSum(int inSal, int inID);

    // GetIndex - Returns the data of a given node using search by rank:
    Employee* getByIndex(int inIndex);

    // GetClosest - Returns a pointer to the data of the node with a key higher or equal to the input:
    Employee* getClosest(int inSal, bool Orient);

    // ===== AVL functions =====

    // AddNode - Adds a node with the given data:
    StatusType AddNode(Employee* in_data);

    // DeleteNode - Deletes the node with the given data:
    StatusType DeleteNode(int inSal, int inID, int inGrade);

    // ===== Traversal functions =====

    // InOrder - sorts the data into the given array:
    StatusType inOrder(Employee** dataArray);

    // ===== Misc functions =====

    // Replace - Replaces the whole tree with a given SORTED array:
    StatusType replace(Employee** data, int size);

    // Compare - used finding nodes:
    Comparison compare(int Sal1, int ID1, int Sal2, int ID2);

    // Print
    void Print2D();


};

#endif