#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static FILE* output_file;
static const char* source_filename;

#define MAX_DECLARED_VARS 1024
static char* declared_vars[MAX_DECLARED_VARS];
static int declared_var_count = 0;

static int is_var_declared(const char* name) {
    for (int i = 0; i < declared_var_count; i++) {
        if (strcmp(declared_vars[i], name) == 0) return 1;
    }
    return 0;
}

static void mark_var_declared(const char* name) {
    if (declared_var_count < MAX_DECLARED_VARS) {
        declared_vars[declared_var_count++] = strdup(name);
    }
}

struct ValueNode* create_int_value(int val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 0;
    node->int_val = val;
    return node;
}

struct ValueNode* create_float_value(double val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 1;
    node->float_val = val;
    return node;
}

struct ValueNode* create_string_value(char* val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 2;
    node->str_val = val;
    return node;
}

struct ValueNode* create_bool_value(int val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 3;
    node->int_val = val;
    return node;
}

struct ValueNode* create_array_value(struct ValueList* elements) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 4;
    node->list_val = elements;
    return node;
}

struct ValueNode* create_identifier_value(char* name) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 7;
    node->str_val = name;
    return node;
}

struct ValueNode* create_index_access_value(char* name, struct ValueNode* index_expr) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 6;
    node->str_val = name;
    node->index_expr = index_expr;
    return node;
}

struct ValueNode* create_object_placeholder() {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 5;
    return node;
}

struct ValueNode* create_binary_op_node(int op, struct ValueNode* left, struct ValueNode* right) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 8;
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

struct ValueNode* create_exec_node(struct ValueNode* cmd_expr) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 9;
    node->index_expr = cmd_expr;
    return node;
}

struct ValueNode* create_len_node(struct ValueNode* expr) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 10;
    node->index_expr = expr;
    return node;
}

struct ValueNode* create_must_node(struct ValueNode* expr) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 11;
    node->index_expr = expr;
    return node;
}

struct ValueNode* create_has_flag_node(struct ValueNode* flag_expr) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 12;
    node->index_expr = flag_expr;
    return node;
}

struct ValueNode* create_get_flag_node(struct ValueNode* flag_expr, struct ValueNode* default_expr) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 13;
    node->left = flag_expr;
    node->right = default_expr;
    return node;
}

struct AssignmentNode* create_assign_node(char* name, struct ValueNode* value, int is_const, int line_no) {
    struct AssignmentNode* node = malloc(sizeof(struct AssignmentNode));
    node->name = name;
    node->value = value;
    node->is_const = is_const;
    node->line_no = line_no;
    return node;
}

struct StringList* create_string_list(char* str, struct StringList* next) {
    struct StringList* node = malloc(sizeof(struct StringList));
    node->str = str;
    node->next = next;
    return node;
}

struct StringList* append_string_list(struct StringList* list, char* str) {
    struct StringList* node = create_string_list(str, NULL);
    if (!list) return node;
    struct StringList* curr = list;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = node;
    return list;
}

struct ValueList* create_value_list(struct ValueNode* val, struct ValueList* next) {
    struct ValueList* node = malloc(sizeof(struct ValueList));
    node->val = val;
    node->next = next;
    return node;
}

struct ValueList* append_value_list(struct ValueList* list, struct ValueNode* val) {
    struct ValueList* node = create_value_list(val, NULL);
    if (!list) return node;
    struct ValueList* curr = list;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = node;
    return list;
}

void free_string_list_nodes(struct StringList* list) {
    struct StringList* curr = list;
    while (curr) {
        struct StringList* temp = curr->next;
        free(curr);
        curr = temp;
    }
}

void free_value_list_nodes(struct ValueList* list) {
    struct ValueList* curr = list;
    while (curr) {
        struct ValueList* temp = curr->next;
        free(curr);
        curr = temp;
    }
}

struct MultiAssignNode* create_multi_assign_node(struct StringList* names, struct ValueList* values, int line_no) {
    struct MultiAssignNode* node = malloc(sizeof(struct MultiAssignNode));
    
    // Count names
    int name_count = 0;
    struct StringList* curr_name = names;
    while (curr_name) {
        name_count++;
        curr_name = curr_name->next;
    }
    
    node->count = name_count;
    node->names = malloc(sizeof(char*) * name_count);
    node->values = malloc(sizeof(struct ValueNode*) * name_count);
    
    struct StringList* cn = names;
    struct ValueList* cv = values;
    for (int i = 0; i < name_count; i++) {
        node->names[i] = cn->str;
        if (cv) {
            node->values[i] = cv->val;
            cv = cv->next;
        } else {
            node->values[i] = create_int_value(0);
        }
        cn = cn->next;
    }
    
    free_string_list_nodes(names);
    free_value_list_nodes(values);
    
    node->line_no = line_no;
    return node;
}

void codegen_init(const char* filename) {
    source_filename = filename;
    output_file = fopen("output.c", "w");
    fprintf(output_file, "#include <stdio.h>\n");
    fprintf(output_file, "#include <stdlib.h>\n");
    fprintf(output_file, "#include <string.h>\n\n");
    
    // Self-referencing Value definition
    fprintf(output_file, "typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_BOOL, TYPE_ARRAY, TYPE_OBJECT } ValueType;\n\n");
    fprintf(output_file, "typedef struct Value Value;\n\n");
    fprintf(output_file, "struct Value {\n");
    fprintf(output_file, "    ValueType type;\n");
    fprintf(output_file, "    union {\n");
    fprintf(output_file, "        int int_val;\n");
    fprintf(output_file, "        double float_val;\n");
    fprintf(output_file, "        char* str_val;\n");
    fprintf(output_file, "        int bool_val;\n");
    fprintf(output_file, "        struct {\n");
    fprintf(output_file, "            Value* elements;\n");
    fprintf(output_file, "            int length;\n");
    fprintf(output_file, "        } array_val;\n");
    fprintf(output_file, "    } data;\n");
    fprintf(output_file, "};\n\n");
    
    // Array safety helper
    fprintf(output_file, "Value get_array_element(Value arr, Value index) {\n");
    fprintf(output_file, "    if (arr.type != TYPE_ARRAY) {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: Attempt to index non-array value\\n\");\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    int idx = 0;\n");
    fprintf(output_file, "    if (index.type == TYPE_INT) {\n");
    fprintf(output_file, "        idx = index.data.int_val;\n");
    fprintf(output_file, "    } else {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: Array index must be an integer\\n\");\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    if (idx < 0 || idx >= arr.data.array_val.length) {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: Index %%d out of bounds (length %%d)\\n\", idx, arr.data.array_val.length);\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return arr.data.array_val.elements[idx];\n");
    fprintf(output_file, "}\n\n");
    
    // Truthiness helper
    fprintf(output_file, "int is_truthy(Value v) {\n");
    fprintf(output_file, "    switch (v.type) {\n");
    fprintf(output_file, "        case TYPE_BOOL: return v.data.bool_val;\n");
    fprintf(output_file, "        case TYPE_INT: return v.data.int_val != 0;\n");
    fprintf(output_file, "        case TYPE_FLOAT: return v.data.float_val != 0.0;\n");
    fprintf(output_file, "        case TYPE_STRING: return v.data.str_val && strlen(v.data.str_val) > 0;\n");
    fprintf(output_file, "        default: return 0;\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "}\n\n");
    
    // Addition helper
    fprintf(output_file, "Value eval_add(Value left, Value right) {\n");
    fprintf(output_file, "    Value res;\n");
    fprintf(output_file, "    if (left.type == TYPE_STRING || right.type == TYPE_STRING) {\n");
    fprintf(output_file, "        res.type = TYPE_STRING;\n");
    fprintf(output_file, "        char l_buf[256] = \"\";\n");
    fprintf(output_file, "        char r_buf[256] = \"\";\n");
    fprintf(output_file, "        if (left.type == TYPE_INT) sprintf(l_buf, \"%%d\", left.data.int_val);\n");
    fprintf(output_file, "        else if (left.type == TYPE_FLOAT) sprintf(l_buf, \"%%f\", left.data.float_val);\n");
    fprintf(output_file, "        else if (left.type == TYPE_STRING) strcpy(l_buf, left.data.str_val);\n");
    fprintf(output_file, "        else if (left.type == TYPE_BOOL) strcpy(l_buf, left.data.bool_val ? \"true\" : \"false\");\n");
    fprintf(output_file, "        if (right.type == TYPE_INT) sprintf(r_buf, \"%%d\", right.data.int_val);\n");
    fprintf(output_file, "        else if (right.type == TYPE_FLOAT) sprintf(r_buf, \"%%f\", right.data.float_val);\n");
    fprintf(output_file, "        else if (right.type == TYPE_STRING) strcpy(r_buf, right.data.str_val);\n");
    fprintf(output_file, "        else if (right.type == TYPE_BOOL) strcpy(r_buf, right.data.bool_val ? \"true\" : \"false\");\n");
    fprintf(output_file, "        char* new_str = malloc(strlen(l_buf) + strlen(r_buf) + 1);\n");
    fprintf(output_file, "        strcpy(new_str, l_buf);\n");
    fprintf(output_file, "        strcat(new_str, r_buf);\n");
    fprintf(output_file, "        res.data.str_val = new_str;\n");
    fprintf(output_file, "    } else {\n");
    fprintf(output_file, "        if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {\n");
    fprintf(output_file, "            res.type = TYPE_FLOAT;\n");
    fprintf(output_file, "            double l = (left.type == TYPE_INT) ? left.data.int_val : left.data.float_val;\n");
    fprintf(output_file, "            double r = (right.type == TYPE_INT) ? right.data.int_val : right.data.float_val;\n");
    fprintf(output_file, "            res.data.float_val = l + r;\n");
    fprintf(output_file, "        } else {\n");
    fprintf(output_file, "            res.type = TYPE_INT;\n");
    fprintf(output_file, "            int l = (left.type == TYPE_BOOL) ? left.data.bool_val : left.data.int_val;\n");
    fprintf(output_file, "            int r = (right.type == TYPE_BOOL) ? right.data.bool_val : right.data.int_val;\n");
    fprintf(output_file, "            res.data.int_val = l + r;\n");
    fprintf(output_file, "        }\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return res;\n");
    fprintf(output_file, "}\n\n");
    
    // Exec helper
    fprintf(output_file, "Value run_exec(Value cmd) {\n");
    fprintf(output_file, "    Value res;\n");
    fprintf(output_file, "    res.type = TYPE_INT;\n");
    fprintf(output_file, "    res.data.int_val = -1;\n");
    fprintf(output_file, "    if (cmd.type == TYPE_STRING) {\n");
    fprintf(output_file, "        res.data.int_val = system(cmd.data.str_val);\n");
    fprintf(output_file, "    } else {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: exec command must be a string\\n\");\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return res;\n");
    fprintf(output_file, "}\n\n");
    
    // Len helper
    fprintf(output_file, "Value eval_len(Value v) {\n");
    fprintf(output_file, "    Value res;\n");
    fprintf(output_file, "    res.type = TYPE_INT;\n");
    fprintf(output_file, "    res.data.int_val = 0;\n");
    fprintf(output_file, "    if (v.type == TYPE_ARRAY) {\n");
    fprintf(output_file, "        res.data.int_val = v.data.array_val.length;\n");
    fprintf(output_file, "    } else if (v.type == TYPE_STRING) {\n");
    fprintf(output_file, "        res.data.int_val = v.data.str_val ? strlen(v.data.str_val) : 0;\n");
    fprintf(output_file, "    } else {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: len() argument must be an array or string\\n\");\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return res;\n");
    fprintf(output_file, "}\n\n");

    // Must helper
    fprintf(output_file, "Value run_must(Value v) {\n");
    fprintf(output_file, "    if (v.type == TYPE_INT && v.data.int_val != 0) {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: must() assertion failed with exit code %%d\\n\", v.data.int_val);\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    } else if (v.type == TYPE_BOOL && v.data.bool_val == 0) {\n");
    fprintf(output_file, "        fprintf(stderr, \"Runtime Error: must() assertion failed (false)\\n\");\n");
    fprintf(output_file, "        exit(1);\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return v;\n");
    fprintf(output_file, "}\n\n");

    // Comparison helper
    fprintf(output_file, "Value eval_binary_op(Value left, int op, Value right) {\n");
    fprintf(output_file, "    Value res;\n");
    fprintf(output_file, "    res.type = TYPE_BOOL;\n");
    fprintf(output_file, "    res.data.bool_val = 0;\n");
    fprintf(output_file, "    double l_num = 0, r_num = 0;\n");
    fprintf(output_file, "    int is_numeric = 0;\n");
    fprintf(output_file, "    if ((left.type == TYPE_INT || left.type == TYPE_FLOAT) && (right.type == TYPE_INT || right.type == TYPE_FLOAT)) {\n");
    fprintf(output_file, "        is_numeric = 1;\n");
    fprintf(output_file, "        l_num = (left.type == TYPE_INT) ? left.data.int_val : left.data.float_val;\n");
    fprintf(output_file, "        r_num = (right.type == TYPE_INT) ? right.data.int_val : right.data.float_val;\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    switch(op) {\n");
    fprintf(output_file, "        case 0: // EQ\n");
    fprintf(output_file, "            if (is_numeric) {\n");
    fprintf(output_file, "                res.data.bool_val = (l_num == r_num);\n");
    fprintf(output_file, "            } else if (left.type == right.type) {\n");
    fprintf(output_file, "                if (left.type == TYPE_STRING) {\n");
    fprintf(output_file, "                    res.data.bool_val = (strcmp(left.data.str_val, right.data.str_val) == 0);\n");
    fprintf(output_file, "                } else if (left.type == TYPE_BOOL) {\n");
    fprintf(output_file, "                    res.data.bool_val = (left.data.bool_val == right.data.bool_val);\n");
    fprintf(output_file, "                } else {\n");
    fprintf(output_file, "                    res.data.bool_val = 0;\n");
    fprintf(output_file, "                }\n");
    fprintf(output_file, "            } else {\n");
    fprintf(output_file, "                res.data.bool_val = 0;\n");
    fprintf(output_file, "            }\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 1: // NE\n");
    fprintf(output_file, "            if (is_numeric) {\n");
    fprintf(output_file, "                res.data.bool_val = (l_num != r_num);\n");
    fprintf(output_file, "            } else if (left.type == right.type) {\n");
    fprintf(output_file, "                if (left.type == TYPE_STRING) {\n");
    fprintf(output_file, "                    res.data.bool_val = (strcmp(left.data.str_val, right.data.str_val) != 0);\n");
    fprintf(output_file, "                } else if (left.type == TYPE_BOOL) {\n");
    fprintf(output_file, "                    res.data.bool_val = (left.data.bool_val != right.data.bool_val);\n");
    fprintf(output_file, "                } else {\n");
    fprintf(output_file, "                    res.data.bool_val = 1;\n");
    fprintf(output_file, "                }\n");
    fprintf(output_file, "            } else {\n");
    fprintf(output_file, "                res.data.bool_val = 1;\n");
    fprintf(output_file, "            }\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 2: // LT\n");
    fprintf(output_file, "            if (is_numeric) res.data.bool_val = (l_num < r_num);\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 3: // GT\n");
    fprintf(output_file, "            if (is_numeric) res.data.bool_val = (l_num > r_num);\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 4: // LE\n");
    fprintf(output_file, "            if (is_numeric) res.data.bool_val = (l_num <= r_num);\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 5: // GE\n");
    fprintf(output_file, "            if (is_numeric) res.data.bool_val = (l_num >= r_num);\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 6: // ADD\n");
    fprintf(output_file, "            return eval_add(left, right);\n");
    fprintf(output_file, "        case 7: // AND\n");
    fprintf(output_file, "            res.data.bool_val = (is_truthy(left) && is_truthy(right));\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 8: // OR\n");
    fprintf(output_file, "            res.data.bool_val = (is_truthy(left) || is_truthy(right));\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 9: // NOT\n");
    fprintf(output_file, "            res.data.bool_val = !is_truthy(left);\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        case 10: // SUB\n");
    fprintf(output_file, "            if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {\n");
    fprintf(output_file, "                res.type = TYPE_FLOAT;\n");
    fprintf(output_file, "                double sl = (left.type == TYPE_INT) ? left.data.int_val : left.data.float_val;\n");
    fprintf(output_file, "                double sr = (right.type == TYPE_INT) ? right.data.int_val : right.data.float_val;\n");
    fprintf(output_file, "                res.data.float_val = sl - sr;\n");
    fprintf(output_file, "            } else {\n");
    fprintf(output_file, "                res.type = TYPE_INT;\n");
    fprintf(output_file, "                res.data.int_val = left.data.int_val - right.data.int_val;\n");
    fprintf(output_file, "            }\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return res;\n");
    fprintf(output_file, "}\n\n");
    
    // Recursive print helper
    fprintf(output_file, "void print_value_rec(Value v, int quoted) {\n");
    fprintf(output_file, "    switch(v.type) {\n");
    fprintf(output_file, "        case TYPE_INT: printf(\"%%d\", v.data.int_val); break;\n");
    fprintf(output_file, "        case TYPE_FLOAT: printf(\"%%f\", v.data.float_val); break;\n");
    fprintf(output_file, "        case TYPE_STRING: {\n");
    fprintf(output_file, "            if (quoted) printf(\"\\\"%%s\\\"\", v.data.str_val);\n");
    fprintf(output_file, "            else printf(\"%%s\", v.data.str_val);\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        }\n");
    fprintf(output_file, "        case TYPE_BOOL: printf(\"%%s\", v.data.bool_val ? \"true\" : \"false\"); break;\n");
    fprintf(output_file, "        case TYPE_ARRAY: {\n");
    fprintf(output_file, "            printf(\"[\");\n");
    fprintf(output_file, "            for (int i = 0; i < v.data.array_val.length; i++) {\n");
    fprintf(output_file, "                print_value_rec(v.data.array_val.elements[i], 1);\n");
    fprintf(output_file, "                if (i < v.data.array_val.length - 1) printf(\", \");\n");
    fprintf(output_file, "            }\n");
    fprintf(output_file, "            printf(\"]\");\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        }\n");
    fprintf(output_file, "        case TYPE_OBJECT: printf(\"[object]\"); break;\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "}\n\n");
    
    fprintf(output_file, "void print_value(const char* name, Value v) {\n");
    fprintf(output_file, "    printf(\"%%s = \", name);\n");
    fprintf(output_file, "    print_value_rec(v, 1);\n");
    fprintf(output_file, "    printf(\"\\n\");\n");
    fprintf(output_file, "}\n\n");
    
    fprintf(output_file, "Value args;\n\n");

    // Flag parsing helpers
    fprintf(output_file, "Value eval_has_flag(Value flag) {\n");
    fprintf(output_file, "    Value res;\n");
    fprintf(output_file, "    res.type = TYPE_BOOL;\n");
    fprintf(output_file, "    res.data.bool_val = 0;\n");
    fprintf(output_file, "    if (flag.type != TYPE_STRING) return res;\n");
    fprintf(output_file, "    for (int i = 0; i < args.data.array_val.length; i++) {\n");
    fprintf(output_file, "        if (args.data.array_val.elements[i].type == TYPE_STRING &&\n");
    fprintf(output_file, "            strcmp(args.data.array_val.elements[i].data.str_val, flag.data.str_val) == 0) {\n");
    fprintf(output_file, "            res.data.bool_val = 1;\n");
    fprintf(output_file, "            break;\n");
    fprintf(output_file, "        }\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return res;\n");
    fprintf(output_file, "}\n\n");
    
    fprintf(output_file, "Value eval_get_flag(Value flag, Value default_val) {\n");
    fprintf(output_file, "    if (flag.type != TYPE_STRING) return default_val;\n");
    fprintf(output_file, "    for (int i = 0; i < args.data.array_val.length - 1; i++) {\n");
    fprintf(output_file, "        if (args.data.array_val.elements[i].type == TYPE_STRING &&\n");
    fprintf(output_file, "            strcmp(args.data.array_val.elements[i].data.str_val, flag.data.str_val) == 0) {\n");
    fprintf(output_file, "            return args.data.array_val.elements[i+1];\n");
    fprintf(output_file, "        }\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "    return default_val;\n");
    fprintf(output_file, "}\n\n");

    // Main function with args initialization
    fprintf(output_file, "int main(int argc, char** argv) {\n");
    fprintf(output_file, "    args.type = TYPE_ARRAY;\n");
    fprintf(output_file, "    args.data.array_val.length = argc - 1;\n");
    fprintf(output_file, "    if (argc > 1) {\n");
    fprintf(output_file, "        args.data.array_val.elements = malloc(sizeof(Value) * (argc - 1));\n");
    fprintf(output_file, "        for (int i = 1; i < argc; i++) {\n");
    fprintf(output_file, "            args.data.array_val.elements[i-1].type = TYPE_STRING;\n");
    fprintf(output_file, "            args.data.array_val.elements[i-1].data.str_val = argv[i];\n");
    fprintf(output_file, "        }\n");
    fprintf(output_file, "    } else {\n");
    fprintf(output_file, "        args.data.array_val.elements = NULL;\n");
    fprintf(output_file, "    }\n\n");
}

void codegen_finish() {
    fprintf(output_file, "    return 0;\n");
    fprintf(output_file, "}\n");
    fclose(output_file);
}

static void codegen_print_expr(struct ValueNode* val) {
    switch (val->type) {
        case 0: // INT
            fprintf(output_file, "(Value){.type = TYPE_INT, .data.int_val = %d}", val->int_val);
            break;
        case 1: // FLOAT
            fprintf(output_file, "(Value){.type = TYPE_FLOAT, .data.float_val = %f}", val->float_val);
            break;
        case 2: // STRING
            fprintf(output_file, "(Value){.type = TYPE_STRING, .data.str_val = \"%s\"}", val->str_val);
            break;
        case 3: // BOOL
            fprintf(output_file, "(Value){.type = TYPE_BOOL, .data.bool_val = %d}", val->int_val);
            break;
        case 4:
            fprintf(output_file, "(Value){.type = TYPE_ARRAY, .data.array_val.length = 0}");
            break;
        case 5:
            fprintf(output_file, "(Value){.type = TYPE_OBJECT}");
            break;
        case 6: // INDEX_ACCESS
            fprintf(output_file, "get_array_element(%s, ", val->str_val);
            codegen_print_expr(val->index_expr);
            fprintf(output_file, ")");
            break;
        case 7: // IDENTIFIER
            fprintf(output_file, "%s", val->str_val);
            break;
        case 8: // BINARY_OP
            if (val->op == 9) {
                fprintf(output_file, "eval_binary_op(");
                codegen_print_expr(val->left);
                fprintf(output_file, ", 9, (Value){0})");
            } else {
                fprintf(output_file, "eval_binary_op(");
                codegen_print_expr(val->left);
                fprintf(output_file, ", %d, ", val->op);
                codegen_print_expr(val->right);
                fprintf(output_file, ")");
            }
            break;
        case 9:
            fprintf(output_file, "run_exec(");
            codegen_print_expr(val->index_expr);
            fprintf(output_file, ")");
            break;
        case 10:
            fprintf(output_file, "eval_len(");
            codegen_print_expr(val->index_expr);
            fprintf(output_file, ")");
            break;
        case 11:
            fprintf(output_file, "run_must(");
            codegen_print_expr(val->index_expr);
            fprintf(output_file, ")");
            break;
        case 12:
            fprintf(output_file, "eval_has_flag(");
            codegen_print_expr(val->index_expr);
            fprintf(output_file, ")");
            break;
        case 13:
            fprintf(output_file, "eval_get_flag(");
            codegen_print_expr(val->left);
            fprintf(output_file, ", ");
            codegen_print_expr(val->right);
            fprintf(output_file, ")");
            break;
    }
}

static void codegen_assign_value(const char* target, struct ValueNode* val) {
    switch (val->type) {
        case 0: // INT
            fprintf(output_file, "%s.type = TYPE_INT;\n", target);
            fprintf(output_file, "%s.data.int_val = %d;\n", target, val->int_val);
            break;
        case 1: // FLOAT
            fprintf(output_file, "%s.type = TYPE_FLOAT;\n", target);
            fprintf(output_file, "%s.data.float_val = %f;\n", target, val->float_val);
            break;
        case 2: // STRING
            fprintf(output_file, "%s.type = TYPE_STRING;\n", target);
            fprintf(output_file, "%s.data.str_val = \"%s\";\n", target, val->str_val);
            break;
        case 3: // BOOL
            fprintf(output_file, "%s.type = TYPE_BOOL;\n", target);
            fprintf(output_file, "%s.data.bool_val = %d;\n", target, val->int_val);
            break;
        case 4: { // ARRAY
            int len = 0;
            struct ValueList* curr = val->list_val;
            while (curr) {
                len++;
                curr = curr->next;
            }
            fprintf(output_file, "%s.type = TYPE_ARRAY;\n", target);
            fprintf(output_file, "%s.data.array_val.length = %d;\n", target, len);
            if (len > 0) {
                fprintf(output_file, "%s.data.array_val.elements = malloc(sizeof(Value) * %d);\n", target, len);
                curr = val->list_val;
                for (int i = 0; i < len; i++) {
                    char elem_target[256];
                    sprintf(elem_target, "%s.data.array_val.elements[%d]", target, i);
                    fprintf(output_file, "    ");
                    codegen_assign_value(elem_target, curr->val);
                    curr = curr->next;
                }
            } else {
                fprintf(output_file, "%s.data.array_val.elements = NULL;\n", target);
            }
            break;
        }
        case 5: // OBJECT
            fprintf(output_file, "%s.type = TYPE_OBJECT;\n", target);
            fprintf(output_file, "// object initialization placeholder\n");
            break;
        case 6: { // INDEX_ACCESS
            fprintf(output_file, "%s = get_array_element(%s, ", target, val->str_val);
            codegen_print_expr(val->index_expr);
            fprintf(output_file, ");\n");
            break;
        }
        case 7: // IDENTIFIER
            fprintf(output_file, "%s = %s;\n", target, val->str_val);
            break;
        case 8: // BINARY_OP
        case 9: // EXEC
        case 10: // LEN
        case 11: // MUST
        case 12: // HAS_FLAG
        case 13: // GET_FLAG
            fprintf(output_file, "%s = ", target);
            codegen_print_expr(val);
            fprintf(output_file, ";\n");
            break;
    }
}

void codegen_assign(struct AssignmentNode* node) {
    fprintf(output_file, "\n#line %d \"%s\"\n", node->line_no, source_filename);
    if (!is_var_declared(node->name)) {
        fprintf(output_file, "    Value %s;\n", node->name);
        mark_var_declared(node->name);
    }
    fprintf(output_file, "    ");
    codegen_assign_value(node->name, node->value);
}

void codegen_const_assign(struct AssignmentNode* node) {
    fprintf(output_file, "\n#line %d \"%s\"\n", node->line_no, source_filename);
    fprintf(output_file, "    const Value %s = {.type = ", node->name);
    mark_var_declared(node->name);

    switch(node->value->type) {
        case 0:
            fprintf(output_file, "TYPE_INT, .data.int_val = %d};\n", node->value->int_val);
            break;
        case 1:
            fprintf(output_file, "TYPE_FLOAT, .data.float_val = %f};\n", node->value->float_val);
            break;
        case 2:
            fprintf(output_file, "TYPE_STRING, .data.str_val = \"%s\"};\n", node->value->str_val);
            break;
        case 3:
            fprintf(output_file, "TYPE_BOOL, .data.bool_val = %d};\n", node->value->int_val);
            break;
    }
}

void codegen_multi_assign(struct MultiAssignNode* node) {
    fprintf(output_file, "\n#line %d \"%s\"\n", node->line_no, source_filename);
    for (int i = 0; i < node->count; i++) {
        if (!is_var_declared(node->names[i])) {
            fprintf(output_file, "    Value %s;\n", node->names[i]);
            mark_var_declared(node->names[i]);
        }
        fprintf(output_file, "    ");
        codegen_assign_value(node->names[i], node->values[i]);
    }
}

void codegen_print(struct ValueList* args, int line_no) {
    fprintf(output_file, "\n#line %d \"%s\"\n", line_no, source_filename);
    struct ValueList* curr = args;
    while (curr) {
        fprintf(output_file, "    {\n");
        fprintf(output_file, "        Value tmp;\n");
        fprintf(output_file, "        ");
        codegen_assign_value("tmp", curr->val);
        fprintf(output_file, "        print_value_rec(tmp, 0);\n");
        fprintf(output_file, "    }\n");
        curr = curr->next;
        if (curr) {
            fprintf(output_file, "    printf(\" \");\n");
        }
    }
    fprintf(output_file, "    printf(\"\\n\");\n");
}

void codegen_expression_statement(struct ValueNode* val, int line_no) {
    fprintf(output_file, "\n#line %d \"%s\"\n", line_no, source_filename);
    fprintf(output_file, "    ");
    codegen_print_expr(val);
    fprintf(output_file, ";\n");
}

void codegen_if_start(struct ValueNode* condition, int line_no) {
    fprintf(output_file, "\n#line %d \"%s\"\n", line_no, source_filename);
    fprintf(output_file, "    if (is_truthy(");
    codegen_print_expr(condition);
    fprintf(output_file, ")) {\n");
}

void codegen_else_if_start(struct ValueNode* condition, int line_no) {
    fprintf(output_file, "\n#line %d \"%s\"\n", line_no, source_filename);
    fprintf(output_file, "    } else if (is_truthy(");
    codegen_print_expr(condition);
    fprintf(output_file, ")) {\n");
}

void codegen_else_start() {
    fprintf(output_file, "    } else {\n");
}

void codegen_if_end() {
    fprintf(output_file, "    }\n");
}
