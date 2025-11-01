#include "HashTable.h"

// ===== Supporting functions =====

int HashFunctionFirst(int inID, int size) { return (inID % size); }

int HashFunctionSecond(int inID, int size) { 
    
    unsigned long long mult = (unsigned long long)((unsigned long long)inID * (unsigned long long)size);
    
    mult--;

    mult = mult % size;

    return mult;
}

// ================================

// ExpandTable - Expands the table to double the size:
StatusType HashTable::ExpandTable() {
    // Creating a new table with double the size of the previous one:
    Node** ExpandedTable = new Node * [TotalSize * 2];
    if (ExpandedTable == nullptr) {
        return ALLOCATION_ERROR;
    }

    // Filling the table with available slots:
    for (int i = 0; i < (TotalSize * 2); i++) {
        ExpandedTable[i] = new Node();
        ExpandedTable[i]->Emp = nullptr;
        ExpandedTable[i]->Status = AVAILABLE;
    }

    // Rehashing the data into the new table:
    StatusType rehash = RehashTable(ExpandedTable, TotalSize, TotalSize * 2);
    if (rehash != SUCCESS) {
        for (int i = 0; i < (TotalSize * 2); i++) {
            delete ExpandedTable[i];
        }
        delete[] ExpandedTable;
        return rehash;
    }

    return SUCCESS;
}

// ShrinkTable - Shrinks the table to half the size:
StatusType HashTable::ShrinkTable() {
    // Creating a new table with double the size of the previous one:
    Node** ShrunkTable = new Node * [TotalSize / 2];
    if (ShrunkTable == nullptr) {
        return ALLOCATION_ERROR;
    }

    // Filling the table with available slots:
    for (int i = 0; i < (TotalSize / 2); i++) {
        ShrunkTable[i] = new Node();
        ShrunkTable[i]->Emp = nullptr;
        ShrunkTable[i]->Status = AVAILABLE;
    }

    // Rehashing the data into the new table:
    StatusType rehash = RehashTable(ShrunkTable, TotalSize, TotalSize / 2);
    if (rehash != SUCCESS) {
        for (int i = 0; i < (TotalSize / 2); i++) {
            delete ShrunkTable[i];
        }
        delete[] ShrunkTable;
        return rehash;
    }

    return SUCCESS;
}

// RehashTable - Replaces the current table with a new one and re-inserts the employees to it:
StatusType HashTable::RehashTable(Node** DestTable, int OldSize, int NewSize) {
    // Moving the current table to a temporary pointer:
    Node** TempTable = Employees;

    // Setting the DestTable to be the main HashTable:
    Employees = DestTable;

    // Resetting the current size to not mess up the insert:
    CurrentSize = 0;

    // Adjusting the size of the table:
    TotalSize = NewSize;

    // Rehashing the employees from the previous table to the new one:
    for (int i = 0; i < OldSize; i++) {
        // Checking if the current index holds an employee:
        if (TempTable[i]->Emp == nullptr) continue;

        // Inserting the employee to the new table:
        StatusType add = Insert(TempTable[i]->Emp);
        if (add != SUCCESS) {
            return add;
        }
    }

    // Deleting the old table:
    for (int i = 0; i < OldSize; i++) {
        delete TempTable[i];
    }

    delete[] TempTable;

    return SUCCESS;
}

// Constructor:
HashTable::HashTable() {
    // Creating a new table:
    Employees = new Node * [CAPACITY];
    if (Employees == nullptr) {
        throw ALLOCATION_ERROR;
    }

    // Setting all the slots to be available:
    for (int i = 0; i < CAPACITY; i++) {
        Employees[i] = new Node();
        Employees[i]->Emp = nullptr;
        Employees[i]->Status = AVAILABLE;
    }

    // Setting the size and the current count of employees in the table:
    TotalSize = CAPACITY;
    CurrentSize = 0;
}

// Destructor:
HashTable::~HashTable() {
    // Deleting all the allocated nodes:
    for (int i = 0; i < TotalSize; i++) {
        delete Employees[i];
    }

    delete[] Employees;
}

// Insert - Inserts an employee to the table:
StatusType HashTable::Insert(Employee* inEmp) {
    // Making sure there's enough space in the table:
    if (TotalSize == CurrentSize) {
        StatusType expand = ExpandTable();
        if (expand != SUCCESS) {
            return expand;
        }
    }

    // Getting the index using the HashFunctions and proceeding to the next available spot:
    int index = HashFunctionFirst(inEmp->getID(), TotalSize);
    int offset = HashFunctionSecond(inEmp->getID(), TotalSize);

    while (Employees[index]->Status == NOTAVAILABLE) {
        index = (index + offset) % TotalSize;
    }

    // Adding the employee to the table and incrementing the count:
    Employees[index]->Emp = inEmp;
    Employees[index]->Status = NOTAVAILABLE;
    CurrentSize++;

    return SUCCESS;
}

// Remove - Deletes an employee from the table:
StatusType HashTable::Remove(int inID) {
    // Getting the index using the HashFunctions and proceeding to the correct spot:
    int index = HashFunctionFirst(inID, TotalSize);
    int offset = HashFunctionSecond(inID, TotalSize);

    while (true) {
        if (Employees[index]->Status == NOTAVAILABLE && Employees[index]->Emp->getID() == inID) break;

        index = (index + offset) % TotalSize;
    }

    // Removing the employee from the table and decrementing the count:
    Employees[index]->Emp = nullptr;
    Employees[index]->Status = DELETED;
    CurrentSize--;

    // Checking if there's a need to shrink the table:
    if (CurrentSize == (TotalSize / 4) && TotalSize > CAPACITY) {
        StatusType shrink = ShrinkTable();
        if (shrink != SUCCESS) {
            return shrink;
        }
    }

    return SUCCESS;
}

// Find - Returns a pointer to the employee associated with the given ID:
Employee* HashTable::Find(int inID) {
    // Getting the index using the HashFunctions and proceeding to the correct spot:
    int index = HashFunctionFirst(inID, TotalSize);
    int offset = HashFunctionSecond(inID, TotalSize);
    int initial = index;
    bool first = true;

    while (true) {
        if (Employees[index]->Status == AVAILABLE)
            break;
        else if (Employees[index]->Status == NOTAVAILABLE && Employees[index]->Emp->getID() == inID)
            return Employees[index]->Emp;
        else if (index == initial && (first == false))
            return nullptr;
        else
            index = (index + offset) % TotalSize;

        first = false;
    }

    return nullptr;
}

// GetSize - Returns the current filled size of the table:
int HashTable::GetSize() const { return CurrentSize; }

// FreeAllEmployees - Executes the delete function on every employee in the table:
void HashTable::FreeAllEmployees() {
    for (int i = 0; i < TotalSize; i++) {
        if (Employees[i]->Emp == nullptr) continue;

        delete Employees[i]->Emp;
    }
}

// GetEmployees - Fills a given array with pointers to all of the employees:
void HashTable::GetEmployees(Employee** toFill) {
    // Going over all the employees and filling the array:
    int counter = 0;
    for (int i = 0; i < TotalSize; i++) {
        if (Employees[i]->Emp == nullptr) continue;

        toFill[counter] = Employees[i]->Emp;
        counter++;
    }
}

// ReplaceArray - Replaces the current array with the data from a given array:
void HashTable::ReplaceArray(Employee** toReplace, int inSize) {
    // Getting rid of the current array:
    for (int i = 0; i < TotalSize; i++) {
        delete Employees[i];
    }

    delete[] Employees;

    // Creating a new table:
    Employees = new Node * [CAPACITY];
    if (Employees == nullptr) {
        throw ALLOCATION_ERROR;
    }

    // Setting all the slots to be available:
    for (int i = 0; i < CAPACITY; i++) {
        Employees[i] = new Node();
        Employees[i]->Emp = nullptr;
        Employees[i]->Status = AVAILABLE;
    }

    // Setting the size and the current count of employees in the table:
    TotalSize = CAPACITY;
    CurrentSize = 0;

    // Inserting the employees from the given array:
    for (int i = 0; i < inSize; i++) {
        Insert(toReplace[i]);
    }
}

// ===== Testing =====
void HashTable::Printshit()
{
    for (int i = 0; i < TotalSize; i++) {
        switch (Employees[i]->Status) {
        case AVAILABLE: std::cout << "Available" << std::endl;
            break;
        case NOTAVAILABLE: std::cout << "ID = " << Employees[i]->Emp->getID() << " Sal = " << Employees[i]->Emp->getSal() << " Grade = " << Employees[i]->Emp->getGrade() << " Employer = " << Employees[i]->Emp->getEmployer() << std::endl;
            break;
        case DELETED: std::cout << "Deleted" << std::endl;
            break;
        }
    }
}
