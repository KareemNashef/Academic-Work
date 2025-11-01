#ifndef TOKENS_HPP_
#define TOKENS_HPP_

#include <iostream>
#include <cstdlib>
#include <vector>

enum tokentype {
  VOID,
  INT,
  BYTE,
  B,
  BOOL,
  AND,
  OR,
  NOT,
  TRUE,
  FALSE,
  RETURN,
  IF,
  ELSE,
  WHILE,
  BREAK,
  CONTINUE,
  SC,
  COMMA,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  ASSIGN,
  RELOP,
  BINOP,
  COMMENT,
  ID,
  NUM,
  STRING,
  INVALIDCHAR
};

extern int lineNumber;
extern bool stringStart;
extern bool stringEnd;
extern char* yytext;
extern int yyleng;
extern int yylex();

void InvalidCharacter(char* invalid);
void UnclosedString();
void BadSequence(char* invalid);

#endif /* TOKENS_HPP_ */