#include "CompanyManager.h"

// Constructor:
CompanyManager::CompanyManager(int inSize) {
    Companies = new UnionFind(inSize);
    AllEmps = new HashTable();
    SalEmps = new AVL();
    Size = inSize;
    totalInternGrade = 0;
}

// Destructor:
CompanyManager::~CompanyManager() {
    delete Companies;

    AllEmps->FreeAllEmployees();
    delete AllEmps;

    delete SalEmps;
}

// ===== Needed functions =====
StatusType CompanyManager::AddEmployee(int employeeID, int companyID, int grade) {
    // Checking if the employee already exists:
    if (AllEmps->Find(employeeID) != nullptr) {
        return FAILURE;
    }
    Company* AddedTo = FixedComp;

    // Allocating a new employee:
    Employee* NewEmp = new Employee(employeeID, 0, grade, AddedTo->getID(), AddedTo);

    // Sending a pointer of the employee to the list of all employees:
    StatusType add = AllEmps->Insert(NewEmp);
    if (add != SUCCESS) {
        return add;
    }

    // Sending a pointer of the employee to its company:
    add = FixedComp->AddEmployee(NewEmp);
    if (add != SUCCESS) {
        AllEmps->Remove(employeeID);
        return add;
    }

    // Adjusting the grade sum of intern employees:
    totalInternGrade += grade;

    // No need to add the employee to the AVL tree as they do not have a salary at the moment

    return SUCCESS;
}

StatusType CompanyManager::RemoveEmployee(int employeeID) {
    // Checking if the employee exists:
    Employee* toDelete = AllEmps->Find(employeeID);
    if (toDelete == nullptr) {
        return FAILURE;
    }
    
    Company* Hirer = (Company*)toDelete->getHirer();

    // If the employee has a non-zero salary then we need to delete them from the AVL tree:
    if (toDelete->getSal() != 0) {
        StatusType remove = SalEmps->DeleteNode(toDelete->getSal(), toDelete->getID(), toDelete->getGrade());
        if (remove != SUCCESS) {
            return remove;
        }
    }
    // If the employee has a zero salary then we need to adjust the grades total:
    else {
        totalInternGrade -= toDelete->getGrade();
    }

    // Removing the employee from the AllEmps HashTable:
    StatusType remove = AllEmps->Remove(employeeID);
    if (remove != SUCCESS) {
        return remove;
    }

    // Removing the employee from their Company:
    remove = Hirer->RemoveEmployee(employeeID);
    if (remove != SUCCESS) {
        return remove;
    }

    // Deleting the employee:
    delete toDelete;

    return SUCCESS;
}

StatusType CompanyManager::AcquireCompany(int companyID1, int companyID2, double factor) {
    
    // Making sure the companies aren't from the same group:
    int AcquirerID = Companies->Owner(companyID1);
    int TargetID = Companies->Owner(companyID2);

    if (AcquirerID == TargetID) {
        return INVALID_INPUT;
    }

    // Part 1 - Moving the employees from one company to the other:
    Company* Acquirer = Companies->GetComp(Companies->Owner(companyID1));
    Company* Target = Companies->GetComp(Companies->Owner(companyID2));

    // Getting the sizes:
    int Acq_AllSize = Acquirer->getAllEmps()->GetSize();
    int Tar_AllSize = Target->getAllEmps()->GetSize();
    int Acq_SalSize = Acquirer->getSalEmps()->getSize();
    int Tar_SalSize = Target->getSalEmps()->getSize();

    // Part 1.1 - Transferring the hashtable:
    
    // Checking for empty tables:
    if (Acq_AllSize != 0 && Tar_AllSize != 0) {
        // Allocating the arrays that we will use to save the employee data temporarily:
        Employee** Acq_AllEmps = new Employee * [Acq_AllSize];
        Employee** Tar_AllEmps = new Employee * [Tar_AllSize];

        // Getting all of the employees from the companies:
        Acquirer->getAllEmps()->GetEmployees(Acq_AllEmps);
        Target->getAllEmps()->GetEmployees(Tar_AllEmps);

        // Merging all of the employees into one table and adjusting the hirer ID:
        int AllSize = Acq_AllSize + Tar_AllSize;
        Employee** AllEmployees = new Employee * [AllSize];

        for (int i = 0; i < Acq_AllSize; i++) {
            AllEmployees[i] = Acq_AllEmps[i];
            AllEmployees[i]->setEmployer(Acquirer->getID(),Acquirer);
        }
        for (int i = Acq_AllSize; i < AllSize; i++) {
            AllEmployees[i] = Tar_AllEmps[i - Acq_AllSize];
            AllEmployees[i]->setEmployer(Acquirer->getID(), Acquirer);
        }

        Acquirer->getAllEmps()->ReplaceArray(AllEmployees, AllSize);
        Target->getAllEmps()->ReplaceArray(nullptr, 0);

        Acquirer->setTotal(Acquirer->getTotal() + Target->getTotal());
        Target->setTotal(0);

        // Freeing the data:
        delete[] Acq_AllEmps;
        delete[] Tar_AllEmps;
        delete[] AllEmployees;
    }
    else if (Acq_AllSize == 0 && Tar_AllSize != 0) {
        // Allocating the arrays that we will use to save the employee data temporarily:
        Employee** Tar_AllEmps = new Employee * [Tar_AllSize];

        // Getting all of the employees from the companies:
        Target->getAllEmps()->GetEmployees(Tar_AllEmps);

        for (int i = 0; i < Tar_AllSize; i++) {
            Tar_AllEmps[i]->setEmployer(Acquirer->getID(), Acquirer);
        }

        Acquirer->getAllEmps()->ReplaceArray(Tar_AllEmps, Tar_AllSize);

        Acquirer->setTotal(Target->getTotal());
        Target->setTotal(0);

        // Freeing the data:
        delete[] Tar_AllEmps;
    }

    // If the target is zero then there would be no change
    
    // Part 1.2 - Transferring the avl tree:

    // Checking for empty trees:
    if (Acq_SalSize != 0 && Tar_SalSize != 0)
    {
        // Allocating the arrays that we will use to save the employee data temporarily:
        Employee** Acq_SalEmps = new Employee * [Acq_SalSize];
        Employee** Tar_SalEmps = new Employee * [Tar_SalSize];

        // Getting all of the employees from the companies:
        Acquirer->getSalEmps()->inOrder(Acq_SalEmps);
        Target->getSalEmps()->inOrder(Tar_SalEmps);

        // Merging the two sorted Salaried employees arrays into one:
        int indexS1 = 0, indexS2 = 0, indexD = 0;
        int SalSize = Acq_SalSize + Tar_SalSize;
        Employee** AllSalariedEmployees = new Employee * [SalSize];

        // Traverse both array:
        while (indexS1 < Acq_SalSize && indexS2 < Tar_SalSize) {
            if (Acq_SalEmps[indexS1]->getSal() < Tar_SalEmps[indexS2]->getSal())
                AllSalariedEmployees[indexD++] = Acq_SalEmps[indexS1++];
            else if (Acq_SalEmps[indexS1]->getSal() > Tar_SalEmps[indexS2]->getSal())
                AllSalariedEmployees[indexD++] = Tar_SalEmps[indexS2++];
            else {
                if (Acq_SalEmps[indexS1]->getID() < Tar_SalEmps[indexS2]->getID())
                    AllSalariedEmployees[indexD++] = Acq_SalEmps[indexS1++];
                else
                    AllSalariedEmployees[indexD++] = Tar_SalEmps[indexS2++];
            }
        }

        // Adding what's left of the acquirer:
        while (indexS1 < Acq_SalSize) AllSalariedEmployees[indexD++] = Acq_SalEmps[indexS1++];

        // Adding what's left of the target:
        while (indexS2 < Tar_SalSize) AllSalariedEmployees[indexD++] = Tar_SalEmps[indexS2++];

        // Setting the new size:
        Acquirer->setSize(SalSize);
        Target->setSize(0);

        // Replacing the Acquirer tree with the new data:
        StatusType replace = Acquirer->getSalEmps()->replace(AllSalariedEmployees, SalSize);
        if (replace != SUCCESS) {
            return replace;
        }

        // Replacing the Target tree with an empty:
        replace = Target->getSalEmps()->replace(nullptr, 0);
        if (replace != SUCCESS) {
            return replace;
        }

        // Freeing the data:
        delete[] Acq_SalEmps;
        delete[] Tar_SalEmps;
        delete[] AllSalariedEmployees;
    }
    else if (Acq_SalSize == 0 && Tar_SalSize != 0) {
        // Allocating the arrays that we will use to save the employee data temporarily:
        Employee** Tar_SalEmps = new Employee * [Tar_SalSize];

        // Getting all of the employees from the companies:
        Target->getSalEmps()->inOrder(Tar_SalEmps);

        // Setting the new size and the new totalInternGrades:
        Acquirer->setSize(Tar_SalSize);
        Target->setSize(0);

        // Replacing the Acquirer tree with the new data:
        StatusType replace = Acquirer->getSalEmps()->replace(Tar_SalEmps, Tar_SalSize);
        if (replace != SUCCESS) {
            return replace;
        }

        // Replacing the Target tree with an empty:
        replace = Target->getSalEmps()->replace(nullptr, 0);
        if (replace != SUCCESS) {
            return replace;
        }

        // Freeing the data:
        delete[] Tar_SalEmps;
    }

    // If the target is zero then there would be no change

    // Part 2 - Adjusting the company groups and values:

    // Checking which company is the smaller one:
    int SmallerComp, BiggerComp;
    if (Companies->GetGroupSize(TargetID) <= Companies->GetGroupSize(AcquirerID)) {
        SmallerComp = Companies->Find(TargetID);
        BiggerComp = Companies->Find(AcquirerID);
    }
    else {
        SmallerComp = Companies->Find(AcquirerID);
        BiggerComp = Companies->Find(TargetID);
    }
    
    // Uniting the two groups:
    Companies->Union(SmallerComp, BiggerComp, Companies->Owner(AcquirerID), factor);

    return SUCCESS;
}

StatusType CompanyManager::EmployeeSalaryIncrease(int employeeID, int salaryIncrease) {
    // Checking if the employee exists:
    Employee* toIncrease = AllEmps->Find(employeeID);
    if (toIncrease == nullptr) {
        return FAILURE;
    }

    // Checking what the current salary is:
    if (toIncrease->getSal() == 0) {
        // Case - The salary is 0, we adjust the salary and add the employee to the AVL tree:
        toIncrease->setSal(salaryIncrease);
        StatusType Increase = SalEmps->AddNode(toIncrease);
        if (Increase != SUCCESS) {
            return Increase;
        }
        
        // Removing the employee's grade from the intern sum:
        totalInternGrade -= toIncrease->getGrade();

        // Adjusting the AVL inside their company as well:
        int companyID = toIncrease->getEmployer();
        return FixedComp->IncreaseEmployee(toIncrease);
    }

    // Case - The salary is not 0:
    
    // We remove the employee from both trees, the main and the company one:
    int companyID = toIncrease->getEmployer();

    StatusType remove = SalEmps->DeleteNode(toIncrease->getSal(), toIncrease->getID(), toIncrease->getGrade());
    if (remove != SUCCESS) {
        return remove;
    }

    remove = FixedComp->getSalEmps()->DeleteNode(toIncrease->getSal(), toIncrease->getID(), toIncrease->getGrade());
    if (remove != SUCCESS) {
        return remove;
    }

    // Adjusting the employee's salary:
    toIncrease->setSal(toIncrease->getSal() + salaryIncrease);

    // Re-adding the employee back to the trees:
    StatusType add = SalEmps->AddNode(toIncrease);
    if (add != SUCCESS) {
        return add;
    }

    add = FixedComp->getSalEmps()->AddNode(toIncrease);
    if (add != SUCCESS) {
        return add;
    }

    return SUCCESS;
}

StatusType CompanyManager::PromoteEmployee(int employeeID, int bumpGrade) {
    // Checking if the employee exists:
    Employee* toPromote = AllEmps->Find(employeeID);
    if (toPromote == nullptr) {
        return FAILURE;
    }

    // Checking if there's a need to bump the grade:
    if (bumpGrade < 1) {
        return SUCCESS;
    }

    // Checking what the current salary is:
    if (toPromote->getSal() == 0) {
        // Case - The salary is 0, we adjust the grade and increase the total:
        toPromote->bumpGrade(bumpGrade);

        totalInternGrade += bumpGrade;

        // Adjusting the total inside their company as well:
        int companyID = toPromote->getEmployer();
        FixedComp->incTot(bumpGrade);

        return SUCCESS;
    }

    // Case - The salary is not 0:

    // We remove the employee from both trees, the main and the company one:
    int companyID = toPromote->getEmployer();

    StatusType remove = SalEmps->DeleteNode(toPromote->getSal(), toPromote->getID(), toPromote->getGrade());
    if (remove != SUCCESS) {
        return remove;
    }

    remove = FixedComp->getSalEmps()->DeleteNode(toPromote->getSal(), toPromote->getID(), toPromote->getGrade());
    if (remove != SUCCESS) {
        return remove;
    }

    // Adjusting the employee's salary:
    toPromote->bumpGrade(bumpGrade);

    // Re-adding the employee back to the trees:
    StatusType add = SalEmps->AddNode(toPromote);
    if (add != SUCCESS) {
        return add;
    }

    add = FixedComp->getSalEmps()->AddNode(toPromote);
    if (add != SUCCESS) {
        return add;
    }

    return SUCCESS;
}

StatusType CompanyManager::SumOfBumpGradeBetweenTopWorkersByGroup(int companyID, int m) {
    // Getting the AVL tree that we're operating on:
    AVL* Employees;

    if (companyID == 0) {
        Employees = SalEmps;
    }
    else {
        Employees = FixedComp->getSalEmps();
    }

    // Checking if there's at least m employees in the tree:
    int TotalEmps = Employees->getSize();
    if (TotalEmps < m) {
        return FAILURE;
    }

    // Since we need the top m employees, the lowest rank would be total - m:
    int LowestEmp = TotalEmps - m;

    // Getting the employee with this rank:
    Employee* Lowest = Employees->getByIndex(LowestEmp);
    
    if (m == 1) {
        // Printing the sum:
        printf("SumOfBumpGradeBetweenTopWorkersByGroup: %d\n", Lowest->getGrade());

        return SUCCESS;
    }
    
    // Getting the sum of grades below this employee:
    long long LowerGrades = Employees->getLowerSum(Lowest->getSal(), Lowest->getID()) - Lowest->getGrade();

    // Printing the sum:
    printf("SumOfBumpGradeBetweenTopWorkersByGroup: %lld\n", (Employees->getTotal() - LowerGrades));

    return SUCCESS;
}

StatusType CompanyManager::AverageBumpGradeBetweenSalaryByGroup(int companyID, int lowerSalary, int higherSalary) {
    
    // Getting the tree and the hash that we're operating on:
    HashTable* Interns;
    AVL* Employees;
    long long InternGrades;

    if (companyID == 0) {
        Interns = AllEmps;
        Employees = SalEmps;
        InternGrades = totalInternGrade;
    }
    else {
        Interns = FixedComp->getAllEmps();
        Employees = FixedComp->getSalEmps();
        InternGrades = FixedComp->getTotal();
    }

    // Checking if there are any employees in the first place:
    if (Interns->GetSize() == 0) {
        return FAILURE;
    }

    // Checking which employees are included:
    if ((lowerSalary == 0 && higherSalary == 0) || (lowerSalary == 0 && Employees->getSize() == 0)) {
        // Case - Only interns:

        // Checking if there are employees that fit within the bounds:
        int NumOfEmps = Interns->GetSize() - Employees->getSize();
        if (NumOfEmps == 0) return FAILURE;

        double average = (double)InternGrades / NumOfEmps;
        
        // Printing the average:
        printf("AverageBumpGradeBetweenSalaryByGroup: %.1f\n", (double)average);
    }
    else if (lowerSalary == 0) {
        // Case - Interns and salaried employees:
        
        // Since the Max is not zero then we must search for the closest salary to it in the tree:
        Employee* Finish = Employees->getClosest(higherSalary, true);
        
        // Converting to indexes:
        int FinishIndex = Employees->getRank(Finish->getSal(), Finish->getID()) - 1;
        if (Finish->getSal() > higherSalary) FinishIndex--;

        int NumOfEmps;
        double Average;

        // Checking for a negative index, meaning that no nodes are lower than the highersalary:
        if (FinishIndex < 0) {
            // Checking if there are employees that fit within the bounds:
            NumOfEmps = Interns->GetSize() - Employees->getSize();
            if (NumOfEmps == 0) return FAILURE;

            Average = (double)InternGrades / NumOfEmps;
        }
        else {
            // Checking if there are employees that fit within the bounds:
            NumOfEmps = Interns->GetSize() - Employees->getSize() + (FinishIndex + 1);
            if (NumOfEmps == 0) return FAILURE;

            // Getting the total of grades between those employees:
            Finish = Employees->getByIndex(FinishIndex);

            // Getting the total of grades below this employee:
            long long LowerGrades = Employees->getLowerSum(Finish->getSal(), Finish->getID());

            Average = (double)(InternGrades + LowerGrades) / NumOfEmps;
        }

        // Printing the average:
        printf("AverageBumpGradeBetweenSalaryByGroup: %.1f\n", (double)Average);
    }
    else {
        // Case - Salaried employees only:
        
        // Since we're only dealing with salaried employees we need to get the min and max from the tree:
        Employee* Start = Employees->getClosest(lowerSalary, false);
        if (Start == nullptr)
            return FAILURE;

        Employee* Finish = Employees->getClosest(higherSalary, true);
        if (Finish == nullptr)
            return FAILURE;

        // Converting to indexes:
        int StartIndex = Employees->getRank(Start->getSal(), Start->getID()) - 1;
        if (Start->getSal() < lowerSalary) StartIndex++;
        int FinishIndex = Employees->getRank(Finish->getSal(), Finish->getID()) - 1;
        if (Finish->getSal() > higherSalary) FinishIndex--;

       
        // Getting the number of employees that fit within the bounds:
        int NumOfEmps = FinishIndex - StartIndex + 1;
        if (NumOfEmps < 1) return FAILURE;
        if (NumOfEmps == 1) {
            Employee* Emp = Employees->getByIndex(StartIndex);

            if (Emp->getSal() < lowerSalary || Emp->getSal() > higherSalary)
                return FAILURE;

            printf("AverageBumpGradeBetweenSalaryByGroup: %.1f\n", (double)Emp->getGrade());
            return SUCCESS;
        }

        // Getting the total of grades between those employees:
        Start = Employees->getByIndex(StartIndex);
        Finish = Employees->getByIndex(FinishIndex);
        
        long long MinGrades;
        if (Start->getSal() >= lowerSalary)
            MinGrades = Employees->getLowerSum(Start->getSal(), Start->getID()) - Start->getGrade();
        else
            MinGrades = Employees->getLowerSum(Start->getSal(), Start->getID());

        long long MaxGrades;
        if (Finish->getSal() <= higherSalary)
            MaxGrades = Employees->getLowerSum(Finish->getSal(), Finish->getID());
        else
            MaxGrades = Employees->getLowerSum(Finish->getSal(), Finish->getID()) - Finish->getGrade();
        
        double average = (double)(MaxGrades - MinGrades) / NumOfEmps;

        // Printing the average:
        printf("AverageBumpGradeBetweenSalaryByGroup: %.1f\n", (double)average);
    }

    return SUCCESS;
}

StatusType CompanyManager::CompanyValue(int companyID) {

    // Printing the value:
    printf("CompanyValue: %.1f\n", Companies->GetValue(companyID));

    return SUCCESS;
}

// ===== Helpful functions =====
int CompanyManager::GetSize() { return Size; }

// ===== Testing =====
void CompanyManager::Printshit()
{
    std::cout << "--------------------------" << std::endl;
    AllEmps->Printshit();
}
