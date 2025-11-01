#include <iostream>

#include "EmployeeManager.h"

// ===== Employee Manager definitions =====

EmployeeManager::EmployeeManager() {
    highestID = 0;
    highestSal = 0;
    Companies = new AVL<Company>();
    AllEmployeesID = new AVL<Employee>();
    AllEmployeesSal = new AVL<Employee>();
    HighestEmployees = new AVL<int>();
}

EmployeeManager::~EmployeeManager() {
    // Deleting all companies then the tree itself:
    Company** dataC = new Company * [Companies->getSize()];
    Companies->inOrder(dataC);

    for (int i = 0; i < Companies->getSize(); i++) {
        delete dataC[i];
    }

    delete[] dataC;
    delete Companies;

    // Deleting all employees then both trees:
    Employee** dataE = new Employee * [AllEmployeesID->getSize()];
    AllEmployeesID->inOrder(dataE);

    for (int i = 0; i < AllEmployeesID->getSize(); i++) {
        delete dataE[i];
    }

    delete[] dataE;
    delete AllEmployeesID;
    delete AllEmployeesSal;

    // Deleting all highest employees - the data was already in companies and was deleted:
    delete HighestEmployees;
}

StatusType EmployeeManager::AddCompany(int CompanyID, int Value) {
    // =====================
    // ===== O(log(k)) =====
    // =====================

    // Initializing the new company:
    Company* newComp = new Company(CompanyID, Value);
    if (newComp == nullptr) return ALLOCATION_ERROR;

    // Adding the company to the list:
    StatusType add = Companies->AddNode(CompanyID, Value, SORT_ID, newComp);
    if (add != SUCCESS) {
        delete newComp;
        return add;
    }

    return SUCCESS;
}

StatusType EmployeeManager::AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade) {
    // ==============================
    // ===== O(log(n) + log(k)) =====
    // ==============================

    // Checking if the employee already exists in a company:
    Employee** search = new Employee * ();
    if (search == nullptr) {
        return ALLOCATION_ERROR;
    }
    if (AllEmployeesID->getData(EmployeeID, search) == FOUND) {
        delete search;
        return FAILURE;
    }

    // Checking if the company exists:
    Company** current = new Company * ();
    if (current == nullptr) {
        delete search;
        return ALLOCATION_ERROR;
    }
    if (Companies->getData(CompanyID, current) == NOTFOUND) {
        delete search;
        delete current;
        return FAILURE;
    }

    // Initializing a new employee:
    Employee* newEmp = new Employee(EmployeeID, Salary, Grade, CompanyID, *current);
    if (newEmp == nullptr) {
        delete search;
        delete current;
        return ALLOCATION_ERROR;
    }

    // Adding the employee to the full list:
    StatusType Add = AllEmployeesID->AddNode(EmployeeID, Salary, SORT_ID, newEmp);
    if (Add != SUCCESS) {
        delete search;
        delete current;
        delete newEmp;
        return Add;
    }

    Add = AllEmployeesSal->AddNode(Salary, EmployeeID, SORT_SAL, newEmp);
    if (Add != SUCCESS) {
        delete search;
        delete current;
        delete newEmp;
        AllEmployeesID->DeleteNode(EmployeeID, Salary, SORT_ID);
        return Add;
    }

    // Checking and setting if this employee is the new highest:
    setHighest(EmployeeID, Salary);
    Add = addHighest(CompanyID, ((*current)->getHighestID()));
    if (Add != SUCCESS) {
        delete search;
        delete current;
        delete newEmp;
        AllEmployeesID->DeleteNode(EmployeeID, Salary, SORT_ID);
        AllEmployeesSal->DeleteNode(Salary, EmployeeID, SORT_SAL);
        return Add;
    }

    // Adding the employee to the company:
    Add = (*current)->AddEmployee(newEmp);
    if (Add != SUCCESS) {
        delete search;
        delete current;
        delete newEmp;
        AllEmployeesID->DeleteNode(EmployeeID, Salary, SORT_ID);
        AllEmployeesSal->DeleteNode(Salary, EmployeeID, SORT_SAL);
        HighestEmployees->DeleteNode(CompanyID, 1, SORT_ID);
        return Add;
    }

    delete search;
    delete current;

    return SUCCESS;
}

StatusType EmployeeManager::RemoveCompany(int CompanyID) {
    // =====================
    // ===== O(log(k)) =====
    // =====================

    // Checking if the company exists:
    Company** to_remove = new Company * ();
    if (Companies->getData(CompanyID, to_remove) == NOTFOUND) {
        delete to_remove;
        return FAILURE;
    }

    // Making sure the Company is empty:
    if ((*to_remove)->getSize() != 0) {
        delete to_remove;
        return FAILURE;
    }

    // Removing the company from the list:
    Companies->DeleteNode(CompanyID, (*to_remove)->getVal(), SORT_ID);

    // Removing the company from the HighestEmployees list:
    HighestEmployees->DeleteNode(CompanyID, 0, false);

    delete (*to_remove);

    delete to_remove;

    return SUCCESS;
}

StatusType EmployeeManager::RemoveEmployee(int EmployeeID) {
    // =====================
    // ===== O(log(n)) =====
    // =====================

    // Checking if the employee exists:
    Employee** currentEM = new Employee * ();
    if (currentEM == nullptr) return ALLOCATION_ERROR;

    if (AllEmployeesID->getData(EmployeeID, currentEM) == NOTFOUND) {
        delete currentEM;
        return FAILURE;
    }

    // Removing the employee from his current company:
    Company* currentCO = (*currentEM)->getHire();
    currentCO->RemoveEmployee(EmployeeID);

    // Removing the employee from the overall list:
    AllEmployeesID->DeleteNode(EmployeeID, (*currentEM)->getSal(), SORT_ID);

    AllEmployeesSal->DeleteNode((*currentEM)->getSal(), EmployeeID, SORT_SAL);

    // Freeing the data:
    delete (*currentEM);

    // Checking if the employee manager is now empty:
    if (AllEmployeesID->getSize() == 0) {
        highestID = 0;
        highestSal = 0;
    }

    // Adjusting the highest employees:
    if (highestID == EmployeeID) {
        Employee* last = AllEmployeesSal->getLast();
        highestID = last->getID();
        highestSal = last->getSal();
    }
    if (currentCO->getSize() == 0) {
        HighestEmployees->DeleteNode(currentCO->getID(), 1, SORT_ID);
    }

    delete currentEM;

    return SUCCESS;
}

StatusType EmployeeManager::GetCompanyInfo(int CompanyID, int* Value, int* NumEmployees) {
    // =====================
    // ===== O(log(k)) =====
    // =====================

    // Getting the company data:
    Company** current = new Company * ();
    if (current == nullptr) return ALLOCATION_ERROR;

    if (Companies->getData(CompanyID, current) == NOTFOUND) {
        delete current;
        return FAILURE;
    }

    // Getting the info:
    *Value = (*current)->getVal();
    *NumEmployees = (*current)->getSize();

    delete current;

    return SUCCESS;
}

StatusType EmployeeManager::GetEmployeeInfo(int EmployeeID, int* EmployerID, int* Salary, int* Grade) {
    // =====================
    // ===== O(log(n)) =====
    // =====================

    // Getting the employee data:
    Employee** current = new Employee * ();
    if (AllEmployeesID->getData(EmployeeID, current) == NOTFOUND) {
        delete current;
        return FAILURE;
    }

    // Getting the info:
    *EmployerID = (*current)->getCompID();
    *Salary = (*current)->getSal();
    *Grade = (*current)->getGrade();

    delete current;

    return SUCCESS;
}

StatusType EmployeeManager::IncreaseCompanyValue(int CompanyID, int ValueIncrease) {
    // =====================
    // ===== O(log(k)) =====
    // =====================

    // Getting the company data:
    Company** current = new Company * ();

    if (Companies->getData(CompanyID, current) == NOTFOUND) {
        delete current;
        return FAILURE;
    }

    // Increasing the value:
    (*current)->setVal((*current)->getVal() + ValueIncrease);

    delete current;

    return SUCCESS;
}

StatusType EmployeeManager::PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade) {
    // =====================
    // ===== O(log(n)) =====
    // =====================

    // Checking if the employee exists:
    Employee** current = new Employee * ();
    if (AllEmployeesID->getData(EmployeeID, current) == NOTFOUND) {
        delete current;
        return FAILURE;
    }

    // Getting all of the employee info:
    int Sal = (*current)->getSal() + SalaryIncrease;
    int Grade = (*current)->getGrade();
    if (BumpGrade > 0) Grade++;
    int Employer = (*current)->getCompID();
    Company* comp = (*current)->getHire();

    // Removing the Employee then adding them again with the new data
    StatusType Remove = RemoveEmployee(EmployeeID);
    if (Remove != SUCCESS) return Remove;

    // Initializing a new employee:
    Employee* newEmp = new Employee(EmployeeID, Sal, Grade, Employer, comp);
    if (newEmp == nullptr) return ALLOCATION_ERROR;

    // Adding the employee to the full list:
    StatusType Add = AllEmployeesID->AddNode(EmployeeID, Sal, SORT_ID, newEmp);
    if (Add != SUCCESS) return Add;

    Add = AllEmployeesSal->AddNode(Sal, EmployeeID, SORT_SAL, newEmp);
    if (Add != SUCCESS) {
        AllEmployeesID->DeleteNode(EmployeeID, Sal, SORT_ID);
        return Add;
    }

    // Checking and setting if this employee is the new highest:
    setHighest(EmployeeID, Sal);

    // Adding the employee to the company:
    Add = comp->AddEmployee(newEmp);
    if (Add != SUCCESS) {
        AllEmployeesID->DeleteNode(EmployeeID, Sal, SORT_ID);
        AllEmployeesSal->DeleteNode(Sal, EmployeeID, SORT_SAL);
        HighestEmployees->DeleteNode(Employer, 1, SORT_ID);
        return Add;
    }
    addHighest(comp->getID(), comp->getHighestID()); // ===== NEW APPEAL LINES =====
    delete current;

    return SUCCESS;
}

StatusType EmployeeManager::HireEmployee(int EmployeeID, int NewCompanyID) {
    // ==============================
    // ===== O(log(n) + log(k)) =====
    // ==============================

    // Checking if the employee exists:
    Employee** current = new Employee * ();
    if (AllEmployeesID->getData(EmployeeID, current) == NOTFOUND) {
        delete current;
        return FAILURE;
    }

    // Checking if the employee already works at the new company:
    if ((*current)->getCompID() == NewCompanyID) {
        delete current;
        return FAILURE;
    }

    // Checking if the destination company exists:
    Company** destination = new Company * ();
    if (Companies->getData(NewCompanyID, destination) == NOTFOUND) {
        delete destination;
        delete current;
        return FAILURE;
    }

    // Removing the employee and re-adding him:
    int Sal = (*current)->getSal();
    int Grade = (*current)->getGrade();

    delete current;
    delete destination;

    StatusType move = RemoveEmployee(EmployeeID);
    if (move != SUCCESS) return move;

    return AddEmployee(EmployeeID, NewCompanyID, Sal, Grade);
}

StatusType EmployeeManager::AcquireCompany(int AcquirerID, int TargetID, double Factor) {
    // ===============================
    // ===== O(log(k) + n1 + n2  =====
    // ===============================

    // Getting the data from both companies:
    Company** TargetCO = new Company * ();
    if (Companies->getData(TargetID, TargetCO) == NOTFOUND) {
        delete TargetCO;
        return FAILURE;
    }

    Company** AcquirerCO = new Company * ();
    if (Companies->getData(AcquirerID, AcquirerCO) == NOTFOUND) {
        delete TargetCO;
        delete AcquirerCO;
        return FAILURE;
    }

    // Cheking if the companies meet the value rules:
    if ((*AcquirerCO)->getVal() < 10 * (*TargetCO)->getVal()) {
        delete TargetCO;
        delete AcquirerCO;
        return FAILURE;
    }

    // Checking if the target company has any employees:
    if ((*TargetCO)->getSize() != 0) {
        // Getting all of the employees info, from the target company:
        AVL<Employee>* targetEmpID = (*TargetCO)->getAllEmpID();
        AVL<Employee>* targetEmpSal = (*TargetCO)->getAllEmpSal();

        Employee** data_S1_ID = new Employee * [(*TargetCO)->getSize()];
        Employee** data_S1_Sal = new Employee * [(*TargetCO)->getSize()];
        if (data_S1_ID == nullptr || data_S1_Sal == nullptr) {
            if (data_S1_ID == nullptr)
                delete[] data_S1_Sal;
            else
                delete[] data_S1_ID;
            delete TargetCO;
            delete AcquirerCO;
            return ALLOCATION_ERROR;
        }

        targetEmpID->inOrder(data_S1_ID);
        targetEmpSal->inOrder(data_S1_Sal);

        // Getting all of the employees info, from the acquirer company:
        AVL<Employee>* acqEmpID = (*AcquirerCO)->getAllEmpID();
        AVL<Employee>* acqEmpSal = (*AcquirerCO)->getAllEmpSal();

        Employee** data_S2_ID = new Employee * [(*AcquirerCO)->getSize()];
        Employee** data_S2_Sal = new Employee * [(*AcquirerCO)->getSize()];
        if (data_S2_ID == nullptr || data_S2_Sal == nullptr) {
            if (data_S2_ID == nullptr)
                delete[] data_S2_Sal;
            else
                delete[] data_S2_ID;
            delete[] data_S1_ID;
            delete[] data_S1_Sal;
            delete TargetCO;
            delete AcquirerCO;
            return ALLOCATION_ERROR;
        }

        acqEmpID->inOrder(data_S2_ID);
        acqEmpSal->inOrder(data_S2_Sal);

        // Making a sorted array from both datasets:
        int totalSize = (*AcquirerCO)->getSize() + (*TargetCO)->getSize();

        Employee** data_T_ID = new Employee * [totalSize];
        Employee** data_T_Sal = new Employee * [totalSize];
        if (data_T_ID == nullptr || data_T_Sal == nullptr) {
            if (data_T_ID == nullptr)
                delete[] data_T_Sal;
            else
                delete[] data_T_ID;
            delete[] data_S1_ID;
            delete[] data_S1_Sal;
            delete[] data_S2_ID;
            delete[] data_S2_Sal;
            delete TargetCO;
            delete AcquirerCO;
            return ALLOCATION_ERROR;
        }

        mergeArraysID(data_S1_ID, data_S2_ID, (*TargetCO)->getSize(), (*AcquirerCO)->getSize(), data_T_ID);
        mergeArraysSal(data_S1_Sal, data_S2_Sal, (*TargetCO)->getSize(), (*AcquirerCO)->getSize(), data_T_Sal);

        // Setting the new size:
        (*AcquirerCO)->setSize(totalSize);
        (*TargetCO)->setSize(0);

        // Replacing the Acquirer company employee lists:
        (*AcquirerCO)->replaceEmployees(data_T_ID, data_T_Sal, totalSize);

        // Adjusting the highest employee:
        Employee* last = acqEmpSal->getLast();
        (*AcquirerCO)->setHighest(last->getID(), last->getSal(), false);
        addHighest((*AcquirerCO)->getID(), (*AcquirerCO)->getHighestID()); // ===== NEW APPEAL LINES =====
        // Freeing the new:
        delete[] data_S1_ID;
        delete[] data_S1_Sal;
        delete[] data_S2_ID;
        delete[] data_S2_Sal;
        delete[] data_T_ID;
        delete[] data_T_Sal;
    }

    // Adjusting the value of the acquirer company:
    (*AcquirerCO)->setVal(((*TargetCO)->getVal() + (*AcquirerCO)->getVal()) * Factor);

    // Removing the Target company:
    RemoveCompany(TargetID);

    delete TargetCO;
    delete AcquirerCO;

    return SUCCESS;
}

StatusType EmployeeManager::GetHighestEarner(int CompanyID, int* EmployeeID) {
    // =============================
    // ===== O(1) || O(log(k)) =====
    // =============================

    // Checking if we want the highest earner in a specific company or in all companies:
    if (CompanyID > 0) {
        // Getting the company data:
        Company** current = new Company * ();
        if (Companies->getData(CompanyID, current) == NOTFOUND) {
            delete current;
            return FAILURE;
        }

        // Checking if there are any employees in the company:
        if ((*current)->getSize() == 0) {
            delete current;
            return FAILURE;
        }

        // Getting the highest earner:
        *EmployeeID = *((*current)->getHighestID());

        delete current;

        return SUCCESS;
    }
    else {
        // Checking if there are any employees in the list:
        if (AllEmployeesID->getSize() == 0) return FAILURE;

        // Getting the highest earner:
        *EmployeeID = highestID;

        return SUCCESS;
    }
}

StatusType EmployeeManager::GetAllEmployeesBySalary(int CompanyID, int** Employees, int* NumOfEmployees) {
    // ===========================================
    // ===== O(n) || O(log(k) + CompanySize) =====
    // ===========================================

    // Declaring the tree that will be filled with either a specific company or with all employees:
    AVL<Employee>* Emps;

    // Checking if we want the all employees in a specific company or in all companies:
    if (CompanyID > 0) {
        // Getting the company data:
        Company** current = new Company * ();
        if (Companies->getData(CompanyID, current) == NOTFOUND) {
            delete current;
            return FAILURE;
        }

        // Checking if there are any employees in the company:
        if ((*current)->getSize() == 0) {
            delete current;
            return FAILURE;
        }

        Emps = (*current)->getAllEmpSal();
        *NumOfEmployees = (*current)->getSize();

        delete current;

    }
    else {
        // Checking if there are any employees in the list:
        if (AllEmployeesID->getSize() == 0) return FAILURE;

        // Putting the number of employees in the poiner:
        *NumOfEmployees = AllEmployeesSal->getSize();

        Emps = AllEmployeesSal;
    }

    // Getting all of the employees info, sorted from low to high by salary:
    Employee** data = new Employee * [*NumOfEmployees];
    if (data == nullptr) return ALLOCATION_ERROR;

    Emps->inOrder(data);

    *Employees = (int*)malloc(sizeof(int) * (*NumOfEmployees));
    if (Employees == nullptr) {
        delete[] data;
        return ALLOCATION_ERROR;
    }

    // Adding the IDs to the array:
    for (int i = (*NumOfEmployees - 1); i >= 0; i--) {
        (*Employees)[*NumOfEmployees - i - 1] = data[i]->getID();
    }

    // Freeing the new:
    delete[] data;

    return SUCCESS;
}

StatusType EmployeeManager::GetHighestEarnerInEachCompany(int NumOfCompanies, int** Employees) {
    // ======================================
    // ===== O(log(k) + NumOfCompanies) =====
    // ======================================

    // Making sure that there's enough companies:
    if (HighestEmployees->getSize() < NumOfCompanies) return FAILURE;

    // Getting all of the companies info, sorted from low to high by ID:
    int** data = new int* [NumOfCompanies];
    if (data == nullptr) return ALLOCATION_ERROR;

    int count = HighestEmployees->inOrderBoundedNum(data, NumOfCompanies);
    if (count != NumOfCompanies) {
        delete[] data;
        return FAILURE;
    }

    *Employees = (int*)malloc(sizeof(int) * NumOfCompanies);
    if (Employees == nullptr) {
        delete[] data;
        return ALLOCATION_ERROR;
    }

    // Adding the IDs to the array:
    for (int i = 0; i < NumOfCompanies; i++) {
        (*Employees)[i] = *data[i];
    }

    // Freeing the new:
    delete[] data;

    return SUCCESS;
}

StatusType EmployeeManager::GetNumEmployeesMatching(int CompanyID, int MinEmployeeID, int MaxEmployeeId, int MinSalary,
    int MinGrade, int* TotalNumOfEmployees, int* NumOfEmployees) {
    // =======================================================================================
    // ===== O(log(n) + TotalNumOfEmployees) || O(log(k) + log(n) + TotalNumOfEmployees) =====
    // =======================================================================================
    AVL<Employee>* Emps;
    int fullSize;

    if (CompanyID > 0) {
        // Getting the company data:
        Company** currentCO = new Company * ();
        if (Companies->getData(CompanyID, currentCO) == NOTFOUND) {
            delete currentCO;
            return FAILURE;
        }
        // Checking if there are any employees in the company:
        if ((*currentCO)->getSize() == 0) {
            delete currentCO;
            return FAILURE;
        }

        Emps = (*currentCO)->getAllEmpID();
        fullSize = (*currentCO)->getSize();

        delete currentCO;

    }
    else {
        // Checking if there are any employees in the list:
        if (AllEmployeesID->getSize() == 0) return FAILURE;

        Emps = AllEmployeesID;
        fullSize = AllEmployeesID->getSize();
    }

    // Getting the closest node to the minimum & maximum id:
    Employee* start = Emps->getClosest(MinEmployeeID);   // One before
    Employee* finish = Emps->getClosest(MaxEmployeeId);  // One after

    // Converting to indexes:
    int start_index = Emps->getRank(start->getID()) - 1;
    if (start->getID() < MinEmployeeID) start_index++;
    int finish_index = Emps->getRank(finish->getID()) - 1;
    if (finish->getID() > MaxEmployeeId) finish_index--;

    // Making sure the indexes are in bound:
    if (start_index > fullSize) {
        *TotalNumOfEmployees = 0;
        *NumOfEmployees = 0;
        return SUCCESS;
    }

    // Adjusting the total number of employees:
    *TotalNumOfEmployees = finish_index - start_index + 1;

    if (*TotalNumOfEmployees < 1) {
        *NumOfEmployees = 0;
        return SUCCESS;
    }

    // Getting the range of employees who pass the ID check:
    Employee** data = new Employee * [*TotalNumOfEmployees];
    if (data == nullptr) return ALLOCATION_ERROR;

    start = Emps->getByIndex(start_index);
    finish = Emps->getByIndex(finish_index);

    StatusType range = Emps->getRange(data, start->getID(), finish->getID(), *TotalNumOfEmployees);
    if (range != SUCCESS) {
        delete[] data;
        return range;
    }

    // Getting the number of employees who pass the minimim salary/grade check:
    int num_size = *TotalNumOfEmployees;
    for (int i = 0; i < *TotalNumOfEmployees; i++) {
        if (data[i]->getSal() < MinSalary || data[i]->getGrade() < MinGrade) num_size--;
    }

    // Adjusting the number of employees:
    *NumOfEmployees = num_size;

    // Freeing the news:
    delete[] data;

    return SUCCESS;
}

void EmployeeManager::setHighest(int id, int sal) {
    // ================
    // ===== O(1) =====
    // ================

    if (sal > highestSal || highestID == 0) {
        highestID = id;
        highestSal = sal;
    }
    else if (sal == highestSal) {
        if (id < highestID) {
            highestID = id;
            highestSal = sal;
        }
    }
}

StatusType EmployeeManager::addHighest(int CompID, int* EmpID) {
    // ====================
    // ===== O(log(k) =====
    // ====================

    // Adding the company to the HighestEmployees tree
    // If it already exists then it wont be added
    if (HighestEmployees->AddNode(CompID, 1, SORT_ID, EmpID) == ALLOCATION_ERROR) return ALLOCATION_ERROR;

    return SUCCESS;
}

void mergeArraysID(Employee** Source1, Employee** Source2, int max1, int max2, Employee** Destination) {
    int indexS1 = 0, indexS2 = 0, indexD = 0;

    // Traverse both array
    while (indexS1 < max1 && indexS2 < max2) {
        if (Source1[indexS1]->getID() < Source2[indexS2]->getID())
            Destination[indexD++] = Source1[indexS1++];
        else
            Destination[indexD++] = Source2[indexS2++];
    }

    // Adding what's left of Source1
    while (indexS1 < max1) Destination[indexD++] = Source1[indexS1++];

    // Adding what's left of Source2
    while (indexS2 < max2) Destination[indexD++] = Source2[indexS2++];
}

void mergeArraysSal(Employee** Source1, Employee** Source2, int max1, int max2, Employee** Destination) {
    int indexS1 = 0, indexS2 = 0, indexD = 0;

    // Traverse both array
    while (indexS1 < max1 && indexS2 < max2) {
        if (Source1[indexS1]->getSal() < Source2[indexS2]->getSal())
            Destination[indexD++] = Source1[indexS1++];
        else
            Destination[indexD++] = Source2[indexS2++];
    }

    // Adding what's left of Source1
    while (indexS1 < max1) Destination[indexD++] = Source1[indexS1++];

    // Adding what's left of Source2
    while (indexS2 < max2) Destination[indexD++] = Source2[indexS2++];
}
