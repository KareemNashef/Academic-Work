#ifndef COMPANY_MANAGER_H
#define COMPANY_MANAGER_H

// ===== Included headers and used namespaces =====

#include "UnionFind.h"
#include <cmath>

#define FixedComp Companies->GetComp(Companies->Owner(companyID))

// ================================================

class CompanyManager {
    UnionFind* Companies;  // List of all companies
    HashTable* AllEmps;    // List of all employees
    AVL* SalEmps;       // List of employees who have a non-zero salary

    int Size;              // Total number of employees
    long long totalInternGrade;  // Sum of the grades of employees with no salary

public:
    // Constructor:
    CompanyManager(int inSize);

    // Destructor:
    ~CompanyManager();

    // ===== Needed functions =====
    StatusType AddEmployee(int employeeID, int companyID, int grade);
    StatusType RemoveEmployee(int employeeID);
    StatusType AcquireCompany(int companyID1, int companyID2, double factor);
    StatusType EmployeeSalaryIncrease(int employeeID, int salaryIncrease);
    StatusType PromoteEmployee(int employeeID, int bumpGrade);
    StatusType SumOfBumpGradeBetweenTopWorkersByGroup(int companyID, int m);
    StatusType AverageBumpGradeBetweenSalaryByGroup(int companyID, int lowerSalary, int higherSalary);
    StatusType CompanyValue(int companyID);

    // ===== Helpful functions =====
    int GetSize();

    // ===== Testing =====
    void Printshit();
};

#endif
