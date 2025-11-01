#ifndef COMPANY_H
#define COMPANY_H

// ===== Included headers and used namespaces =====
#include "NewAVL.h"
#include "HashTable.h"
// ================================================

class Company {
    int companyID;         // Company ID
    int companyVal;        // Company Value
    int size;              // Total number of employees
    long long totalInternGrade;  // Sum of the grades of employees with no salary
    HashTable* AllEmps;    // List of all employees
    AVL* SalEmps;       // List of employees who have a non-zero salary

public:
    // Constructor:
    Company(int CompanyID, int Value);

    // Destructor:
    ~Company();

    // ===== Needed functions =====
    StatusType AddEmployee(Employee* newEmp);
    StatusType IncreaseEmployee(Employee* inEmp);
    StatusType RemoveEmployee(int inID);

    // ===== Helpful functions =====
    void setVal(int input);
    void setSize(int input);
    void setTotal(long long input);
    void incTot(int bump);

    int getID() const;
    int getVal() const;
    int getSize() const;
    long long getTotal() const;

    HashTable* getAllEmps();
    AVL* getSalEmps();
};

#endif