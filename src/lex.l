%{
#include <string.h>
#include "parser.tab.h"
%}

%option noyywrap
%option yylineno

%%

"#"([^\n])*           { /* ignore comments */ }
[ \t\n\r]+            { /* ignore whitespace */ }

"const"               { return CONST; }
"print"               { return PRINT; }
"true"                { yylval.intval = 1; return BOOL_LIT; }
"false"               { yylval.intval = 0; return BOOL_LIT; }

"="                   { return ASSIGN; }
","                   { return COMMA; }
":"                   { return COLON; }
"["                   { return LBRACKET; }
"]"                   { return RBRACKET; }
"{"                   { return LBRACE; }
"}"                   { return RBRACE; }
"("                   { return LPAREN; }
")"                   { return RPAREN; }

[0-9]+"."[0-9]+       { yylval.floatval = atof(yytext); return FLOAT_LIT; }
[0-9]+                { yylval.intval = atoi(yytext); return INT_LIT; }

\"[^"]*\"             { 
                        yytext[strlen(yytext)-1] = '\0';
                        yylval.strval = strdup(yytext + 1); 
                        return STRING_LIT; 
                      }

[a-zA-Z_][a-zA-Z0-9_]* { 
                        yylval.strval = strdup(yytext);
                        return IDENTIFIER; 
                      }

.                     { printf("Unknown token: %s\n", yytext); return yytext[0]; }

%%
