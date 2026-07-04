#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static FILE* output_file;

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

struct ValueNode* create_array_placeholder() {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 4;
    return node;
}

struct ValueNode* create_object_placeholder() {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 5;
    return node;
}

struct AssignmentNode* create_assign_node(char* name, struct ValueNode* value, int is_const) {
    struct AssignmentNode* node = malloc(sizeof(struct AssignmentNode));
    node->name = name;
    node->value = value;
    node->is_const = is_const;
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

struct MultiAssignNode* create_multi_assign_node(struct StringList* names, struct ValueList* values) {
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
    
    return node;
}

void codegen_init() {
    output_file = fopen("output.c", "w");
    fprintf(output_file, "#include <stdio.h>\n");
    fprintf(output_file, "#include <stdlib.h>\n");
    fprintf(output_file, "#include <string.h>\n\n");
    fprintf(output_file, "typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_BOOL, TYPE_ARRAY, TYPE_OBJECT } ValueType;\n\n");
    fprintf(output_file, "typedef struct {\n");
    fprintf(output_file, "    ValueType type;\n");
    fprintf(output_file, "    union {\n");
    fprintf(output_file, "        int int_val;\n");
    fprintf(output_file, "        double float_val;\n");
    fprintf(output_file, "        char* str_val;\n");
    fprintf(output_file, "        int bool_val;\n");
    fprintf(output_file, "    } data;\n");
    fprintf(output_file, "} Value;\n\n");
    
    // Helper function
    fprintf(output_file, "void print_value(const char* name, Value v) {\n");
    fprintf(output_file, "    printf(\"%%s = \", name);\n");
    fprintf(output_file, "    switch(v.type) {\n");
    fprintf(output_file, "        case TYPE_INT: printf(\"%%d\\n\", v.data.int_val); break;\n");
    fprintf(output_file, "        case TYPE_FLOAT: printf(\"%%f\\n\", v.data.float_val); break;\n");
    fprintf(output_file, "        case TYPE_STRING: printf(\"%%s\\n\", v.data.str_val); break;\n");
    fprintf(output_file, "        case TYPE_BOOL: printf(\"%%s\\n\", v.data.bool_val ? \"true\" : \"false\"); break;\n");
    fprintf(output_file, "        case TYPE_ARRAY: printf(\"[array]\\n\"); break;\n");
    fprintf(output_file, "        case TYPE_OBJECT: printf(\"[object]\\n\"); break;\n");
    fprintf(output_file, "    }\n");
    fprintf(output_file, "}\n\n");
    
    fprintf(output_file, "int main() {\n");
}

void codegen_finish() {
    fprintf(output_file, "    return 0;\n");
    fprintf(output_file, "}\n");
    fclose(output_file);
}

void codegen_assign(struct AssignmentNode* node) {
    fprintf(output_file, "    Value %s;\n", node->name);
    
    switch(node->value->type) {
        case 0: // INT
            fprintf(output_file, "    %s.type = TYPE_INT;\n", node->name);
            fprintf(output_file, "    %s.data.int_val = %d;\n", node->name, node->value->int_val);
            break;
        case 1: // FLOAT
            fprintf(output_file, "    %s.type = TYPE_FLOAT;\n", node->name);
            fprintf(output_file, "    %s.data.float_val = %f;\n", node->name, node->value->float_val);
            break;
        case 2: // STRING
            fprintf(output_file, "    %s.type = TYPE_STRING;\n", node->name);
            fprintf(output_file, "    %s.data.str_val = \"%s\";\n", node->name, node->value->str_val);
            break;
        case 3: // BOOL
            fprintf(output_file, "    %s.type = TYPE_BOOL;\n", node->name);
            fprintf(output_file, "    %s.data.bool_val = %d;\n", node->name, node->value->int_val);
            break;
        case 4: // ARRAY
            fprintf(output_file, "    %s.type = TYPE_ARRAY;\n", node->name);
            fprintf(output_file, "    // array initialization placeholder\n");
            break;
        case 5: // OBJECT
            fprintf(output_file, "    %s.type = TYPE_OBJECT;\n", node->name);
            fprintf(output_file, "    // object initialization placeholder\n");
            break;
    }
    
    // Debug print
    fprintf(output_file, "    print_value(\"%s\", %s);\n", node->name, node->name);
}

void codegen_const_assign(struct AssignmentNode* node) {
    // Langsung inisialisasi untuk const
    fprintf(output_file, "    const Value %s = {.type = ", node->name);
    
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
    
    // Debug print
    fprintf(output_file, "    print_value(\"%s\", %s);\n", node->name, node->name);
}

void codegen_multi_assign(struct MultiAssignNode* node) {
    for (int i = 0; i < node->count; i++) {
        fprintf(output_file, "    Value %s;\n", node->names[i]);
        
        switch(node->values[i]->type) {
            case 0: // INT
                fprintf(output_file, "    %s.type = TYPE_INT;\n", node->names[i]);
                fprintf(output_file, "    %s.data.int_val = %d;\n", node->names[i], node->values[i]->int_val);
                break;
            case 1: // FLOAT
                fprintf(output_file, "    %s.type = TYPE_FLOAT;\n", node->names[i]);
                fprintf(output_file, "    %s.data.float_val = %f;\n", node->names[i], node->values[i]->float_val);
                break;
            case 2: // STRING
                fprintf(output_file, "    %s.type = TYPE_STRING;\n", node->names[i]);
                fprintf(output_file, "    %s.data.str_val = \"%s\";\n", node->names[i], node->values[i]->str_val);
                break;
            case 3: // BOOL
                fprintf(output_file, "    %s.type = TYPE_BOOL;\n", node->names[i]);
                fprintf(output_file, "    %s.data.bool_val = %d;\n", node->names[i], node->values[i]->int_val);
                break;
            case 4: // ARRAY
                fprintf(output_file, "    %s.type = TYPE_ARRAY;\n", node->names[i]);
                break;
            case 5: // OBJECT
                fprintf(output_file, "    %s.type = TYPE_OBJECT;\n", node->names[i]);
                break;
        }
        fprintf(output_file, "    print_value(\"%s\", %s);\n", node->names[i], node->names[i]);
    }
}
