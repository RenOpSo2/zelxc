%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern char* yytext;
extern int yylineno;

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s at '%s'\n", s, yytext);
}
%}

%expect 2

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

%token CONST PRINT LPAREN RPAREN
%token ASSIGN COMMA COLON
%token LBRACKET RBRACKET LBRACE RBRACE
%token EQ NE LE GE LT GT IF ELSE PLUS EXEC AND OR NOT LEN
%token <intval> INT_LIT BOOL_LIT
%token <floatval> FLOAT_LIT
%token <strval> STRING_LIT IDENTIFIER

%type <value_node> value
%type <assign_node> single_assign
%type <multi_assign> multi_assign
%type <str_list> identifier_list
%type <val_list> value_list array array_elements print_args

%left OR
%left AND
%left EQ NE LT GT LE GE
%left PLUS
%right NOT

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
    | PRINT LPAREN print_args RPAREN { codegen_print($3, yylineno); }
    | if_statement
    | value            { codegen_expression_statement($1, yylineno); }
    ;

if_statement:
    if_header block elif_list else_clause_opt {
        codegen_if_end();
    }
    ;

if_header:
    IF value {
        codegen_if_start($2, yylineno);
    }
    ;

elif_list:
    elif_list else_if_header block
    | /* empty */
    ;

else_if_header:
    ELSE IF value {
        codegen_else_if_start($3, yylineno);
    }
    ;

else_clause_opt:
    else_header block
    | /* empty */
    ;

else_header:
    ELSE {
        codegen_else_start();
    }
    ;

block:
    LBRACE statement_list RBRACE
    ;

single_assign:
    IDENTIFIER ASSIGN value    { $$ = create_assign_node($1, $3, 0, yylineno); }
    ;

const_assign:
    CONST IDENTIFIER ASSIGN value {
        struct AssignmentNode* node = create_assign_node($2, $4, 1, yylineno);
        codegen_const_assign(node);
    }
    ;

multi_assign:
    identifier_list ASSIGN value_list {
        $$ = create_multi_assign_node($1, $3, yylineno);
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

print_args:
    value {
        $$ = create_value_list($1, NULL);
    }
    | print_args COMMA value {
        $$ = append_value_list($1, $3);
    }
    ;

value:
    INT_LIT       { $$ = create_int_value($1); }
    | FLOAT_LIT   { $$ = create_float_value($1); }
    | STRING_LIT  { $$ = create_string_value($1); }
    | BOOL_LIT    { $$ = create_bool_value($1); }
    | IDENTIFIER  { $$ = create_identifier_value($1); }
    | array       { $$ = create_array_value($1); }
    | object      { $$ = create_object_placeholder(); }
    | IDENTIFIER LBRACKET value RBRACKET { $$ = create_index_access_value($1, $3); }
    | value EQ value { $$ = create_binary_op_node(0, $1, $3); }
    | value NE value { $$ = create_binary_op_node(1, $1, $3); }
    | value LT value { $$ = create_binary_op_node(2, $1, $3); }
    | value GT value { $$ = create_binary_op_node(3, $1, $3); }
    | value LE value { $$ = create_binary_op_node(4, $1, $3); }
    | value GE value { $$ = create_binary_op_node(5, $1, $3); }
    | value PLUS value { $$ = create_binary_op_node(6, $1, $3); }
    | value AND value { $$ = create_binary_op_node(7, $1, $3); }
    | value OR value { $$ = create_binary_op_node(8, $1, $3); }
    | NOT value { $$ = create_binary_op_node(9, $2, NULL); }
    | EXEC LPAREN value RPAREN { $$ = create_exec_node($3); }
    | LEN LPAREN value RPAREN { $$ = create_len_node($3); }
    | LPAREN value RPAREN { $$ = $2; }
    ;

array:
    LBRACKET array_elements RBRACKET {
        $$ = $2;
    }
    ;

array_elements:
    value {
        $$ = create_value_list($1, NULL);
    }
    | array_elements COMMA value {
        $$ = append_value_list($1, $3);
    }
    ;

object:
    LBRACE object_pairs RBRACE
    ;

object_pairs:
    IDENTIFIER COLON value
    | object_pairs COMMA IDENTIFIER COLON value
    ;

%%
