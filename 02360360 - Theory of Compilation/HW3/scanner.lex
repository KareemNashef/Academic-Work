%{
    #include <stdio.h>
    #include "Nodes.hpp"
    #include "parser.tab.hpp"
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

void                            yylval = new Node(yytext);    return VOID;
int                             yylval = new Node(yytext);    return INT;
byte                            yylval = new Node(yytext);    return BYTE;
b                               yylval = new Node(yytext);    return B;
bool                            yylval = new Node(yytext);    return BOOL;
and                             yylval = new Node(yytext);    return AND;
or                              yylval = new Node(yytext);    return OR;
not                             yylval = new Node(yytext);    return NOT;
true                            yylval = new Node(yytext);    return TRUE;
false                           yylval = new Node(yytext);    return FALSE;
return                          yylval = new Node(yytext);    return RETURN;
if                              yylval = new Node(yytext);    return IF;
else                            yylval = new Node(yytext);    return ELSE;
while                           yylval = new Node(yytext);    return WHILE;
break                           yylval = new Node(yytext);    return BREAK;
continue                        yylval = new Node(yytext);    return CONTINUE;

;                               yylval = new Node(yytext);    return SC;
,                               yylval = new Node(yytext);    return COMMA;
\(                              yylval = new Node(yytext);    return LPAREN;
\)                              yylval = new Node(yytext);    return RPAREN;
\{                              yylval = new Node(yytext);    return LBRACE;
\}                              yylval = new Node(yytext);    return RBRACE;
=                               yylval = new Node(yytext);    return ASSIGN;

"=="                            yylval = new Node(yytext);    return RELOP;
"!="                            yylval = new Node(yytext);    return RELOP;
"<"                             yylval = new Node(yytext);    return RELOP;
">"                             yylval = new Node(yytext);    return RELOP;
"<="                            yylval = new Node(yytext);    return RELOP;
">="                            yylval = new Node(yytext);    return RELOP;
"+"                             yylval = new Node(yytext);    return BINOP;
"-"                             yylval = new Node(yytext);    return BINOP;
"*"                             yylval = new Node(yytext);    return BINOP;
"/"                             yylval = new Node(yytext);    return BINOP;

{Identifier}                    yylval = new Node(yytext);    return ID;
{Number}                        yylval = new Node(yytext);    return NUM;

"\""                            { BEGIN(STRNG); }
<STRNG><<EOF>>                  { output::errorLex(yylineno); exit(0); }
<STRNG>{ApprovedString}(\")     { BEGIN(INITIAL);     yylval = new Node(yytext);     return STRING; }
<STRNG>{NAString}               { output::errorLex(yylineno); exit(0); }

.                               { output::errorLex(yylineno); exit(0); }

%%
