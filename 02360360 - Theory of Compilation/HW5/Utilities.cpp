#include "Utilities.hpp"

// ==================== Global Variables ====================

Compiler* MainTable;
int Loops;
Entry* CurrentFunction;
vector<OverrideHelper> OverrideList = {};

CodeGeneration* Generator;
CodeBuffer* Buffer;

// ==================== Helping Functions ====================

void ProgramFinish()
{
    // Looking for the main function
    Entry* MainFunction = MainTable->getEntry("main");
    if (MainFunction == nullptr) {
        output::errorMainMissing();
        exit(0);
    }
    else if (MainFunction->Type.back() != "VOID"){
        output::errorMainMissing();
        exit(0);
    }
    else if (MainFunction->Type.size() > 1){
        output::errorMainMissing();
        exit(0);
    }

    // Otherwise we close the scope
    MainTable->removeScope();

    // We print out the buffers
    Buffer->printGlobalBuffer();
    Buffer->printCodeBuffer();
}

void FunctionFinish()
{

    // Emitting a ret in case it was not specified in the code
    Buffer->emit((CurrentFunction->Type.back() == "VOID") ? "ret void" : ("ret " + ConvertType(CurrentFunction->Type.back()) + " 0"));

    // Emitting the closing braces
    Buffer->emit("}");
    Buffer->emit("");

    CurrentFunction = nullptr;

}

void InsertArgs(Formals* inList) {
    
    vector<string> ArgNames;
    vector<string> ArgTypes;

    for (int i = inList->DeclsList.size() - 1; i > -1; i--) {
        ArgNames.push_back((inList->DeclsList[i])->Value);
        ArgTypes.push_back((inList->DeclsList[i])->DeclType);
    }

    MainTable->addArguments(ArgNames,ArgTypes);
    
}

bool IntByteCheck(string inA , string inB){

    if (inA == inB)
        return true;

    bool ANum = (inA == "BYTE" || inA == "INT") ? true : false;
    bool BNum = (inB == "BYTE" || inB == "INT") ? true : false;

    return (ANum && BNum);
}

void OpenScope()
{
    //cout << "OPEN " << yylineno << endl;
    MainTable->addScope();
}

void CloseScope()
{
    //cout << "CLOSE " <<  yylineno << endl;
    MainTable->removeScope();
}

void AddLoop()
{
    Loops++;
}

void RemoveLoop()
{
    Loops--;
}

void CheckBool(Exp *inExp)
{
    // Making sure that the expression is of boolean type
    if(inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

string ConvertType(string inType) {
    // Matching the given type with its appropriate LLVM name
    if (inType == "VOID")
        return "void";    
    else if (inType == "INT")
        return "i32";
    else if (inType == "BYTE")
        return "i8";   
    else if (inType == "BOOL")
        return "i1";   
    else
        return "i8*";
}

string TruncRegister(string inReg, string inType) {
    string TruncedReg = Generator->freshRegister();
    Buffer->emit(TruncedReg + " = trunc i32 " + inReg + " to " + inType);
    return TruncedReg;
}

string ExtendRegister(string inReg, string inType) {
    string ExtendedRegister = Generator->freshRegister();
    Buffer->emit(ExtendedRegister + " = zext " + inType + " " + inReg + " to i32");
    return ExtendedRegister;
}

int GetIdentifier(Entry *inEntry)
{
    for (OverrideHelper Iter : OverrideList) {
        if(Iter.FuncName == inEntry->Name) {
            if (Iter.FuncTypes == inEntry->Type)
                return Iter.Identifier;
        }
    }

    return -1;
}

void BoolJumps(Exp* inExp) {
    // Generating a series of jumps instead of using registers
    string TrueJump = Generator->freshLabel();
    Buffer->emit("br label %" + TrueJump);
    Buffer->emit("");
    Buffer->emit(TrueJump + ":");
    int First = Buffer->emit("br label @");

    string FalseJump = Generator->freshLabel();
    Buffer->emit("br label %" + FalseJump);
    Buffer->emit("");
    Buffer->emit(FalseJump + ":");
    int Second = Buffer->emit("br label @");

    string NextJump = Generator->freshLabel();
    Buffer->emit("");
    Buffer->emit(NextJump + ":");

    // Merging and backpatching
    BufferList Next = Buffer->merge(Buffer->makelist(pair<int, BranchLabelIndex>(First, FIRST)) , Buffer->makelist(pair<int, BranchLabelIndex>(Second, FIRST)));
    Buffer->bpatch(inExp->TrueList, TrueJump);
    Buffer->bpatch(inExp->FalseList, FalseJump);
    Buffer->bpatch(Next, NextJump);
    
    // Emitting the phi line
    inExp->Register = Generator->freshRegister();
    Buffer->emit(inExp->Register + " = phi i1 [1, %" + TrueJump + "], [0, %" + FalseJump + "]");
}

// ==================== Compiler Implementation ====================

void SymbolTable::addVariable(string inName, string inType)
{
    Data.push_back(new Entry(inName, inType, CurrentOffset));
    CurrentOffset++;
}

void SymbolTable::addFunction(string inName, vector<string> inType, bool toOverride)
{
    CurrentFunction = new Entry(inName, inType, 0, toOverride);
    Data.push_back(CurrentFunction);
}

void SymbolTable::addArgument(string inName, string inType, int inOffset)
{
    Data.push_back(new Entry(inName, inType, inOffset));
}

Entry* SymbolTable::getEntry(string inName)
{
    for (Entry* Iter : Data) {
        if (Iter->Name == inName)
            return Iter;
    }
    return nullptr;
}

Compiler::Compiler()
{
    // Creating the main scope and initializing the offset table
    OffsetTable.push_back(0);
    addScope();
    
    // Adding the "print" and "printi" entries
    vector<string> printPar = { "STRING", "VOID" };
    vector<string> printiPar = { "INT", "VOID" };
    addFunction("print", printPar, false);
    addFunction("printi", printiPar, false);
}

void Compiler::addScope()
{
    // Creating a new symbol table and adding it to the stack
    SymbolTable* newTable = new SymbolTable(OffsetTable.back());

    // Initializing/Passing the stack register
    newTable->StackRegister = (TableStack.size()) ? TableStack.back()->StackRegister : "";

    TableStack.push_back(newTable);

    // Pushing a duplicate offset
    OffsetTable.push_back(OffsetTable.back());
}

void Compiler::removeScope()
{
    // Printing the endScope line and listing the entries
    output::endScope();
    SymbolTable* Current = TableStack.back();
    for (Entry* Iter : Current->Data) {
        if (Iter->FunctionFlag) {
            string ReturnType = Iter->Type.back();
            Iter->Type.pop_back();
            
            reverse(Iter->Type.begin(),Iter->Type.end());
            //output::printID(Iter->Name, Iter->Offset, output::makeFunctionType(ReturnType, Iter->Type));
        }
        else {
            //output::printID(Iter->Name, Iter->Offset, Iter->Type.back());
        }
    }

    // Popping and deleting the scope
    TableStack.pop_back();
    OffsetTable.pop_back();
    delete(Current);
}

void Compiler::addVariable(string inName, string inType)
{
    // Making sure the ID does not already exist in the table
    if (lookupEntry(inName)) {
        output::errorDef(yylineno, inName);
        exit(0);
    }

    // Adding the variable to the current table and incrementing the offset
    SymbolTable* Current = TableStack.back();
    Current->addVariable(inName, inType);
    int newOffset = OffsetTable.back() + 1;
    OffsetTable.pop_back();
    OffsetTable.push_back(newOffset);
}

void Compiler::addFunction(string inName, vector<string> inType, bool toOverride)
{
    // Making sure that the main function is not overridable
    if (inName == "main" && toOverride) {
        output::errorMainOverride(yylineno);
        exit(0);
    }

    // Checking if the ID is available or overridable
    if (lookupEntry(inName)) {

        // Checking if the parameters are the same
        Entry* PrevEntry = getEntry(inName);

        if (PrevEntry->Type == inType) {
            output::errorDef(yylineno, inName);
            exit(0);
        }

        // Checking if we're overriding
        if (!toOverride && PrevEntry->overridable == false) {
            output::errorDef(yylineno, inName);
            exit(0);
        }

        // Checking if the function is overridable
        if (PrevEntry->overridable == false) {
            output::errorFuncNoOverride(yylineno, inName);
            exit(0);
        }

        // Checking if the new definition is overridable
        if (toOverride == false) {
            output::errorOverrideWithoutDeclaration(yylineno, inName);
            exit(0);
        }
        
    }

    // Adding the function to the current table
    SymbolTable* Current = TableStack.back();
    Current->addFunction(inName, inType, toOverride);
}

void Compiler::addArguments(vector<string> inNames, vector<string> inTypes)
{
    // Checking if any of the argument IDs already exists in the table
    for (string Iter : inNames) {
        if(lookupEntry(Iter)){
            output::errorDef(yylineno, Iter);
            exit(0);
        }
    }

    // Checking if any of the arguments is a duplicate of itself
    for (string Iter : inNames) {
        int count = 0;
        for (string CurName : inNames)
            if (Iter == CurName)
                count++;

        if (count != 1) {
            output::errorDef(yylineno, Iter);
            exit(0);
        }
    }

    // Adding the arguments to the table
    SymbolTable* Current = TableStack.back();
    for (int i = 0; i < inNames.size(); i++) {
        Current->addArgument(inNames[i],inTypes[i],-(i+1));
    }
}

bool Compiler::lookupEntry(string inName)
{
    // Looking for the entry in the table
    for (SymbolTable* CurrentTable : TableStack)
        for (Entry* Iter : CurrentTable->Data)
            if (Iter->Name == inName)
                return true;

    return false;
}

bool Compiler::lookupOverride(string inName)
{
    // Looking for the entry in the table
    for (SymbolTable* CurrentTable : TableStack)
        for (Entry* Iter : CurrentTable->Data)
            if (Iter->Name == inName)
                return Iter->overridable;

    return false;
}

void Compiler::printAll()
{
    cout << "-- ----- ----- --" << endl;

    for (SymbolTable* CurrentTable : TableStack) {
        cout << "-- Scope Start --" << endl;
        for (Entry* Iter : CurrentTable->Data) {
            if (Iter->FunctionFlag) {
                string ReturnType = Iter->Type.back();
                Iter->Type.pop_back();
                output::printID(Iter->Name, Iter->Offset, output::makeFunctionType(ReturnType, Iter->Type));
            }
            else {
                output::printID(Iter->Name, Iter->Offset, Iter->Type.back());
            }
        }
        cout << "-- Scope End --" << endl;
    }
}

// ==================== Code Generator Implementation ====================

string CodeGeneration::freshRegister()
{
    // Creating a new variable and returning it
    string newRegister = "%reg_" + to_string(CurrentRegisters);
    CurrentRegisters++;
    return newRegister;
}

string CodeGeneration::freshLabel()
{
    // Creating a new label and returning it
    string newLabel = "label_" + to_string(CurrentLabels);
    CurrentLabels++;
    return newLabel;
}

// ==================== Nodes Implementation ====================

Program::Program()
{
    // Initializing the global SymbolTable
    MainTable = new Compiler();

    // Zeroing out the helping variables
    Loops = 0;
    CurrentFunction == nullptr;

    // == CODEGEN ==

    // Initializing the code generator and buffer
    Generator = new CodeGeneration();
    Buffer = new CodeBuffer();
    
    // Declaring the standard library functions and definitions
    Buffer->emitGlobal("declare i32 @printf(i8*, ...)");
    Buffer->emitGlobal("declare void @exit(i32)");
    Buffer->emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    Buffer->emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    Buffer->emitGlobal("@DivError = private unnamed_addr constant [23 x i8] c\"Error division by zero\\00\"");
    Buffer->emitGlobal("");

    // Emitting a division by zero handling function
    Buffer->emitGlobal("define void @ZeroDivExit() {");
    Buffer->emitGlobal("    %DivZero = getelementptr [23 x i8], [23 x i8]* @DivError, i32 0, i32 0");
    Buffer->emitGlobal("    call void @print (i8* %DivZero)");
    Buffer->emitGlobal("    call void @exit(i32 0)");
    Buffer->emitGlobal("    ret void");
    Buffer->emitGlobal("}");
    Buffer->emitGlobal("");

    // Emitting the print functions
    Buffer->emitGlobal("define void @printi(i32) {");
    Buffer->emitGlobal("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
    Buffer->emitGlobal("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
    Buffer->emitGlobal("    ret void");
    Buffer->emitGlobal("}");
    Buffer->emitGlobal("");

    Buffer->emitGlobal("define void @print(i8*) {");
    Buffer->emitGlobal("    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
    Buffer->emitGlobal("    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
    Buffer->emitGlobal("    ret void");
    Buffer->emitGlobal("}");
    Buffer->emitGlobal("");
}

// ==================== Functions

FuncDecl::FuncDecl(RetType* inRetType, Node* inName, Formals* inFormals, OverRide* inOverride)
{
    
    // Generating the Type vector for this function
    for (FormalDecl* Iter : inFormals->DeclsList)
        Types.push_back(Iter->DeclType);

    Types.push_back(inRetType->Value);
    // Adding it to the last scope
    MainTable->addFunction(inName->Value,Types,inOverride->value);

    // Adding the function to the override list if applicable
    if (inOverride->value)
        OverrideList.push_back(OverrideHelper(inName->Value,Types,OverrideList.size()));

    // == CODEGEN ==

    // Putting the parameters in a single string
    string Parameters = "";
    for (int i = Types.size() - 2; i >= 0; i--) {

        // Converting the regular types into their LLVM names
        Parameters += ConvertType(Types[i]);
        
        if (i != 0)
            Parameters += ", ";
    }

    // Declaring the function
    if (inOverride->value)
        Buffer->emit("define " + ConvertType(inRetType->Value) + " @" + inName->Value + "_" + to_string(OverrideList.size() - 1) + "(" + Parameters + ") {");
    else
        Buffer->emit("define " + ConvertType(inRetType->Value) + " @" + inName->Value + "(" + Parameters + ") {");

    // Allocating the stack and saving the register that holds it in the scopetable
    MainTable->TableStack.back()->StackRegister = Generator->freshRegister();
    Buffer->emit(MainTable->TableStack.back()->StackRegister + " = alloca i32, i32 50");
    
}

OverRide::OverRide(bool inVal)
{
    value = inVal;
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

RetType::RetType(Node *inType)
{
        if (inType->Value == "int" || inType->Value == "INT")
            Value = "INT";
        else if (inType->Value == "void" || inType->Value == "VOID")
            Value = "VOID";
        else if (inType->Value == "byte" || inType->Value == "BYTE")
            Value = "BYTE";
        else if (inType->Value == "bool" || inType->Value == "BOOL")
            Value = "BOOL";
        else
            Value = "";

}

// ==================== Statements

Statements::Statements(Statement *inState)
{
    // Merging the lists
    BreakList = Buffer->merge(BreakList, inState->BreakList);
    ContinueList = Buffer->merge(ContinueList, inState->ContinueList);
}

Statements::Statements(Statements *inStates, Statement *inState)
{
    // Merging the lists
    BreakList = Buffer->merge(inStates->BreakList, inState->BreakList);
    ContinueList = Buffer->merge(inStates->ContinueList, inState->ContinueList);
}

// IF
Statement::Statement(Exp* inExp, Label* inTrueLabel, Statement* inStatement)
{

    // Making sure that the expression is of boolean type
    if(inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Backpatching the boolean expression to the start of the statement in case it was true
    Buffer->bpatch(inExp->TrueList, inTrueLabel->Value);

    // Generating the false label and backpatching the expression
    string FalseLabel = Generator->freshLabel();
    Buffer->bpatch(inExp->FalseList, FalseLabel);
    Buffer->emit("br label %" + FalseLabel);
    Buffer->emit("");
    Buffer->emit(FalseLabel + ":");

    // Merging the break/continue lists
    BreakList = Buffer->merge(BreakList, inStatement->BreakList);
    ContinueList = Buffer->merge(ContinueList, inStatement->ContinueList);
}

// IF-ELSE
Statement::Statement(Exp* inExp, Label* inTrueLabel, Statement* inTrueStatement, Label* inFalseLabel, Statement* inFalseStatement, Branch* inBranch)
{
    // Making sure that the expression is of boolean type
    if(inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Backpatching the boolean expression to the start of the truestatement in case it was true
    Buffer->bpatch(inExp->TrueList, inTrueLabel->Value);

    // Backpatching the boolean expression to the start of the falsestatement in case it was false
    Buffer->bpatch(inExp->FalseList, inFalseLabel->Value);

    // Generating the next label and backpatching the expression
    string NextLabel = Generator->freshLabel();
    BufferList Next = Buffer->makelist(pair<int, BranchLabelIndex>(inBranch->Location, FIRST));
    Buffer->bpatch(Next, NextLabel);
    Buffer->emit("br label %" + NextLabel);
    Buffer->emit("");
    Buffer->emit(NextLabel + ":");

    // Merging the break/continue lists
    BreakList = Buffer->merge(inTrueStatement->BreakList, inFalseStatement->BreakList);
    ContinueList = Buffer->merge(inTrueStatement->ContinueList, inFalseStatement->ContinueList);
}

// WHILE
Statement::Statement(Exp* inExp, Label* inTrueLabel, Statement* inStatement, Label* inLoopLabel)
{

    // Making sure that the expression is of boolean type
    if(inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Backpatching the boolean expression to the start of the statement in case it was true
    Buffer->bpatch(inExp->TrueList, inTrueLabel->Value);

    // Emitting a branch at the end of the while loop
    Buffer->emit("br label %" + inLoopLabel->Value);

    // Generating the false label and backpatching the expression
    string FalseLabel = Generator->freshLabel();
    Buffer->bpatch(inExp->FalseList, FalseLabel);
    Buffer->emit("br label %" + FalseLabel);
    Buffer->emit("");
    Buffer->emit(FalseLabel + ":");

    // Backpatching the break/continue lists
    Buffer->bpatch(inStatement->BreakList, FalseLabel);
    Buffer->bpatch(inStatement->ContinueList, inLoopLabel->Value);
}

// Type ID SC
Statement::Statement(Type *inType, Node* inID)
{

    // Checking if the ID exists in the ScopeTable
    if(MainTable->lookupEntry(inID->Value)){
        output::errorDef(yylineno, inID->Value);
        exit(0);
    }

    // Adding it to the last scope
    MainTable->addVariable(inID->Value , inType->Value);

// TODO - bool?

    // Grabbing the entry for the offset
    Entry* EntryInfo = MainTable->getEntry(inID->Value);
    string StackReg = MainTable->getRegister(inID->Value);

    // Creating a default initialization
    string NewReg = Generator->freshRegister();
    Buffer->emit(NewReg + " = add i32 0,0");

    // Storing the reg in the stack
    string StoringReg = Generator->freshRegister();
    Buffer->emit(StoringReg + " = getelementptr i32, i32* " + StackReg + ", i32 " + to_string(EntryInfo->Offset));
    Buffer->emit("store i32 " + NewReg + ", i32* " + StoringReg);
}

// Type ID ASSIGN Exp SC
Statement::Statement(Type *inType, Node* inID, Exp *inExp)
{
    
    // Checking if the ID exists in the ScopeTable
    if(MainTable->lookupEntry(inID->Value)){
        output::errorDef(yylineno, inID->Value);
        exit(0);
    }

    // Checking if the types match
    if(inType->Value != inExp->ExpType) {
        
        // Checking if we can cast
        if (!(inType->Value == "INT" && inExp->ExpType == "BYTE")){
            output::errorMismatch(yylineno);
            exit(0);
        }
    }

    // Adding it to the last scope
    MainTable->addVariable(inID->Value , inType->Value);

    // Switching to a series of jumps in case the exp was a bool
    if (inExp->ExpType == "BOOL")
        BoolJumps(inExp);

    string ExpRegister;
    // Extending the register in case it was a byte
    ExpRegister = (inExp->ExpType == "BYTE") ? ExtendRegister(inExp->Register, "i8") : inExp->Register;

    // Extending the register in case it was a bool
    ExpRegister = (inExp->ExpType == "BOOL") ? ExtendRegister(inExp->Register, "i1") : ExpRegister;

    // Storing the expression in the stack according to the offset
    string StoringRegister = Generator->freshRegister();

    // Since we cannot reuse the same register we allocate a temporary one and use it to store to the stack
    Buffer->emit(StoringRegister + " = getelementptr i32, i32* " + MainTable->getRegister(inID->Value) + ", i32 " + (to_string(MainTable->getEntry(inID->Value)->Offset)));
    Buffer->emit("store i32 " + ExpRegister + ", i32* " + StoringRegister);
}

// ID ASSIGN Exp SC
Statement::Statement(Node* inID, Exp *inExp)
{
    // Checking if the ID exists in the ScopeTable
    if(!(MainTable->lookupEntry(inID->Value))){
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }

    // Getting the entry from the table
    Entry* EntryInfo = MainTable->getEntry(inID->Value);

    // Making sure that the entry is not a function
    if (EntryInfo->Type.size() > 1){
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }

    // Checking if the types match
    if (!IntByteCheck(EntryInfo->Type.back(),inExp->ExpType)){
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Switching to a series of jumps in case the exp was a bool
    if (inExp->ExpType == "BOOL")
        BoolJumps(inExp);

    string ExpRegister;
    // Extending the register in case it was a byte
    ExpRegister = (inExp->ExpType == "BYTE") ? ExtendRegister(inExp->Register, "i8") : inExp->Register;

    // Extending the register in case it was a bool
    ExpRegister = (inExp->ExpType == "BOOL") ? ExtendRegister(inExp->Register, "i1") : ExpRegister;

    // Storing the expression in the stack according to the offset
    string StoringRegister = Generator->freshRegister();
    
    // Since we cannot reuse the same register we allocate a temporary one and use it to store to the stack
    Buffer->emit(StoringRegister + " = getelementptr i32, i32* " + MainTable->getRegister(inID->Value) + ", i32 " + (to_string(MainTable->getEntry(inID->Value)->Offset)));
    Buffer->emit("store i32 " + ExpRegister + ", i32* " + StoringRegister);
    
}

// Call SC
Statement::Statement(Call *inCall)
{
    // Checking if bool and backpatching if so
    if (inCall->Value != "BOOL")
        return;

    Exp* Cast = new Exp();
    Cast->ExpType = inCall->Value;
    Cast->TrueList = inCall->TrueList;
    Cast->FalseList = inCall->FalseList;
    Cast->Register = inCall->Register;
    BoolJumps(Cast);
}

// RETURN SC
Statement::Statement(Node* inRetType , bool NA)
{

    // Checking if the return type of the current function matches inRetType
    if (CurrentFunction->Type.back() != "VOID") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Emitting void return
    Buffer->emit("ret void");

}

// RETURN Exp SC
Statement::Statement(Exp *inExp)
{
    // Checking if the return type of the current function matches inExp type
    if (!IntByteCheck(CurrentFunction->Type.back(),inExp->ExpType)) {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Checking what's the type of the variable we're returning
    if (inExp->ExpType == "BOOL")
        BoolJumps(inExp);

    if (CurrentFunction->Type.back() == "INT" && inExp->ExpType == "BYTE") {
        // Extending the register

        // Emitting the return line
        Buffer->emit("ret i32 " + ExtendRegister(inExp->Register,"i8"));
        return;
    }
    // Emitting the return line
    Buffer->emit("ret " + ConvertType(inExp->ExpType) + " " + inExp->Register);

}

// BREAK SC || CONTINUE SC
Statement::Statement(Node* inBrCon)
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

    // Emitting a branch
    int BranchAddress = Buffer->emit("br label @");

    // Adjusting the break/continue lists
    if (inBrCon->Value == "break")
        BreakList = Buffer->makelist(pair<int, BranchLabelIndex>(BranchAddress,FIRST));
    else
        ContinueList = Buffer->makelist(pair<int, BranchLabelIndex>(BranchAddress,FIRST));

}

// ==================== Calls

// ID LPAREN ExpList RPAREN
Call::Call(Node* inID, ExpList *inExpList)
{
    
    // Checking if the Function exists in the ScopeTable
    bool FoundName = false;
    bool FoundSize = false;
    bool FoundType = false;
    Entry* EntryInfo;
    for (SymbolTable* CurrentTable : MainTable->TableStack){
        for (Entry* Iter : CurrentTable->Data) {
            if (Iter->Name == inID->Value){
                FoundName = true;

                // Checking if the sizes are the same
                if (Iter->Type.size() == (inExpList->InnerList.size() + 1)) {
                    FoundSize = true;

                    // Checking if the types are the same or castable
                    for(int i = 0; i < inExpList->InnerList.size(); i++) {
                        if ((Iter->Type[inExpList->InnerList.size() - i - 1] == inExpList->InnerList[i]->ExpType) || (Iter->Type[inExpList->InnerList.size() - i - 1] == "INT" && inExpList->InnerList[i]->ExpType == "BYTE")) {
                            FoundType = true;
                            EntryInfo = Iter;
                            continue;
                        }

                        FoundType = false;
                    }
                }

                if (FoundType)
                    break;
            }
        }

        if (FoundType)
            break;
            
    }
                
    // No function with that name found
    if(FoundName == false){
        output::errorUndefFunc(yylineno, inID->Value);
        exit(0);
    }

    // Function found but with different number of arguments
    if(FoundSize == false){
        output::errorPrototypeMismatch(yylineno, inID->Value);
        exit(0);
    }

    // Function found but with different types
    if(FoundType == false){

        // Function is overridable
        if (EntryInfo->overridable) {
            output::errorAmbiguousCall(yylineno, inID->Value);
            exit(0);
        }
        else {
            output::errorPrototypeMismatch(yylineno, inID->Value);
            exit(0);
        }
        
    }
    
    // Setting the value to the return value of the function
    Value = EntryInfo->Type.back();

    // == CODEGEN ==

    // Putting the parameters in a single string
    string Parameters = "";

    for (int i = 0; i < inExpList->InnerList.size(); i++) {

        // Checking if the ExpType and the function parameter type match
        if (inExpList->InnerList[i]->ExpType == EntryInfo->Type[inExpList->InnerList.size() - i - 1])
            Parameters += ConvertType(inExpList->InnerList[i]->ExpType) + " " + inExpList->InnerList[i]->Register;
        else 
            Parameters += "i32 " + ExtendRegister(inExpList->InnerList[i]->Register, ConvertType(inExpList->InnerList[i]->ExpType));

        if (i < inExpList->InnerList.size() - 1)
            Parameters += ", ";
    }

    // Calling the function
    if (Value == "VOID")
        Buffer->emit("call void @" + inID->Value + " (" + Parameters + ")");
    else {
        Register = Generator->freshRegister();
        
        // Checking if it is an overridden function
        if (EntryInfo->overridable)
            Buffer->emit(Register + " = call " + ConvertType(Value) + " @" + inID->Value + "_" + to_string(GetIdentifier(EntryInfo)) + " (" + Parameters + ")");
        else 
            Buffer->emit(Register + " = call " + ConvertType(Value) + " @" + inID->Value + " (" + Parameters + ")");

        // Emitting a branch in case the return type is a bool
        if (Value == "BOOL") {
            string Compare = Generator->freshRegister();
            Buffer->emit(Compare + " = icmp ne i1 0, " + Register);
            Register = Compare;
            int Address = Buffer->emit("br i1 " + Register + " ,label @, label @");
            TrueList = Buffer->makelist(pair<int, BranchLabelIndex>(Address, FIRST));
            FalseList = Buffer->makelist(pair<int, BranchLabelIndex>(Address, SECOND));
        }
    }
}

// ID LPAREN RPAREN
Call::Call(Node* inID)
{

    // Checking if the Function exists in the ScopeTable
    bool FoundName = false;
    bool FoundSize = false;
    Entry* EntryInfo;
    for (SymbolTable* CurrentTable : MainTable->TableStack)
        for (Entry* Iter : CurrentTable->Data)
            if (Iter->Name == inID->Value){
                FoundName = true;
                if (Iter->Type.size() == 1) {
                    FoundSize = true;
                    EntryInfo = Iter;
                }
            }
                
    if(FoundName == false){
        output::errorUndefFunc(yylineno, inID->Value);
        exit(0);
    }

    // Checking if the argument types and size match
    if(FoundSize == false){
        output::errorPrototypeMismatch(yylineno, inID->Value);
        exit(0);
    }

    // Making sure that the entry is a function
    if (EntryInfo->FunctionFlag == false) {
        output::errorUndefFunc(yylineno, inID->Value);
        exit(0);
    }

    // Setting the value to the return value of the function
    Value = EntryInfo->Type.back();

    // == CODEGEN ==

    // Calling the function
    if (Value == "VOID")
        Buffer->emit("call void @" + inID->Value + "()");
    else {
        Register = Generator->freshRegister();
        // Checking if it is an overridden function
        if (EntryInfo->overridable)
            Buffer->emit(Register + " = call " + ConvertType(Value) + " @" + inID->Value + "_" + to_string(GetIdentifier(EntryInfo)) + " ()");
        else 
            Buffer->emit(Register + " = call " + ConvertType(Value) + " @" + inID->Value + " ()");

        // Emitting a branch in case the return type is a bool
        if (Value == "BOOL") {
            string Compare = Generator->freshRegister();
            Buffer->emit(Compare + " = icmp ne i1 0, " + Register);
            Register = Compare;
            int Address = Buffer->emit("br i1 " + Register + " ,label @, label @");
            TrueList = Buffer->makelist(pair<int, BranchLabelIndex>(Address, FIRST));
            FalseList = Buffer->makelist(pair<int, BranchLabelIndex>(Address, SECOND));
        }
    }
}

// ==================== Expressions & Types

ExpList::ExpList(Exp *inExp, ExpList *inExpList)
{
    InnerList = vector<Exp*>(inExpList->InnerList);

    // Checking if the expression is not a bool or if it already has a register
    if (inExp->ExpType != "BOOL"){
        InnerList.push_back(inExp);
        return;
    }
    
    // Otherwise we generate a series of jumps instead of using registers
    BoolJumps(inExp);

    // Adding the exp to the list
    InnerList.push_back(inExp);
}

ExpList::ExpList(Exp *inExp)
{
    // Checking if the expression is not a bool or if it already has a register
    if (inExp->ExpType != "BOOL"){
        InnerList.push_back(inExp);
        return;
    }

    // Otherwise we generate a series of jumps instead of using registers
    BoolJumps(inExp);

    // Adding the exp to the list
    InnerList.push_back(inExp);
}

Type::Type(Node* inType)
{

    // Checking which type we got
    if(inType->Value == "int" || inType->Value == "INT")
        Value = "INT";
    else if (inType->Value == "byte" || inType->Value == "BYTE")
        Value = "BYTE";
    else if (inType->Value == "bool" || inType->Value == "BOOL")
        Value = "BOOL";
    else
        Value = "";

}

// LPAREN Exp RPAREN
Exp::Exp(Exp *inExp)
{
    Value = inExp->Value;
    ExpType = inExp->ExpType;

    // == CODEGEN ==
    TrueList = inExp->TrueList;
    FalseList = inExp->FalseList;
    Register = inExp->Register;
}

// Exp AND || OR || RELOP || BINOP Exp
Exp::Exp(Exp *inExp1, Node* inSign, Exp *inExp2 , string inOP, Label* inLabel)
{
    
    // Making sure the operands are of similar types
    if (!IntByteCheck(inExp1->ExpType,inExp2->ExpType) || inExp1->ExpType == "STRING" || inExp2->ExpType == "STRING") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Checking if we're dealing with a boolean equation
    if (inExp1->ExpType == "BOOL" && inExp2->ExpType == "BOOL") {

        // AND || OR
        ExpType = "BOOL";

        // Checking which operation we got
        if (inOP == "AND") {

            // Patching the first operand's truelist to go check the second operand
            Buffer->bpatch(inExp1->TrueList, inLabel->Value);

            // Setting the AND truelist to be the truelist of the second operand
            TrueList = BufferList(inExp2->TrueList);

            // Merging the operands falselist to the AND falselist
            FalseList = Buffer->merge(inExp1->FalseList, inExp2->FalseList);

        }
        else if (inOP == "OR"){

            // Patching the first operand's falselist to go check the second operand
            Buffer->bpatch(inExp1->FalseList, inLabel->Value);

            // Merging the operands truelist to the AND truelist
            TrueList = Buffer->merge(inExp1->TrueList, inExp2->TrueList);

            // Setting the AND falselist to be the falselist of the second operand
            FalseList = BufferList(inExp2->FalseList);
            
        }
        else {
            output::errorMismatch(yylineno);
            exit(0);
        }
    
    }
    // Checking if we're dealing with a relation operation
    else if (inOP == "RELOP") {

        // Setting the expression type to bool
        ExpType = "BOOL";

        // Converting the RELOP to its LLVM variant
        string LLVMOP;
        if (inSign->Value == "==")
            LLVMOP = "eq";
        else if (inSign->Value == "!=")
            LLVMOP = "ne";
        else if (inSign->Value == "<")
            LLVMOP = "slt";
        else if (inSign->Value == ">")
            LLVMOP = "sgt";
        else if (inSign->Value == "<=")
            LLVMOP = "sle";
        else
            LLVMOP = "sge";


        // Allocating a register and emitting the code line
        Register = Generator->freshRegister();

        // Checking the types
        if (inExp1->ExpType == "BYTE" && inExp2->ExpType == "BYTE") {
            // Both bytes
            Buffer->emit(Register + " = icmp " + LLVMOP + " i8 " + inExp1->Register + ", " + inExp2->Register);
        }
        else {
            // Both are int || one is byte

            // Extending the register that is a byte
            string OP1 = (inExp1->ExpType == "BYTE") ? ExtendRegister(inExp1->Register, "i8") : inExp1->Register;
            string OP2 = (inExp2->ExpType == "BYTE") ? ExtendRegister(inExp2->Register, "i8") : inExp2->Register;

            Buffer->emit(Register + " = icmp " + LLVMOP + " i32 " + OP1 + ", " + OP2);
        }

        // Adjusting the true/false lists
        int JumpAddress = Buffer->emit("br i1 " + Register + ", label @, label @");
        TrueList = Buffer->makelist(pair<int, BranchLabelIndex>(JumpAddress, FIRST));
        FalseList = Buffer->makelist(pair<int, BranchLabelIndex>(JumpAddress, SECOND));
        
    }
    // We're dealing with a binary operation
    else {

        // Setting the expression type to the result's type
        ExpType = (inExp1->ExpType == "INT" || inExp2->ExpType == "INT") ? "INT" : "BYTE";

        // Converting the BINOP to its LLVM variant
        string LLVMOP;
        if (inSign->Value == "+")
            LLVMOP = "add";
        else if (inSign->Value == "-")
            LLVMOP = "sub";
        else if (inSign->Value == "*")
            LLVMOP = "mul";
        else
            LLVMOP = (ExpType == "INT") ? "sdiv" : "udiv";

        // Checking for zero division
        if (inSign->Value == "/"){
            // Checking if the second expression is zero
            string Checker = Generator->freshRegister();
            string ErrorLine = Generator->freshLabel();
            string NextLine = Generator->freshLabel();
            Buffer->emit(Checker + " = icmp ne " + ConvertType(inExp2->ExpType) + " " + inExp2->Register +", 0");
            Buffer->emit("br i1 " + Checker + ", label %" + NextLine + ", label %" + ErrorLine);

            Buffer->emit("br label %" + ErrorLine);
            Buffer->emit("");
            Buffer->emit(ErrorLine + ":");
            Buffer->emit("call void @ZeroDivExit()");

            Buffer->emit("br label %" + NextLine);
            Buffer->emit("");
            Buffer->emit(NextLine + ":");
        }

        // Allocating a register and emitting the code line
        Register = Generator->freshRegister();

        // Checking the types
        if (inExp1->ExpType == "BYTE" && inExp2->ExpType == "BYTE") {
            // Both bytes
            Buffer->emit(Register + " = " + LLVMOP + " i8 " + inExp1->Register + ", " + inExp2->Register);
        }
        else {
            // Both are int || one is byte

            // Extending the register that is a byte
            string OP1 = (inExp1->ExpType == "BYTE") ? ExtendRegister(inExp1->Register, "i8") : inExp1->Register;
            string OP2 = (inExp2->ExpType == "BYTE") ? ExtendRegister(inExp2->Register, "i8") : inExp2->Register;

            Buffer->emit(Register + " = " + LLVMOP + " i32 " + OP1 + ", " + OP2);
        }
    }
    
}

// Call
Exp::Exp(Call *inCall)
{
    ExpType = inCall->Value;

    // == CODEGEN ==
    TrueList = inCall->TrueList;
    FalseList = inCall->FalseList;
    Register = inCall->Register;
}

// NUM || NUM B || STRING || TRUE || FALSE
Exp::Exp(Node* inObject , string inType)
{

    // Checking if the value is legal for the type
    if (inType == "BYTE") {
        if (stoi(inObject->Value) > 255) {
            output::errorByteTooLarge(yylineno, inObject->Value);
            exit(0);
        }
    }

    // Setting the value & type of the expression
    Value = inObject->Value;
    ExpType = inType;

    // == CODEGEN ==

    // Generating a register and emitting the initilization
    Register = Generator->freshRegister();

    // Initializing an INT number
    if (ExpType == "INT")
        Buffer->emit(Register + " = add i32 0," + Value);

    // Initializing a BYTE number
    else if (ExpType == "BYTE")
        Buffer->emit(Register + " = add i8 0," + Value);

    // Initializing a BOOL value
    else if (ExpType == "BOOL") {
        int Address = Buffer->emit("br label @");
        if (Value == "true")
            TrueList = Buffer->makelist(pair<int, BranchLabelIndex>(Address, FIRST));
        else
            FalseList = Buffer->makelist(pair<int, BranchLabelIndex>(Address, SECOND));
    }

    // Initializing a STRING
    else {
        // Emitting the string to the global buffer
        string GlobalRegister = Register;
        GlobalRegister[0] = '@';
        inObject->Value[inObject->Value.size() - 1] = '\00';
        Buffer->emitGlobal(GlobalRegister + " = constant [" + to_string(inObject->Value.size()) + " x i8] c\"" + inObject->Value +"\"");
        Buffer->emitGlobal("");

        // Emitting the register to the regular buffer
        Buffer->emit(Register + " = getelementptr [" + to_string(inObject->Value.size()) + " x i8], [" + to_string(inObject->Value.size()) + " x i8]* " + GlobalRegister + ", i8 0, i8 0");
    }

}

// ID
Exp::Exp(Node* inID)
{

    // Checking if the ID exists in the ScopeTable
    if(!(MainTable->lookupEntry(inID->Value))){
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }
    
    // Getting the entry from the table
    Entry* EntryInfo = MainTable->getEntry(inID->Value);

    // Making sure the entry is a variable
    if (EntryInfo->FunctionFlag) {
        output::errorUndef(yylineno, inID->Value);
        exit(0);
    }

    // Adjusting the expression data
    Value = EntryInfo->Name;
    ExpType = EntryInfo->Type.back();

    // Checking if the variable is local or passed
    if (EntryInfo->Offset >= 0) {

        // Variable is local - we need to load it from the stack
        Register = Generator->freshRegister();
        string LoadingRegister = Generator->freshRegister();

        // Since we cannot reuse the same register we allocate a temporary one and use it to load from the stack
        Buffer->emit(LoadingRegister + " = getelementptr i32, i32* " + MainTable->getRegister(inID->Value) + ", i32 " + to_string(EntryInfo->Offset));
        Buffer->emit(Register + " = load i32, i32* " + LoadingRegister);

        // Truncating the register in case it was a byte
        Register = (EntryInfo->Type.back() == "BYTE") ? TruncRegister(Register, "i8") : Register;

        // Truncating the register in case it was a bool
        Register = (EntryInfo->Type.back() == "BOOL") ? TruncRegister(Register, "i1") : Register;

    }
    else {

        // Variable is passed - we need to set the register according to its param location
        Register = "%" + to_string(((-1) * EntryInfo->Offset) - 1);

    }

    // Special handling if the value is a bool
    if (ExpType == "BOOL") {

        // Turning the value of the register into a branch
        string Compare = Generator->freshRegister();
        Buffer->emit(Compare + " = icmp ne i1 0, " + Register);

        int Location = Buffer->emit("br i1 " + Compare + ", label @, label @");
        TrueList = Buffer->makelist(pair<int, BranchLabelIndex>(Location, FIRST));
        FalseList = Buffer->makelist(pair<int, BranchLabelIndex>(Location, SECOND));
    }
}

// NOT Exp
Exp::Exp(Node* inNot, Exp *inExp)
{

    // Checking if the expression is a bool
    if (inExp->ExpType != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }

    // Setting the type for the exp
    ExpType = "BOOL";

    // Getting a new register and emitting a line that would switch the boolean value of the given register
    // Register = Generator->freshRegister();
    // Buffer->emit(Register + " = add i1 1, " + inExp->Register);

    // Flipping the true/false lists
    TrueList = BufferList(inExp->FalseList);
    FalseList = BufferList(inExp->TrueList);

}

// LPAREN Type RPAREN Exp
Exp::Exp(Type *inType, Exp *inExp)
{
    
    // Checking if the types are allowed to be casted
    if (inExp->ExpType != inType->Value) {
        if ((inExp->ExpType == "BYTE" || inExp->ExpType == "INT") && (inType->Value == "BYTE" || inType->Value == "INT")) {
                // Casting
                Value = inExp->Value;
                ExpType = inType->Value;
                return;
        }

        output::errorMismatch(yylineno);
        exit(0);
    }

    // Casting
    Value = inExp->Value;
    ExpType = inType->Value;
    Register = inExp->Register;
}

Label::Label()
{
    // Generating a new label, defining it, and emitting it
    Value = Generator->freshLabel();
    Buffer->emit("br label %" + Value);
    Buffer->emit("");
    Buffer->emit(Value + ":");
}

Branch::Branch()
{
    // Generating a new branch and emitting it
    Location = Buffer->emit("br label @");
}


