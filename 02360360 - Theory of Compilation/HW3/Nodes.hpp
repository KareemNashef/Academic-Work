#ifndef NODES
#define NODES

#include <iostream>
#include "SymbolTable.hpp"

#define YYSTYPE Node*

extern int yylineno;

// ===== Helping Functions Declarations =====

bool IDExists(string inID);
void ProgramFinish();
void FunctionFinish();
void OpenScope();
void CloseScope();
void AddLoop();
void RemoveLoop();
bool IntByteCheck(string inA , string inB);

void PrintShit();

// ===== Classes Declarations =====

// ========================================

class Node;
class Program;
class Funcs;
class FuncDecl;
class RetType;
class Formals;
class FormalsList;
class FormalDecl;
class Statements;
class Statement;
class OStatement;
class CStatement;
class NIStatement;
class Call;
class ExpList;
class Type;
class Exp;

// ========================================

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

    FuncDecl(RetType* inRetType, Node* inName, Formals* inFormals);

};

class RetType : public Node {
public:

    RetType(Node* inType) {
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

    Statements(Statement* inState) {};
    Statements(Statements* inStates , Statement* inState) {};

};

class Statement : public Node {
public:

    Statement(OStatement* inState) {};
    Statement(CStatement* inState) {};

};

class OStatement : public Node {
public:

    OStatement(Exp* inExp);                                                                             // IF || WHILE

};

class CStatement : public Node {
public:

    CStatement(Exp* inExp);                                                                             // IF || WHILE

};

class NIStatement : public Node {
public:

    NIStatement(Statements* inStates) {};                                                               // LBRACE Statements RBRACE
    NIStatement(Type* inType , Node* inID);                                                             // Type ID SC
    NIStatement(Type* inType , Node* inID , Exp* inExp);                                                // Type ID ASSIGN Exp SC
    NIStatement(Node* inID , Exp* inEXP);                                                               // ID ASSIGN Exp SC
    NIStatement(Call* inCall) {};                                                                       // Call SC
    NIStatement(Node* inRetType , bool NA);                                                             // RETURN SC 
    NIStatement(Exp* inExp);                                                                            // RETURN Exp SC
    NIStatement(Node* inBrCon);                                                                         // BREAK SC || CONTINUE SC

};

class Call : public Node {
public:

    Call(Node* inID , ExpList* inExpList);                                                              // ID LPAREN ExpList RPAREN
    Call(Node* inID);                                                                                   // ID LPAREN RPAREN

};

class ExpList : public Node {
public:

    vector<string> InnerList;

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

    Exp(Exp* inExp);                                                                                    // LPAREN Exp RPAREN
    Exp(Exp* inExp1 , Exp* inExp2 , Exp* inExp3);                                                       // Exp IF LPAREN Exp RPAREN ELSE Exp
    Exp(Exp* inExp1 , Node* inSign , Exp* inExp2 , string inOP);                                        // Exp AND || OR || RELOP || BINOP Exp
    Exp(Call* inCall);                                                                                  // Call
    Exp(Node* inString , string inType);                                                                // NUM || NUM B || STRING || TRUE || FALSE
    Exp(Node* inID);                                                                                    // ID
    Exp(Node* inNot , Exp* inExp);                                                                      // NOT Exp
    Exp(Type* inType , Exp* inExp);                                                                     // LPAREN Type RPAREN Exp
    
};



#endif