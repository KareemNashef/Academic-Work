#include "CompanyManager.h"
#include "library2.h"

#define Manager ((CompanyManager*)DS)

void* Init(int k) {
    if (k <= 1) return NULL;

    CompanyManager* DS = new CompanyManager(k);
    return (void*)DS;
}

StatusType AddEmployee(void* DS, int employeeID, int companyID, int grade) {
    if (DS == nullptr || employeeID <= 0 || companyID <= 0 || companyID > Manager->GetSize() || grade < 0) return INVALID_INPUT;

    return Manager->AddEmployee(employeeID, companyID, grade);
}

StatusType RemoveEmployee(void* DS, int employeeID) {
    if (DS == nullptr || employeeID <= 0) return INVALID_INPUT;

    return Manager->RemoveEmployee(employeeID);
}

StatusType AcquireCompany(void* DS, int companyID1, int companyID2, double factor) {
    if (DS == nullptr || companyID1 <= 0 || companyID1 > Manager->GetSize() || companyID2 <= 0 || companyID2 > Manager->GetSize() || companyID1 == companyID2 ||
        factor <= 0)
        return INVALID_INPUT;

    return Manager->AcquireCompany(companyID1, companyID2, factor);
}

StatusType EmployeeSalaryIncrease(void* DS, int employeeID, int salaryIncrease) {
    if (DS == nullptr || employeeID <= 0 || salaryIncrease <= 0) return INVALID_INPUT;

    return Manager->EmployeeSalaryIncrease(employeeID, salaryIncrease);
}

StatusType PromoteEmployee(void* DS, int employeeID, int bumpGrade) {
    if (DS == nullptr || employeeID <= 0) return INVALID_INPUT;

    return Manager->PromoteEmployee(employeeID, bumpGrade);
}

StatusType SumOfBumpGradeBetweenTopWorkersByGroup(void* DS, int companyID, int m) {
    if (DS == nullptr || companyID < 0 || companyID > Manager->GetSize() || m <= 0) return INVALID_INPUT;

    return Manager->SumOfBumpGradeBetweenTopWorkersByGroup(companyID, m);
}

StatusType AverageBumpGradeBetweenSalaryByGroup(void* DS, int companyID, int lowerSalary, int higherSalary) {
    if (DS == nullptr || companyID < 0 || companyID > Manager->GetSize() || lowerSalary > higherSalary || lowerSalary < 0 || higherSalary < 0)
        return INVALID_INPUT;

    return Manager->AverageBumpGradeBetweenSalaryByGroup(companyID, lowerSalary, higherSalary);
}

StatusType CompanyValue(void* DS, int companyID) {
    if (DS == nullptr || companyID <= 0 || companyID > Manager->GetSize()) return INVALID_INPUT;

    return Manager->CompanyValue(companyID);
}

void Quit(void** DS) {

    delete ((CompanyManager*)*DS);

    *DS = NULL;
}
