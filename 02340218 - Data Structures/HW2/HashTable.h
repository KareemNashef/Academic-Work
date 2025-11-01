#ifndef HASHTABLE_H
#define HASHTABLE_H

// ===== Included headers and defines =====

#include "Employee.h"
#include <iostream>

#define CAPACITY 64

// ========================================

// ===== Supporting functions =====

int HashFunctionFirst(int inID, int size);
int HashFunctionSecond(int inID, int size);

typedef enum { AVAILABLE, NOTAVAILABLE, DELETED } NodeStatus;

// ================================

class HashTable {
    // Subclass for entries:
    struct Node {
        Employee* Emp;
        NodeStatus Status;
    };

    // ExpandTable - Expands the table to double the size:
    StatusType ExpandTable();

    // ShrinkTable - Shrinks the table to half the size:
    StatusType ShrinkTable();

    // RehashTable - Replaces the current table with a new one and re-inserts the employees to it:
    StatusType RehashTable(Node** destTable, int oldSize, int newSize);

    Node** Employees;
    int TotalSize;
    int CurrentSize;

public:
    // Constructor:
    HashTable();

    // Destructor:
    ~HashTable();

    // Insert - Inserts an employee to the table:
    StatusType Insert(Employee* inEmp);

    // Remove - Deletes an employee from the table:
    StatusType Remove(int inID);

    // Find - Returns a pointer to the employee associated with the given ID:
    Employee* Find(int inID);

    // GetSize - Returns the current filled size of the table:
    int GetSize() const;

    // FreeAllEmployees - Executes the delete function on every employee in the table:
    void FreeAllEmployees();

    // GetEmployees - Fills a given array with pointers to all of the employees:
    void GetEmployees(Employee** toFill);

    // ReplaceArray - Replaces the current array with the data from a given array:
    void ReplaceArray(Employee** toReplace, int inSize);

    // ===== Testing =====
    void Printshit();
};

#endif
