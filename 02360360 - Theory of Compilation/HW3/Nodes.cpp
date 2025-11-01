#include "Nodes.hpp"

// ==================== Global Variables ====================

vector<SymbolTable*> ScopeTable;
vector<int> OffSetTable;
Entry* CurrentFunction;
int Loops;

// ==================== Helping Functions ====================

bool IDExists(string inID)
{

    // Iterating over all the SymbolTables and checking if the ID exists in any of them
    for (SymbolTable* Iter : ScopeTable) {
        
        if(Iter->CheckID(inID))
            return true;

    }

    return false;
}

void ProgramFinish()
{

    bool Found = false;

    // Looking for the main function
    for(Entry* Iter : ScopeTable.front()->Table) {
        if (Iter->EntryName == "main") {
            CloseScope();
            Found = true;
        }
    }

    if (Found == false) {
        output::errorMainMissing();
        exit(0);
    }

}

void FunctionFinish()
{
    CurrentFunction = nullptr;
}

void OpenScope()
{

    // Allocating a new SymbolTable and adding it to the ScopeTable
    SymbolTable* NewTable = new SymbolTable;
    ScopeTable.push_back(NewTable);

    // Duplicating the current Offset
    OffSetTable.push_back(OffSetTable.back());

}

void CloseScope()
{

    // Printing the end scope line
    output::endScope();

    // Printing the items that were in the scope
    for (Entry* Iter : ScopeTable.back()->Table)
        if (Iter->EntryFunc == false)
            output::printID(Iter->EntryName , Iter->EntryOffset , Iter->EntryType.back());
        else {
            string temp = Iter->EntryType.back();
            Iter->EntryType.pop_back();
            
            if(Iter->EntryType.size() == 0) {
                Iter->EntryType.push_back("");
            }

            output::printID(Iter->EntryName , Iter->EntryOffset , output::makeFunctionType(temp , Iter->EntryType));
        }

    // Removing the scope from the table
    ScopeTable.pop_back();
    OffSetTable.pop_back();

}

void AddLoop()
{
    Loops++;
}

void RemoveLoop()
{
    Loops--;
}

bool IntByteCheck(string inA , string inB){

    if (inA == inB)
        return true;

    bool ANum = (inA == "INT") ? true : false;
    bool BNum = (inB == "BYTE" || inB == "INT") ? true : false;

    return (ANum && BNum);
}

void InsertArgs(Formals* inList) {
    
    int counter = -1;
    for(int i = inList->DeclsList.size() - 1; i > -1; i--) {
        Entry* Temp = new Entry((inList->DeclsList[i])->Value , (inList->DeclsList[i])->DeclType , counter);
        ScopeTable.back()->Table.push_back(Temp);
        counter--;
    }

}

void PrintShit() {
    for (SymbolTable* Iter1 : ScopeTable) {
        
        cout << "===== OPEN =====" << endl;
        for(Entry* Iter2 : Iter1->Table) {
            cout << Iter2->EntryName << " --- " << Iter2->EntryType[0] << endl;
        }
        cout << "===== CLOSE =====" << endl;
    }
}

// ==================== Nodes & Types ====================

// ==================== Main Program

Program::Program()
{
    
    // Allocating a new global SymbolTable and adding it to the ScopeTable
    SymbolTable* NewTable = new SymbolTable;
    ScopeTable.push_back(NewTable);

    // Setting the first offset to 0
    OffSetTable.push_back(0);

    // Zeroing out the helping variables
    Loops = 0;

    // Creating an entries for the print & printi functions
    vector<string> PrintType = {"STRING" , "VOID"};
    Entry* PrintEntry = new Entry("print" , PrintType , 0);
    ScopeTable.back()->Table.push_back(PrintEntry);

    vector<string> PrintiType = {"INT" , "VOID"};
    Entry* PrintiEntry = new Entry("printi" , PrintiType , 0);
    ScopeTable.back()->Table.push_back(PrintiEntry);

}

// ==================== Functions

FuncDecl::FuncDecl(RetType* inRetType, Node* inName, Formals* inFormals)
{

    // Checking if the ID exists in the ScopeTable
    if(IDExists(inName->Value)){
        output::errorDef(yylineno, inName->Value);
        exit(0);
    }

    // Checking if any of the argument IDs already exists in the table
    for (FormalDecl* Iter : inFormals->DeclsList) {

        if(IDExists(Iter->Value)){
            output::errorDef(yylineno, Iter->Value);
            exit(0);
        }

    }

    // Generating the Type vector for this function
    for (FormalDecl* Iter : inFormals->DeclsList)
        Types.push_back(Iter->DeclType);

    Types.push_back(inRetType->Value);

    // Creating an entry for the function and adding it to the last scope
    Entry* NewEntry = new Entry(inName->Value , Types , 0);
    ScopeTable.back()->Table.push_back(NewEntry);

    // Setting the global current function pointer to this entry
    CurrentFunction = NewEntry;

}

Formals::Formals(FormalsList *inList)
{
    DeclsList = vector<FormalDecl*>(inList->DeclsList);
}

FormalsList::FormalsList(FormalDecl *inDecl)
{
    DeclsList.push_back(inDecl);
}

FormalsList::FormalsList(FormalDecl *inDecl, FormalsList *inList)
{
    DeclsList = vector<FormalDecl*>(inList->DeclsList);
    DeclsList.push_back(inDecl);
}

FormalDecl::FormalDecl(Type *inType, Node* inID)
{
    Value = inID->Value;
    DeclType = inType->Value;
}

// ==================== Statements

OStatement::OStatement(Exp *inExp)
{

    // Making sure that the expression is of boolean type
    if(inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

}

CStatement::CStatement(Exp *inExp)
{

    // Making sure that the expression is of boolean type
    if(inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
    
}

NIStatement::NIStatement(Type *inType, Node* inID)
{

    // Checking if the ID exists in the ScopeTable
    if(IDExists(inID->Value)){
        output::errorDef(yylineno, inID->Value);
        exit(0);
    }

    // Creating an entry for the variable and adding it to the last scope
    Entry* NewEntry = new Entry(inID->Value , inType->Value , OffSetTable.back()++);
    ScopeTable.back()->Table.push_back(NewEntry);

}

NIStatement::NIStatement(Type *inType, Node* inID, Exp *inExp)
{
    // Checking if the ID exists in the ScopeTable
    if(IDExists(inID->Value)){
        output::errorDef(yylineno, inID->Value);
        exit(0);
    }

    // Checking if the types match
    
    if(!IntByteCheck(inType->Value,inExp->ExpType)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Creating an entry for the variable and adding it to the last scope
    Entry* NewEntry = new Entry(inID->Value , inType->Value , OffSetTable.back()++);
    ScopeTable.back()->Table.push_back(NewEntry);

}

NIStatement::NIStatement(Node* inID, Exp *inExp)
{
    // Checking if the ID exists in the ScopeTable
    if(!IDExists(inID->Value)){
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }

    // Getting the entry from the table
    Entry* EntryInfo;
    for (SymbolTable* Iter : ScopeTable)
        for (Entry* Inner : Iter->Table) 
            if(Inner->EntryName == inID->Value)
                EntryInfo = Inner;

    // Making sure that the entry is not a function
    if (EntryInfo->EntryType.size() > 1){
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }

    // Checking if the types match
    if (!IntByteCheck(EntryInfo->EntryType.back(),inExp->ExpType)){
        output::errorMismatch(yylineno);
        exit(0);
    }
    
}

NIStatement::NIStatement(Node* inRetType , bool NA)
{

    // Checking if the return type of the current function matches inRetType
    if (CurrentFunction->EntryType.back() != "VOID") {
        output::errorMismatch(yylineno);
        exit(0);
    }

}

NIStatement::NIStatement(Exp *inExp)
{

    // Checking if the return type of the current function matches inExp type
    if (!IntByteCheck(CurrentFunction->EntryType.back(),inExp->ExpType)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

}

NIStatement::NIStatement(Node* inBrCon)
{

    // Checking if there's a loop currently running
    if(Loops == 0) {

        // Case - Continue
        if (inBrCon->Value == "continue")
        {
            output::errorUnexpectedContinue(yylineno);
            exit(0);
        }
        
        // Case - Break
        output::errorUnexpectedBreak(yylineno);
        exit(0);

    }

}

// ==================== Calls

Call::Call(Node* inID, ExpList *inExpList)
{

    // Checking if the Function exists in the ScopeTable
    if(!IDExists(inID->Value)){
        output::errorUndefFunc(yylineno, inID->Value);
        exit(0);
    }

    // Getting the entry from the table
    Entry* EntryInfo;
    for (SymbolTable* Iter : ScopeTable)
        for (Entry* Inner : Iter->Table) 
            if(Inner->EntryName == inID->Value)
                EntryInfo = Inner;

    // Checking if the argument types and size match
    if(EntryInfo->EntryType.size() != (inExpList->InnerList.size() + 1)){

        EntryInfo->EntryType.pop_back();
        output::errorPrototypeMismatch(yylineno, inID->Value, EntryInfo->EntryType);
        exit(0);
        
    }
    for(int i = 0; i < inExpList->InnerList.size(); i++) {

        if(!IntByteCheck(EntryInfo->EntryType[i], inExpList->InnerList[i])){

            EntryInfo->EntryType.pop_back();
            output::errorPrototypeMismatch(yylineno, inID->Value, EntryInfo->EntryType);
            exit(0);

        }

    }

    // Setting the value to the return value of the function
    Value = EntryInfo->EntryType.back();

}

Call::Call(Node* inID)
{

    // Checking if the Function exists in the ScopeTable
    if(!IDExists(inID->Value)){
        output::errorUndefFunc(yylineno, inID->Value);
        exit(0);
    }

    // Getting the entry from the table
    Entry* EntryInfo;
    for (SymbolTable* Iter : ScopeTable)
        for (Entry* Inner : Iter->Table) 
            if(Inner->EntryName == inID->Value)
                EntryInfo = Inner;

    // Checking if the argument types and size match
    if(EntryInfo->EntryType.size() != 1){

        vector<string> Temp = {""};
        output::errorPrototypeMismatch(yylineno, inID->Value, Temp);
        exit(0);
        
    }

    // Setting the value to the return value of the function
    Value = EntryInfo->EntryType.back();

}

// ==================== Expressions & Types

ExpList::ExpList(Exp *inExp, ExpList *inExpList)
{

    InnerList = vector<string>(inExpList->InnerList);
    InnerList.push_back(inExp->ExpType);
}

ExpList::ExpList(Exp *inExp)
{
    InnerList.push_back(inExp->ExpType);
}

Type::Type(Node* inType)
{

    // Checking which type we got
    if(inType->Value == "int")
        Value = "INT";
    else if (inType->Value == "byte")
        Value = "BYTE";
    else if (inType->Value == "bool")
        Value = "BOOL";
    else
        Value = "";

}

Exp::Exp(Exp *inExp)
{
    Value = inExp->Value;
    ExpType = inExp->ExpType;
}

Exp::Exp(Exp *inExp1, Exp *inExp2 , Exp *inExp3)
{

    // Making sure that the second expression is of boolean type
    if (inExp2->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Checking wether the if condition is true
    if (inExp2->Value == "true") {
        Value = inExp1->Value;
        ExpType = inExp1->ExpType;
    }
    else {
        Value = inExp3->Value;
        ExpType = inExp3->ExpType;
    }
}

Exp::Exp(Exp *inExp1, Node* inSign, Exp *inExp2 , string inOP)
{
    
    // Making sure the operands are of similar types
    if (!IntByteCheck(inExp1->ExpType,inExp2->ExpType)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Checking if we're dealing with a boolean equation
    if (inExp1->ExpType == "BOOL") {
        
        // AND || OR
        ExpType = "BOOL";
        
        // Getting the values of the expressions
        bool Left = (inExp1->Value == "true");
        bool Right = (inExp2->Value == "true");

        // Checking which operation we got
        if (inOP == "AND")
            Value = (Left && Right) ? "true" : "false";
        else if (inOP == "OR")
            Value = (Left || Right) ? "true" : "false";
        else {
            output::errorMismatch(yylineno);
            exit(0);
        }
    
    }
    else {

        // OPERATION
        ExpType = (inOP == "RELOP") ? "BOOL" : inExp1->ExpType;

    }
    
}

Exp::Exp(Call *inCall)
{
    ExpType = inCall->Value;
}

Exp::Exp(Node* inString , string inType)
{
    // Setting the value & type of the expression
    Value = inString->Value;
    ExpType = inType;

}

Exp::Exp(Node* inID)
{

    // Checking if the ID exists in the ScopeTable
    if(!IDExists(inID->Value)){
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }
    
    // Getting the entry from the table
    Entry* EntryInfo;
    for (SymbolTable* Iter : ScopeTable)
        for (Entry* Inner : Iter->Table) 
            if(Inner->EntryName == inID->Value)
                EntryInfo = Inner;

    Value = EntryInfo->EntryName;
    ExpType = EntryInfo->EntryType.back();

}

Exp::Exp(Node* inNot, Exp *inExp)
{

    // Checking if the expression is a bool
    if (inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Adjusting the value
    Value = (Value == "true") ? "false" : "true";

}

Exp::Exp(Type *inType, Exp *inExp)
{

}
