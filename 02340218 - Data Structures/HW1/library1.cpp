#include "EmployeeManager.h"
#include "library1.h"

#define Manager ((EmployeeManager*)DS)

void* Init() {
    EmployeeManager* DS = new EmployeeManager();
    return (void*)DS;
}

StatusType AddCompany(void* DS, int CompanyID, int Value) {
    if (DS == nullptr || CompanyID < 1 || Value < 1) return INVALID_INPUT;

    return Manager->AddCompany(CompanyID, Value);
}

StatusType AddEmployee(void* DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if (DS == nullptr || EmployeeID < 1 || CompanyID < 1 || Salary < 1 || Grade < 0) return INVALID_INPUT;

    return Manager->AddEmployee(EmployeeID, CompanyID, Salary, Grade);
}

StatusType RemoveCompany(void* DS, int CompanyID) {
    if (DS == nullptr || CompanyID < 1) return INVALID_INPUT;

    return Manager->RemoveCompany(CompanyID);
}

StatusType RemoveEmployee(void* DS, int EmployeeID) {
    if (DS == nullptr || EmployeeID < 1) return INVALID_INPUT;

    return Manager->RemoveEmployee(EmployeeID);
}

StatusType GetCompanyInfo(void* DS, int CompanyID, int* Value, int* NumEmployees) {
    if (DS == nullptr || CompanyID < 1 || Value == nullptr || NumEmployees == nullptr) return INVALID_INPUT;

    return Manager->GetCompanyInfo(CompanyID, Value, NumEmployees);
}

StatusType GetEmployeeInfo(void* DS, int EmployeeID, int* EmployerID, int* Salary, int* Grade) {
    if (DS == nullptr || EmployeeID < 1 || EmployerID == nullptr || Salary == nullptr || Grade == nullptr)
        return INVALID_INPUT;

    return Manager->GetEmployeeInfo(EmployeeID, EmployerID, Salary, Grade);
}

StatusType IncreaseCompanyValue(void* DS, int CompanyID, int ValueIncrease) {
    if (DS == nullptr || CompanyID < 1 || ValueIncrease < 1) return INVALID_INPUT;

    return Manager->IncreaseCompanyValue(CompanyID, ValueIncrease);
}

StatusType PromoteEmployee(void* DS, int EmployeeID, int SalaryIncrease, int BumpGrade) {
    if (DS == nullptr || EmployeeID < 1 || SalaryIncrease < 1) return INVALID_INPUT;

    return Manager->PromoteEmployee(EmployeeID, SalaryIncrease, BumpGrade);
}

StatusType HireEmployee(void* DS, int EmployeeID, int NewCompanyID) {
    if (DS == nullptr || EmployeeID < 1 || NewCompanyID < 1) return INVALID_INPUT;

    return Manager->HireEmployee(EmployeeID, NewCompanyID);
}

StatusType AcquireCompany(void* DS, int AcquirerID, int TargetID, double Factor) {
    if (DS == nullptr || AcquirerID < 1 || TargetID < 1 || AcquirerID == TargetID || Factor < 1) return INVALID_INPUT;

    return Manager->AcquireCompany(AcquirerID, TargetID, Factor);
}

StatusType GetHighestEarner(void* DS, int CompanyID, int* EmployeeID) {
    if (DS == nullptr || CompanyID == 0 || EmployeeID == nullptr) return INVALID_INPUT;

    return Manager->GetHighestEarner(CompanyID, EmployeeID);
}

StatusType GetAllEmployeesBySalary(void* DS, int CompanyID, int** Employees, int* NumOfEmployees) {
    if (DS == nullptr || CompanyID == 0 || Employees == nullptr || NumOfEmployees == nullptr) return INVALID_INPUT;

    return Manager->GetAllEmployeesBySalary(CompanyID, Employees, NumOfEmployees);
}

StatusType GetHighestEarnerInEachCompany(void* DS, int NumOfCompanies, int** Employees) {
    if (DS == nullptr || NumOfCompanies < 1 || Employees == nullptr) return INVALID_INPUT;

    return Manager->GetHighestEarnerInEachCompany(NumOfCompanies, Employees);
}

StatusType GetNumEmployeesMatching(void* DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId, int MinSalary,
                                   int MinGrade, int* TotalNumOfEmployees, int* NumOfEmployees) {
    if (DS == nullptr || CompanyID == 0 || MinEmployeeID < 0 || MaxEmployeeId < 0 || MaxEmployeeId < MinEmployeeID ||
        MinSalary < 0 || MinGrade < 0 || TotalNumOfEmployees == nullptr || NumOfEmployees == nullptr)
        return INVALID_INPUT;

    return Manager->GetNumEmployeesMatching(CompanyID, MinEmployeeID, MaxEmployeeId, MinSalary, MinGrade,
                                            TotalNumOfEmployees, NumOfEmployees);
}

void Quit(void** DS) {
    delete ((EmployeeManager*)*DS);

    *DS = NULL;
}
