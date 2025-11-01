%{
    #include <stdio.h>
    #define YYSTYPE int
    #include "parser.tab.hpp"
    #include "output.hpp"
%}

%option yylineno
%option noyywrap 

Whitespace                      ([\r\t\n ])

Identifier ([a-zA-Z][a-zA-Z0-9]*)

Digit                           ([0-9]) 
Number                          (({Digit}({Digit}|0)*)|0)

ApprovedString (([\x00-\x09\x0b-\x0c\x0e-\x21\x23-\x5b\x5d-\x7f]|((\\)(\\))|((\\)(\"))|((\\)(n))|((\\)(r)))*)
NAString ([^(\")])*((\")?)

%x STRNG

%% 

{Whitespace}                    ;
\/\/[^\r\n]*[\r|\n|\r\n]?       ;

void                            return VOID;
int                             return INT;
byte                            return BYTE;
b                               return B;
bool                            return BOOL;
override                        return OVERRIDE;
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

{Identifier}                    return ID;
{Number}                        return NUM;

"\""                            { BEGIN(STRNG); }
<STRNG><<EOF>>                  { output::errorLex(yylineno); exit(0); }
<STRNG>{ApprovedString}(\")     { BEGIN(INITIAL); return STRING; }
<STRNG>{NAString}               { output::errorLex(yylineno); exit(0); }

.                               { output::errorLex(yylineno); exit(0); }

%%
