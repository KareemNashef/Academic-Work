#ifndef UTILITIES_H
#define UTILITIES_H

#include "hw3_output.hpp"
#include "bp.hpp"
#include <iostream>
#include <algorithm>
extern int yylineno;

typedef vector<pair<int,BranchLabelIndex>> BufferList;

// ===== Helping Classes =====

class OverrideHelper {
public:
    string FuncName;
    vector<string> FuncTypes;
    int Identifier;

    OverrideHelper(string inName, vector<string> inTypes, int inID) :
        FuncName(inName),
        FuncTypes(inTypes),
        Identifier(inID)
        {}
};

// ===== Main Symbol Table Definitions =====

// Single Variable/Function entry
class Entry {
public:
    string Name;                // Entry Name
    vector<string> Type;        // Entry type or return type
    int Offset;                 // Entry offset in the table
    bool FunctionFlag;          // Entry is a function
    bool overridable;           // Entry is overridable

    // Variable constructor
    Entry(string inName, string inType, int inOffset) :
        Name(inName),
        Offset(inOffset),
        FunctionFlag(false),
        overridable(false)
    {
        Type.push_back(inType);
    }

    // Function constructor
    Entry(string inName, vector<string> inType, int inOffset, bool inOFlag) :
        Name(inName),
        Type(inType),
        Offset(inOffset),
        FunctionFlag(true),
        overridable(inOFlag)
    {}

    // Destructor
    ~Entry() = default;
};

// Table for single scope
class SymbolTable {
public:
    vector<Entry*> Data;        // Vector for the entries
    int CurrentOffset;          // The current max offset
    string StackRegister;       // The register that holds the stack array

    // Constructor
    SymbolTable(int inOffset) :
        CurrentOffset(inOffset)
    {}

    // Destructor
    ~SymbolTable() {
        for (Entry* Iter : Data)
            delete(Iter);
    }

    // Entry management
    void addVariable(string inName, string inType);
    void addFunction(string inName, vector<string> inType, bool toOverride);
    void addArgument(string inName, string inType, int inOffset);
    Entry* getEntry(string inName);

};

class Compiler {
public:

    // Main tables
    vector<SymbolTable*> TableStack;
    vector<int> OffsetTable;

    // Constructor
    Compiler();

    // Scope management
    void addScope();
    void removeScope();

    // Entry management
    void addVariable(string inName, string inType);
    void addFunction(string inName, vector<string> inType, bool toOverride);
    void addArguments(vector<string> inNames, vector<string> inTypes);
    bool lookupEntry(string inName);
    bool lookupOverride(string inName);

    // Helping functions
    Entry* getEntry(string inName){
        // Looking for the entry in the table
        for (SymbolTable* CurrentTable : TableStack)
            for (Entry* Iter : CurrentTable->Data)
                if (Iter->Name == inName)
                    return Iter;

        return nullptr;
    }

    string getRegister(string inName){
        // Looking for the entry in the table
        for (SymbolTable* CurrentTable : TableStack)
            for (Entry* Iter : CurrentTable->Data)
                if (Iter->Name == inName)
                    return CurrentTable->StackRegister;

        return nullptr;
    }

    // Debugging
    void printAll();
};

// ===== Code Generation Definitions =====

class CodeGeneration {
    int CurrentRegisters;
    int CurrentLabels;
public:

    // Constructor
    CodeGeneration() :
        CurrentRegisters(0),
        CurrentLabels(0)
    {}

    // Allocations
    string freshRegister();
    string freshLabel();

};

// ===== Nodes Declarations =====

class Node;
class Program;
class Funcs;
class FuncDecl;
class OverRide;
class RetType;
class Formals;
class FormalsList;
class FormalDecl;
class Statements;
class Statement;
class Call;
class ExpList;
class Type;
class Exp;
class Label;
class Branch;

// ===== ================= =====

class Node {
public:

    string Value;

    Node(string inValue) :
        Value(inValue)
    {}
    Node() :
        Value("")
    {}
    virtual ~Node() = default;

};

class Program : public Node {
public:

    Program();

};

class Funcs : public Node {
public:

    Funcs() {};

};

class FuncDecl : public Node {
public:

    vector<string> Types;

    FuncDecl(RetType* inRetType, Node* inName, Formals* inFormals, OverRide* inOverride);

};

class OverRide : public Node {
public:

    bool value;

    OverRide(bool inVal);

};

class RetType : public Node {
public:

    RetType(Node* inType);

};

class Formals : public Node {
public:

    vector<FormalDecl*> DeclsList;

    Formals() {};
    Formals(FormalsList* inList);
    
};

void InsertArgs(Formals* inList);

class FormalsList : public Node {
public:

    vector<FormalDecl*> DeclsList;

    FormalsList(FormalDecl* inDecl);
    FormalsList(FormalDecl* inDecl , FormalsList* inList);

};

class FormalDecl : public Node {
public:

    string DeclType;

    FormalDecl(Type* inType , Node* inID);

};

class Statements : public Node {
public:
    // == CODEGEN ==
    BufferList ContinueList = {};
    BufferList BreakList = {};

    Statements(Statement* inState);
    Statements(Statements* inStates , Statement* inState);

};

class Statement : public Node {
public:
    // == CODEGEN ==
    BufferList ContinueList = {};
    BufferList BreakList = {};

    Statement(Statements* inState) {};
    Statement(Exp* inExp, Label* inTrueLabel, Statement* inStatement);                                                                              // IF
    Statement(Exp* inExp, Label* inTrueLabel, Statement* inTrueStatement, Label* inFalseLabel, Statement* inFalseStatement, Branch* inBranch);      // IF-ELSE
    Statement(Exp* inExp, Label* inTrueLabel, Statement* inTrueStatement, Label* inLoopLabel);                                                      // WHILE
    Statement(Type* inType , Node* inID);                                                                                                           // Type ID SC
    Statement(Type* inType , Node* inID , Exp* inExp);                                                                                              // Type ID ASSIGN Exp SC
    Statement(Node* inID , Exp* inEXP);                                                                                                             // ID ASSIGN Exp SC
    Statement(Call* inCall);                                                                                                                        // Call SC
    Statement(Node* inRetType , bool NA);                                                                                                           // RETURN SC 
    Statement(Exp* inExp);                                                                                                                          // RETURN Exp SC
    Statement(Node* inBrCon);                                                                                                                       // BREAK SC || CONTINUE SC

};

class Call : public Node {
public:
    // == CODEGEN ==
    BufferList TrueList = {};
    BufferList FalseList = {};
    string Register = "";

    Call(Node* inID , ExpList* inExpList);                                                              // ID LPAREN ExpList RPAREN
    Call(Node* inID);                                                                                   // ID LPAREN RPAREN

};

class ExpList : public Node {
public:

    vector<Exp*> InnerList;

    ExpList(Exp* inExp , ExpList* inExpList);                                                           // Exp COMMA ExpList
    ExpList(Exp* inExp);                                                                                // Exp

};

class Type : public Node {
public:

    Type(Node* inType);                                                                                 // INT || BYTE || BOOL

};

class Exp : public Node {
public:

    string ExpType;

    // == CODEGEN ==
    BufferList TrueList = {};
    BufferList FalseList = {};
    string Register = "";

    Exp() {};
    Exp(Exp* inExp);                                                                                    // LPAREN Exp RPAREN
    Exp(Exp* inExp1 , Node* inSign , Exp* inExp2 , string inOP, Label* inLabel);                        // Exp AND || OR || RELOP || BINOP Exp
    Exp(Call* inCall);                                                                                  // Call
    Exp(Node* inObject , string inType);                                                                // NUM || NUM B || STRING || TRUE || FALSE
    Exp(Node* inID);                                                                                    // ID
    Exp(Node* inNot , Exp* inExp);                                                                      // NOT Exp
    Exp(Type* inType , Exp* inExp);                                                                     // LPAREN Type RPAREN Exp
    
};

class Label : public Node {
public:
    Label();
};

class Branch : public Node {
public:
    int Location;
    Branch();
};
// ===== Helping Functions Declarations =====

#define YYSTYPE Node*

void ProgramFinish();
void FunctionFinish();
void InsertArgs(Formals* inList);
bool IntByteCheck(string inA , string inB);
void OpenScope();
void CloseScope();
void AddLoop();
void RemoveLoop();
void CheckBool(Exp *inExp);

string ConvertType(string inType);
string TruncRegister(string inReg, string inType);
string ExtendRegister(string inReg, string inType);
int GetIdentifier(Entry* inEntry);
void BoolJumps(Exp* inExp);

#endif