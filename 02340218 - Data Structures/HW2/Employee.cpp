#include "Employee.h"

// ===== Employee definitions =====

Employee::Employee(int inID, int inSal, int inGrade, int inEmployer, void* inComp) : EmpID(inID), EmpSal(inSal), EmpGrade(inGrade), Employer(inEmployer), EmployerPtr(inComp) {}

// ===== Get functions =====

int Employee::getID() { return EmpID; }

int Employee::getSal() { return EmpSal; }

int Employee::getGrade() { return EmpGrade; }

int Employee::getEmployer() { return Employer; }

void* Employee::getHirer()
{
	return EmployerPtr;
}

void Employee::setSal(int inSal) { EmpSal = inSal; }

void Employee::setEmployer(int inID, void* inPtr) { 
	Employer = inID;
	EmployerPtr = inPtr;
}

void Employee::bumpGrade(int bump) { EmpGrade += bump; }
