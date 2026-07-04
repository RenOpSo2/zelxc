%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern char* yytext;

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s at '%s'\n", s, yytext);
}
%}

%union {
    int intval;
    double floatval;
    char* strval;
    struct AssignmentNode* assign_node;
    struct ValueNode* value_node;
    struct MultiAssignNode* multi_assign;
    struct StringList* str_list;
    struct ValueList* val_list;
}

%token CONST
%token ASSIGN COMMA COLON
%token LBRACKET RBRACKET LBRACE RBRACE
%token <intval> INT_LIT BOOL_LIT
%token <floatval> FLOAT_LIT
%token <strval> STRING_LIT IDENTIFIER

%type <value_node> value
%type <assign_node> single_assign
%type <multi_assign> multi_assign
%type <str_list> identifier_list
%type <val_list> value_list

%start program

%%

program:
    statement_list
    ;

statement_list:
    statement_list statement
    | /* empty */
    ;

statement:
    single_assign      { codegen_assign($1); }
    | const_assign     { /* handled in const_assign */ }
    | multi_assign     { codegen_multi_assign($1); }
    ;

single_assign:
    IDENTIFIER ASSIGN value    { $$ = create_assign_node($1, $3, 0); }
    ;

const_assign:
    CONST IDENTIFIER ASSIGN value {
        struct AssignmentNode* node = create_assign_node($2, $4, 1);
        codegen_const_assign(node);
    }
    ;

multi_assign:
    identifier_list ASSIGN value_list {
        $$ = create_multi_assign_node($1, $3);
    }
    ;

identifier_list:
    IDENTIFIER COMMA IDENTIFIER {
        $$ = create_string_list($1, NULL);
        $$ = append_string_list($$, $3);
    }
    | identifier_list COMMA IDENTIFIER {
        $$ = append_string_list($1, $3);
    }
    ;

value_list:
    value COMMA value {
        $$ = create_value_list($1, NULL);
        $$ = append_value_list($$, $3);
    }
    | value_list COMMA value {
        $$ = append_value_list($1, $3);
    }
    ;

value:
    INT_LIT       { $$ = create_int_value($1); }
    | FLOAT_LIT   { $$ = create_float_value($1); }
    | STRING_LIT  { $$ = create_string_value($1); }
    | BOOL_LIT    { $$ = create_bool_value($1); }
    | array       { $$ = create_array_placeholder(); }
    | object      { $$ = create_object_placeholder(); }
    ;

array:
    LBRACKET array_elements RBRACKET
    ;

array_elements:
    value
    | array_elements COMMA value
    ;

object:
    LBRACE object_pairs RBRACE
    ;

object_pairs:
    IDENTIFIER COLON value
    | object_pairs COMMA IDENTIFIER COLON value
    ;

%%
