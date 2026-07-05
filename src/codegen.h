#ifndef CODEGEN_H
#define CODEGEN_H

// Forward declarations
struct ValueNode;
struct AssignmentNode;
struct MultiAssignNode;

struct ValueNode {
    int type; // 0:int, 1:float, 2:string, 3:bool, 4:array, 5:object, 6:index_access, 7:identifier, 8:binary_op, 9:exec
    int int_val;
    double float_val;
    char* str_val;
    struct ValueList* list_val;
    struct ValueNode* index_expr;
    int op;
    struct ValueNode* left;
    struct ValueNode* right;
};

struct AssignmentNode {
    char* name;
    struct ValueNode* value;
    int is_const;
    int line_no;
};

struct MultiAssignNode {
    char** names;
    struct ValueNode** values;
    int count;
    int line_no;
};

struct StringList {
    char* str;
    struct StringList* next;
};

struct ValueList {
    struct ValueNode* val;
    struct ValueList* next;
};

struct ValueNode* create_int_value(int val);
struct ValueNode* create_float_value(double val);
struct ValueNode* create_string_value(char* val);
struct ValueNode* create_bool_value(int val);
struct ValueNode* create_array_value(struct ValueList* elements);
struct ValueNode* create_identifier_value(char* name);
struct ValueNode* create_index_access_value(char* name, struct ValueNode* index_expr);
struct ValueNode* create_object_placeholder();
struct ValueNode* create_binary_op_node(int op, struct ValueNode* left, struct ValueNode* right);
struct ValueNode* create_exec_node(struct ValueNode* cmd_expr);

struct StringList* create_string_list(char* str, struct StringList* next);
struct StringList* append_string_list(struct StringList* list, char* str);
struct ValueList* create_value_list(struct ValueNode* val, struct ValueList* next);
struct ValueList* append_value_list(struct ValueList* list, struct ValueNode* val);
void free_string_list_nodes(struct StringList* list);
void free_value_list_nodes(struct ValueList* list);

struct AssignmentNode* create_assign_node(char* name, struct ValueNode* value, int is_const, int line_no);
struct MultiAssignNode* create_multi_assign_node(struct StringList* names, struct ValueList* values, int line_no);

void codegen_init(const char* filename);
void codegen_finish();
void codegen_assign(struct AssignmentNode* node);
void codegen_const_assign(struct AssignmentNode* node);
void codegen_multi_assign(struct MultiAssignNode* node);
void codegen_print(struct ValueList* args, int line_no);
void codegen_if_start(struct ValueNode* condition, int line_no);
void codegen_else_if_start(struct ValueNode* condition, int line_no);
void codegen_else_start();
void codegen_if_end();

#endif

