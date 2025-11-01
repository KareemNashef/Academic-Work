#ifndef UNIONFIND_H
#define UNIONFIND_H

// ===== Included headers and used namespaces =====

#include "Company.h"

// ================================================

class UnionFind {
    // Subclass for UnionFind nodes:
    struct Node {
        Company* Comp;

        double AddedValue;
        double AdjustedValue;

        Node* Parent;
        Node* Owner;
    };

    int Size;
    int* GroupSizes;
    Node** Companies;

public:
    // Constructor:
    UnionFind(int inSize);

    // Destructor:
    ~UnionFind();

    // Find - Returns the company ID of the group's root:
    int Find(int inCompID);

    // Owner - Returns the company ID of the group's owner:
    int Owner(int inCompID);

    // Union - Unites the groups of the buyer and the target:
    void Union(int inSmaller, int inBigger, int inBuyer, double inFactor);

    // GetUnionSize - Returns the size of the union:
    int GetUnionSize() const;

    // GetGroupSize - Returns the size of a specific group:
    int GetGroupSize(int inCompID);

    // GetValue - Returns the size of a specific company:
    double GetValue(int inCompID) const;

    // GetComp - Returns a pointer to the company associated with the given ID:
    Company* GetComp(int inCompID);
};

#endif
