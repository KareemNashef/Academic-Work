#include "Company.h"

// ===== Company definitions =====

Company::Company(int CompanyID, int Value) {
    companyID = CompanyID;
    companyVal = Value;
    size = 0;
    totalInternGrade = 0;
    AllEmps = new HashTable();
    SalEmps = new AVL();
}

Company::~Company() {
    delete AllEmps;
    delete SalEmps;
}

// ===== Needed functions =====

StatusType Company::AddEmployee(Employee* newEmp) {
    // Adding the employee to the HashTable:
    StatusType add = AllEmps->Insert(newEmp);
    if (add != SUCCESS) {
        return add;
    }

    // No need to add the employee to the AVL tree as they do not have a salary at the moment

    // Incrementing the company size & updating the total grades:
    totalInternGrade += newEmp->getGrade();
    size++;

    return SUCCESS;
}

StatusType Company::IncreaseEmployee(Employee* inEmp) {
    // Removing the employee's grade from the intern sum & adding them to the AVL:
    totalInternGrade -= inEmp->getGrade();

    return SalEmps->AddNode(inEmp);
}

StatusType Company::RemoveEmployee(int inID) {
    // Getting the employee's data from the HashTable:
    int Salary = AllEmps->Find(inID)->getSal();
    int Grade = AllEmps->Find(inID)->getGrade();

    // Removing the employee from the HashTable:
    StatusType remove = AllEmps->Remove(inID);
    if (remove != SUCCESS) {
        return remove;
    }

    // Remove the employee from the AVL tree - if necessary:
    if (Salary != 0) {
        remove = SalEmps->DeleteNode(Salary, inID, Grade);
        if (remove == ALLOCATION_ERROR) {
            return remove;
        }

        size--;
        return SUCCESS;
    }

    // Decrementing the Company size & updating the total grades:
    totalInternGrade -= Grade;
    size--;

    return SUCCESS;
}

// ===== Set functions =====

void Company::setVal(int input) { companyVal = input; }

void Company::setSize(int input) { size = input; }

void Company::setTotal(long long input) { totalInternGrade = input; }

void Company::incTot(int bump) { totalInternGrade += bump; }

// ===== Get functions =====

int Company::getID() const { return companyID; }

int Company::getVal() const { return companyVal; }

int Company::getSize() const { return size; }

long long Company::getTotal() const { return totalInternGrade; }

HashTable* Company::getAllEmps() { return AllEmps; }

AVL* Company::getSalEmps() { return SalEmps; }
