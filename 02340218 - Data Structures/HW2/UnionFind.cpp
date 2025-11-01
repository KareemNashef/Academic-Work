#include "UnionFind.h"

// Constructor:
UnionFind::UnionFind(int inSize) {
    // Initializing values and allocating arrays:
    Size = inSize;
    GroupSizes = new int[inSize];
    Companies = new Node * [inSize];

    // Filling up the arrays:
    for (int i = 0; i < inSize; i++) {
        GroupSizes[i] = 1;
        Companies[i] = new Node;

        Companies[i]->Comp = new Company(i + 1, i + 1);
        Companies[i]->AddedValue = 0;
        Companies[i]->AdjustedValue = 0;
        Companies[i]->Parent = Companies[i];
        Companies[i]->Owner = Companies[i];
    }
}

// Destructor:
UnionFind::~UnionFind() {
    // Deleting the companies and the nodes:
    for (int i = 0; i < Size; i++) {
        delete Companies[i]->Comp;
        delete Companies[i];
    }

    // Deleting the arrays:
    delete[] GroupSizes;
    delete[] Companies;
}

// Find - Returns the company ID of the group's root:
int UnionFind::Find(int inCompID) {
    // Converting the ID to an index:
    inCompID--;

    // Initializing an iterating node:
    Node* Iter = Companies[inCompID];

    // Heading up to the top:
    while (Iter != Iter->Parent) {
        Iter = Iter->Parent;
    }

    // Returning the index:
    return Iter->Comp->getID();
}

// Owner - Returns the company ID of the group's owner:
int UnionFind::Owner(int inCompID) {
    // Converting the ID to an index:
    inCompID--;

    // Initializing an iterating node:
    Node* Iter = Companies[inCompID];

    // Heading up to the top:
    while (Iter != Iter->Owner) {
        Iter = Iter->Owner;
    }

    // Returning the index:
    return Iter->Comp->getID();
}

// Union - Unites the groups of the buyer and the target:
void UnionFind::Union(int inSmaller, int inBigger, int inBuyer, double inFactor) {

    // Getting the indexes for the owners:
    int SmallOwner = Owner(inSmaller);
    int BigOwner = Owner(inBigger);

    // Converting the IDs indexes:
    inSmaller--;
    inBigger--;
    inBuyer--;
    SmallOwner--;
    BigOwner--;

    // Adjusting the added values and the owner connections:
    if (inBuyer == BigOwner) {
        Companies[inBigger]->AddedValue += GetValue(SmallOwner + 1) * inFactor;
        Companies[inSmaller]->AdjustedValue -= (Companies[inBigger]->AddedValue);

        Companies[SmallOwner]->Owner = Companies[BigOwner];
        Companies[inSmaller]->Owner = Companies[BigOwner];
    }
    else {
        Companies[inSmaller]->AddedValue += GetValue(BigOwner + 1) * inFactor;
        Companies[inSmaller]->AdjustedValue -= (Companies[inBigger]->AddedValue);

        Companies[BigOwner]->Owner = Companies[SmallOwner];
        Companies[inBigger]->Owner = Companies[SmallOwner];
    }

    // Adjusting the parent connections:
    Companies[inSmaller]->Parent = Companies[inBigger];

    // Adjusting the sizes:
    GroupSizes[inBigger] += GroupSizes[inSmaller];
    GroupSizes[inSmaller] = 0;
}

// GetSize - Returns the size of the union:
int UnionFind::GetUnionSize() const { return Size; }

// GetGroupSize - Returns the size of a specific group:
int UnionFind::GetGroupSize(int inCompID) { return GroupSizes[Find(inCompID) - 1]; }

// GetValue - Returns the size of a specific company:
double UnionFind::GetValue(int inCompID) const {
    // Converting the ID to an index:
    inCompID--;

    // Initializing an iterating node:
    Node* Iter = Companies[inCompID];

    // Initializing a sum collector:
    double sum = Iter->Comp->getVal() + Iter->AddedValue + Iter->AdjustedValue;

    // Heading up to the top:
    while (Iter != Iter->Parent) {
        Iter = Iter->Parent;
        sum += Iter->AddedValue + Iter->AdjustedValue;
    }

    // Returning the index:
    return sum;
}

// GetComp - Returns a pointer to the company associated with the given ID:
Company* UnionFind::GetComp(int inCompID) { return Companies[inCompID - 1]->Comp; }
