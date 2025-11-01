%{
/* Token definitions */
#include "tokens.hpp" 

/* Line counter */
int lineNumber = 1;

/* String indicators */
bool stringStart = false;
bool stringEnd = false;
%}

%option noyywrap 

Whitespace                      ([\r\t ])

Character                       ([a-zA-Z0-9])
HexaChar                        ({Character}{Character})
PrintableChar                   ([\x20-\x7E]|[\x09\x0A\x0D])

Digit                           ([0-9]) 
NonZeroDigit                    ([1-9])
Number                          (([0]|{NonZeroDigit}({Digit})+))

Letter ([a-zA-Z]) 
Identifier ({Letter}({Letter}|{Digit})*)

%x CMNT
%x STRNG

%% 
\n                              lineNumber++;
{Whitespace}                    ;

void                            return VOID;
int                             return INT;
byte                            return BYTE;
b                               return B;
bool                            return BOOL;
and                             return AND;
or                              return OR;
not                             return NOT;
true                            return TRUE;
false                           return FALSE;
return                          return RETURN;
if                              return IF;
else                            return ELSE;
while                           return WHILE;
break                           return BREAK;
continue                        return CONTINUE;

;                               return SC;
,                               return COMMA;
\(                              return LPAREN;
\)                              return RPAREN;
\{                              return LBRACE;
\}                              return RBRACE;
=                               return ASSIGN;

"=="                            return RELOP;
"!="                            return RELOP;
"<"                             return RELOP;
">"                             return RELOP;
"<="                            return RELOP;
">="                            return RELOP;
"+"                             return BINOP;
"-"                             return BINOP;
"*"                             return BINOP;
"/"                             return BINOP;

"//"                            BEGIN(CMNT);
<CMNT>\n                        {BEGIN(INITIAL); lineNumber++; return COMMENT;}
<CMNT>\r                        {BEGIN(INITIAL); lineNumber++; return COMMENT;}
<CMNT>\r\n                      {BEGIN(INITIAL); lineNumber++; return COMMENT;}
<CMNT>.                         ;

{Identifier}                    return ID;
{Number}                        return NUM;

"\""                            {BEGIN(STRNG); stringStart = true; return STRING;}
<STRNG>"\""                     {BEGIN(INITIAL); stringEnd = true; return STRING;}
<STRNG>\\.                      return STRING;
<STRNG>\\x{Character}           return STRING;
<STRNG>\\x{HexaChar}            return STRING;
<STRNG>{PrintableChar}          return STRING;
<STRNG>.                        return INVALIDCHAR;

.                               return INVALIDCHAR;

%%
