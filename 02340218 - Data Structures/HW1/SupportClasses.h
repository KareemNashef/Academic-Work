#ifndef SUPPORT_CLASSES_H
#define SUPPORT_CLASSES_H

// ===== Included headers & defines =====

#include "AVL.h"

#define SORT_ID false
#define SORT_SAL true

class Company;

// ===== Employee class =====

class Employee {
   protected:
    int empID;            // Employee ID
    int empSal;           // Employee Salary
    int empGrade;         // Employee Grade
    int compID;           // Hiring company ID
    Company* hiringComp;  // Pointer to the hiring company

   public:
    // Constructor - Initializes a new employee:
    Employee(int in_id, int in_salary, int in_grade, int in_comp, Company* in_hiring);

    // Destructor - Destroys an employee:
    ~Employee() = default;

    // ===== Get functions =====
    int getID() const;
    int getSal() const;
    int getGrade() const;
    int getCompID() const;
    Company* getHire() const;

    // ===== Set functions =====
    void setID(int input);
    void setSal(int input);
    void setGrade(int input);
    void setCompID(int input);
    void setHire(Company* input);
};

// ===== Company class =====

class Company {
    int companyID;                // Company ID
    int companyVal;               // Company Value
    int size;                     // Total number of employees
    int* highestID;               // Pointer to the ID of the current highest paid employee
    int highestSal;               // Salary of the current highest paid employee
    AVL<Employee>* employeesID;   // List of employees sorted by ID
    AVL<Employee>* employeesSal;  // List of employees sorted by Salary

   public:
    // Constructor:
    Company(int CompanyID, int Value);

    // Destructor:
    ~Company();

    // ===== Needed functions =====
    StatusType AddEmployee(Employee* newEmp);
    StatusType RemoveEmployee(int in_id);

    // ===== Set functions =====
    void setID(int input);
    void setVal(int input);
    void setSize(int input);
    void setHighest(int id, int sal, bool force);

    // ===== Get functions =====
    int getID() const;
    int getVal() const;
    int getSize() const;
    int* getHighestID() const;
    int getHighestSal() const;

    // ===== Misc functions =====
    StatusType replaceEmployees(Employee** data_ID, Employee** data_Sal, int size);
    AVL<Employee>* getAllEmpID();
    AVL<Employee>* getAllEmpSal();
};

#endif