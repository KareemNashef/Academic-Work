#ifndef EMPLOYEE_MANAGER_H
#define EMPLOYEE_MANAGER_H

// ===== Included headers and used namespaces =====
#include "SupportClasses.h"
// ================================================

void mergeArraysID(Employee** Source1, Employee** Source2, int max1, int max2, Employee** Destination);
void mergeArraysSal(Employee** Source1, Employee** Source2, int max1, int max2, Employee** Destination);

class EmployeeManager {
    int highestID;                   // ID of the current highest paid employee
    int highestSal;                  // Salary of the current highest paid employee
    AVL<Company>* Companies;         // AVL tree of the companies
    AVL<Employee>* AllEmployeesID;   // AVL tree of the employees - sorted by id
    AVL<Employee>* AllEmployeesSal;  // AVL tree of the employees - sorted by id
    AVL<int>* HighestEmployees;      // AVL tree of the highest paid employees from every company -
                                     // sorted by companyID

   public:
    // Constructor:
    EmployeeManager();

    // Destructor:
    ~EmployeeManager();

    // ===== Needed functions =====
    StatusType AddCompany(int CompanyID, int Value);
    StatusType AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade);
    StatusType RemoveCompany(int CompanyID);
    StatusType RemoveEmployee(int EmployeeID);
    StatusType GetCompanyInfo(int CompanyID, int* Value, int* NumEmployees);
    StatusType GetEmployeeInfo(int EmployeeID, int* EmployerID, int* Salary, int* Grade);
    StatusType IncreaseCompanyValue(int CompanyID, int ValueIncrease);
    StatusType PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade);
    StatusType HireEmployee(int EmployeeID, int NewCompanyID);
    StatusType AcquireCompany(int AcquirerID, int TargetID, double Factor);
    StatusType GetHighestEarner(int CompanyID, int* EmployeeID);
    StatusType GetAllEmployeesBySalary(int CompanyID, int** Employees, int* NumOfEmployees);
    StatusType GetHighestEarnerInEachCompany(int NumOfCompanies, int** Employees);
    StatusType GetNumEmployeesMatching(int CompanyID, int MinEmployeeID, int MaxEmployeeId, int MinSalary, int MinGrade,
                                       int* TotalNumOfEmployees, int* NumOfEmployees);

    // ===== Helpful functions =====
    void setHighest(int id, int sal);
    StatusType addHighest(int CompID, int* EmpID);

    void yoyo();
};

#endif