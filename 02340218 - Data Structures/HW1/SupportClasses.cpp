#include "SupportClasses.h"

// ===== Employee definitions =====

Employee::Employee(int in_id, int in_salary, int in_grade, int in_comp, Company* in_hiring)
    : empID(in_id), empSal(in_salary), empGrade(in_grade), compID(in_comp), hiringComp(in_hiring) {}

// ===== Get functions =====

int Employee::getID() const { return empID; }

int Employee::getSal() const { return empSal; }

int Employee::getGrade() const { return empGrade; }

int Employee::getCompID() const { return compID; }

Company* Employee::getHire() const { return hiringComp; }

// ===== Set functions =====

void Employee::setID(int input) { empID = input; }

void Employee::setSal(int input) { empSal = input; }

void Employee::setGrade(int input) { empGrade = input; }

void Employee::setCompID(int input) { compID = input; }

void Employee::setHire(Company* input) { hiringComp = input; }

// ===== Company definitions =====

Company::Company(int CompanyID, int Value) {
    companyID = CompanyID;
    companyVal = Value;
    size = 0;
    highestID = new int(0);
    highestSal = 0;
    employeesID = new AVL<Employee>();
    employeesSal = new AVL<Employee>();
}

Company::~Company() {
    delete highestID;
    delete employeesID;
    delete employeesSal;
}

// ===== Needed functions =====

StatusType Company::AddEmployee(Employee* newEmp) {
    // Adding the employee sorted on ID and Salary & checking for errors:
    StatusType Add = employeesID->AddNode(newEmp->getID(), newEmp->getSal(), SORT_ID, newEmp);
    if (Add != SUCCESS) return Add;

    Add = employeesSal->AddNode(newEmp->getSal(), newEmp->getID(), SORT_SAL, newEmp);
    if (Add != SUCCESS) {
        employeesID->DeleteNode(newEmp->getID(), newEmp->getSal(), SORT_ID);
        return Add;
    }

    // Checking & adjusting if this employee is the new highest paid:
    setHighest(newEmp->getID(), newEmp->getSal(), false);

    // Incrementing the company size:
    size++;

    return SUCCESS;
}

StatusType Company::RemoveEmployee(int in_id) {
    // Getting the employee info:
    Employee** to_remove = new Employee*();
    if (to_remove == nullptr) return ALLOCATION_ERROR;
    if (employeesID->getData(in_id, to_remove) == NOTFOUND) {
        delete to_remove;
        return FAILURE;
    }

    int temp_salary = (*to_remove)->getSal();

    // Removing the employee from the company lists & decrementing the size:

    StatusType Remove = employeesID->DeleteNode(in_id, temp_salary, SORT_ID);
    if (Remove != SUCCESS) {
        delete to_remove;
        return Remove;
    }

    Remove = employeesSal->DeleteNode(temp_salary, in_id, SORT_SAL);
    if (Remove != SUCCESS) {
        delete to_remove;
        return Remove;
    }

    // Decrementing the Company size:
    size--;

    // Checking if the tree is now empty
    if (size == 0) {
        *highestID = 0;
        highestSal = 0;
    }

    // Checking & adjusting if this employee was the highest paid:
    if (*highestID == in_id) {
        Employee* last = employeesSal->getLast();
        *highestID = last->getID();
        highestSal = last->getSal();
    }

    delete to_remove;

    return SUCCESS;
}

// ===== Set functions =====

void Company::setID(int input) { companyID = input; }

void Company::setVal(int input) { companyVal = input; }

void Company::setSize(int input) { size = input; }

void Company::setHighest(int id, int sal, bool force) {
    if (force == true) {
        *highestID = id;
        highestSal = sal;
    } else if (sal > highestSal || *highestID == 0) {
        *highestID = id;
        highestSal = sal;
    } else if (sal == highestSal) {
        if (id < *highestID) {
            *highestID = id;
            highestSal = sal;
        }
    }
}

// ===== Get functions =====

int Company::getID() const { return companyID; }

int Company::getVal() const { return companyVal; }

int Company::getSize() const { return size; }

int* Company::getHighestID() const { return highestID; }

int Company::getHighestSal() const { return highestSal; }

// ===== Misc functions =====

StatusType Company::replaceEmployees(Employee** data_ID, Employee** data_Sal, int totalsize) {
    // Creating new arrays to hold the IDs and Salaries
    int* primary = new int[totalsize];
    if (primary == nullptr) return ALLOCATION_ERROR;

    int* secondary = new int[totalsize];
    if (secondary == nullptr) {
        delete[] primary;
        return ALLOCATION_ERROR;
    }

    for (int i = 0; i < totalsize; i++) {
        primary[i] = (data_ID)[i]->getID();
        secondary[i] = data_ID[i]->getSal();
    }

    // Replacing the IDs tree with the new one:
    StatusType replace = employeesID->replace(primary, secondary, data_ID, totalsize);
    if (replace != SUCCESS) {
        delete[] primary;
        delete[] secondary;
        return replace;
    }

    for (int i = 0; i < totalsize; i++) {
        primary[i] = data_Sal[i]->getSal();
        secondary[i] = data_Sal[i]->getID();
    }

    // Replacing the Salaries tree with the new one:
    replace = employeesSal->replace(primary, secondary, data_Sal, totalsize);
    if (replace != SUCCESS) {
        delete[] primary;
        delete[] secondary;
        return replace;
    }

    // Adjusting the hirer ID and the pointer to the company in all employees:
    Employee** data = new Employee*[totalsize];
    if (data == nullptr) {
        delete[] primary;
        delete[] secondary;
        return ALLOCATION_ERROR;
    }

    employeesID->inOrder(data);

    for (int i = 0; i < totalsize; i++) {
        data[i]->setCompID(companyID);
        data[i]->setHire(this);
    }

    // Freeing the data:
    delete[] primary;
    delete[] secondary;
    delete[] data;

    return SUCCESS;
}

AVL<Employee>* Company::getAllEmpID() { return employeesID; }

AVL<Employee>* Company::getAllEmpSal() { return employeesSal; }