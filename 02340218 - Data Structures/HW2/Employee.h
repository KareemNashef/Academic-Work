#ifndef EMPLOYEE_H
#define EMPLOYEE_H

// ===== Included headers and used namespaces =====
#include "library2.h"
// ================================================

class Employee {
    int EmpID;
    int EmpSal;
    int EmpGrade;
    int Employer;
    void* EmployerPtr;

public:
    // Constructor:
    Employee(int inID, int inSal, int inGrade, int inEmployer, void* inComp);

    // Destructor:
    ~Employee() = default;

    // ===== Helpful functions =====
    int getID();
    int getSal();
    int getGrade();
    int getEmployer();
    void* getHirer();

    void setSal(int inSal);
    void setEmployer(int inID, void* inPtr);
    void bumpGrade(int bump);
};

#endif