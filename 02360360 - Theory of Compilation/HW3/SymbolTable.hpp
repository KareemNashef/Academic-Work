#ifndef SYMBOLTABLE
#define SYMBOLTABLE

#include "hw3_output.hpp"
#define MAXSIZE 100

class Entry {
public:

    string EntryName;
    vector<string> EntryType;
    int EntryOffset;
    bool EntryFunc;

    // For Variables
    Entry(string inName, string inType, int inOffset) :
        EntryName(inName),
        EntryOffset(inOffset)
    {
        EntryType.push_back(inType);
        EntryFunc = false;
    }

    // For Functions
    Entry(string inName, vector<string> inType, int inOffset) :
        EntryName(inName),
        EntryType(inType),
        EntryOffset(inOffset),
        EntryFunc(true)
    {}

    ~Entry() = default;
};

class SymbolTable {
public:

    vector<Entry*> Table;

    SymbolTable() = default;
    ~SymbolTable() = default;

    bool CheckID(string inID) {

        for (Entry* Iter : Table) {
            
            if(Iter->EntryName == inID) {
                return true;
            }
                
        }
            

        return false;
    }

};

#endif