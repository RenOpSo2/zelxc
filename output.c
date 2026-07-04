#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_BOOL, TYPE_ARRAY, TYPE_OBJECT } ValueType;

typedef struct Value Value;

struct Value {
    ValueType type;
    union {
        int int_val;
        double float_val;
        char* str_val;
        int bool_val;
        struct {
            Value* elements;
            int length;
        } array_val;
    } data;
};

Value get_array_element(Value arr, Value index) {
    if (arr.type != TYPE_ARRAY) {
        fprintf(stderr, "Runtime Error: Attempt to index non-array value\n");
        exit(1);
    }
    int idx = 0;
    if (index.type == TYPE_INT) {
        idx = index.data.int_val;
    } else {
        fprintf(stderr, "Runtime Error: Array index must be an integer\n");
        exit(1);
    }
    if (idx < 0 || idx >= arr.data.array_val.length) {
        fprintf(stderr, "Runtime Error: Index %d out of bounds (length %d)\n", idx, arr.data.array_val.length);
        exit(1);
    }
    return arr.data.array_val.elements[idx];
}

void print_value_rec(Value v) {
    switch(v.type) {
        case TYPE_INT: printf("%d", v.data.int_val); break;
        case TYPE_FLOAT: printf("%f", v.data.float_val); break;
        case TYPE_STRING: printf("\"%s\"", v.data.str_val); break;
        case TYPE_BOOL: printf("%s", v.data.bool_val ? "true" : "false"); break;
        case TYPE_ARRAY: {
            printf("[");
            for (int i = 0; i < v.data.array_val.length; i++) {
                print_value_rec(v.data.array_val.elements[i]);
                if (i < v.data.array_val.length - 1) printf(", ");
            }
            printf("]");
            break;
        }
        case TYPE_OBJECT: printf("[object]"); break;
    }
}

void print_value(const char* name, Value v) {
    printf("%s = ", name);
    print_value_rec(v);
    printf("\n");
}

int main(int argc, char** argv) {
    Value args;
    args.type = TYPE_ARRAY;
    args.data.array_val.length = argc - 1;
    if (argc > 1) {
        args.data.array_val.elements = malloc(sizeof(Value) * (argc - 1));
        for (int i = 1; i < argc; i++) {
            args.data.array_val.elements[i-1].type = TYPE_STRING;
            args.data.array_val.elements[i-1].data.str_val = argv[i];
        }
    } else {
        args.data.array_val.elements = NULL;
    }

    Value nama;
    nama.type = TYPE_STRING;
nama.data.str_val = "Budi";
    print_value("nama", nama);
    Value umur;
    umur.type = TYPE_INT;
umur.data.int_val = 25;
    print_value("umur", umur);
    Value berat;
    berat.type = TYPE_FLOAT;
berat.data.float_val = 70.500000;
    print_value("berat", berat);
    Value active;
    active.type = TYPE_BOOL;
active.data.bool_val = 1;
    print_value("active", active);
    Value tags;
    tags.type = TYPE_ARRAY;
tags.data.array_val.length = 2;
tags.data.array_val.elements = malloc(sizeof(Value) * 2);
    tags.data.array_val.elements[0].type = TYPE_STRING;
tags.data.array_val.elements[0].data.str_val = "cli";
    tags.data.array_val.elements[1].type = TYPE_STRING;
tags.data.array_val.elements[1].data.str_val = "tools";
    print_value("tags", tags);
    Value config;
    config.type = TYPE_OBJECT;
// object initialization placeholder
    print_value("config", config);
    const Value VERSION = {.type = TYPE_STRING, .data.str_val = "1.0.0"};
    print_value("VERSION", VERSION);
    Value x;
    x.type = TYPE_INT;
x.data.int_val = 10;
    print_value("x", x);
    Value y;
    y.type = TYPE_INT;
y.data.int_val = 20;
    print_value("y", y);
    Value a;
    a.type = TYPE_INT;
a.data.int_val = 1;
    print_value("a", a);
    Value b;
    b.type = TYPE_FLOAT;
b.data.float_val = 2.500000;
    print_value("b", b);
    Value c;
    c.type = TYPE_STRING;
c.data.str_val = "tiga";
    print_value("c", c);
    Value first_tag;
    first_tag = get_array_element(tags, (Value){.type = TYPE_INT, .data.int_val = 0});
    print_value("first_tag", first_tag);
    Value second_tag;
    second_tag = get_array_element(tags, (Value){.type = TYPE_INT, .data.int_val = 1});
    print_value("second_tag", second_tag);
    Value first_arg;
    first_arg = get_array_element(args, (Value){.type = TYPE_INT, .data.int_val = 0});
    print_value("first_arg", first_arg);
    return 0;
}
