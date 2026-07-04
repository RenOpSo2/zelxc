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

void print_value_rec(Value v, int quoted) {
    switch(v.type) {
        case TYPE_INT: printf("%d", v.data.int_val); break;
        case TYPE_FLOAT: printf("%f", v.data.float_val); break;
        case TYPE_STRING: {
            if (quoted) printf("\"%s\"", v.data.str_val);
            else printf("%s", v.data.str_val);
            break;
        }
        case TYPE_BOOL: printf("%s", v.data.bool_val ? "true" : "false"); break;
        case TYPE_ARRAY: {
            printf("[");
            for (int i = 0; i < v.data.array_val.length; i++) {
                print_value_rec(v.data.array_val.elements[i], 1);
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
    print_value_rec(v, 1);
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

    const Value APP_NAME = {.type = TYPE_STRING, .data.str_val = "zelx"};
    print_value("APP_NAME", APP_NAME);
    const Value VERSION = {.type = TYPE_STRING, .data.str_val = "0.1.0"};
    print_value("VERSION", VERSION);
    const Value HELP_TEXT = {.type = TYPE_STRING, .data.str_val = "Usage: zelx <command> [options]"};
    print_value("HELP_TEXT", HELP_TEXT);
    Value command;
    command = get_array_element(args, (Value){.type = TYPE_INT, .data.int_val = 0});
    print_value("command", command);
    Value opt1;
    opt1 = get_array_element(args, (Value){.type = TYPE_INT, .data.int_val = 1});
    print_value("opt1", opt1);
    Value opt2;
    opt2 = get_array_element(args, (Value){.type = TYPE_INT, .data.int_val = 2});
    print_value("opt2", opt2);
    Value opt3;
    opt3 = get_array_element(args, (Value){.type = TYPE_INT, .data.int_val = 3});
    print_value("opt3", opt3);
    Value flags_short;
    flags_short.type = TYPE_ARRAY;
flags_short.data.array_val.length = 5;
flags_short.data.array_val.elements = malloc(sizeof(Value) * 5);
    flags_short.data.array_val.elements[0].type = TYPE_STRING;
flags_short.data.array_val.elements[0].data.str_val = "-h";
    flags_short.data.array_val.elements[1].type = TYPE_STRING;
flags_short.data.array_val.elements[1].data.str_val = "-v";
    flags_short.data.array_val.elements[2].type = TYPE_STRING;
flags_short.data.array_val.elements[2].data.str_val = "-n";
    flags_short.data.array_val.elements[3].type = TYPE_STRING;
flags_short.data.array_val.elements[3].data.str_val = "-o";
    flags_short.data.array_val.elements[4].type = TYPE_STRING;
flags_short.data.array_val.elements[4].data.str_val = "-m";
    print_value("flags_short", flags_short);
    Value flags_long;
    flags_long.type = TYPE_ARRAY;
flags_long.data.array_val.length = 5;
flags_long.data.array_val.elements = malloc(sizeof(Value) * 5);
    flags_long.data.array_val.elements[0].type = TYPE_STRING;
flags_long.data.array_val.elements[0].data.str_val = "--help";
    flags_long.data.array_val.elements[1].type = TYPE_STRING;
flags_long.data.array_val.elements[1].data.str_val = "--version";
    flags_long.data.array_val.elements[2].type = TYPE_STRING;
flags_long.data.array_val.elements[2].data.str_val = "--name";
    flags_long.data.array_val.elements[3].type = TYPE_STRING;
flags_long.data.array_val.elements[3].data.str_val = "--output";
    flags_long.data.array_val.elements[4].type = TYPE_STRING;
flags_long.data.array_val.elements[4].data.str_val = "--mode";
    print_value("flags_long", flags_long);
    Value flags_desc;
    flags_desc.type = TYPE_ARRAY;
flags_desc.data.array_val.length = 5;
flags_desc.data.array_val.elements = malloc(sizeof(Value) * 5);
    flags_desc.data.array_val.elements[0].type = TYPE_STRING;
flags_desc.data.array_val.elements[0].data.str_val = "Show help";
    flags_desc.data.array_val.elements[1].type = TYPE_STRING;
flags_desc.data.array_val.elements[1].data.str_val = "Show version";
    flags_desc.data.array_val.elements[2].type = TYPE_STRING;
flags_desc.data.array_val.elements[2].data.str_val = "Set name";
    flags_desc.data.array_val.elements[3].type = TYPE_STRING;
flags_desc.data.array_val.elements[3].data.str_val = "Set output file";
    flags_desc.data.array_val.elements[4].type = TYPE_STRING;
flags_desc.data.array_val.elements[4].data.str_val = "Set mode";
    print_value("flags_desc", flags_desc);
    Value cmds_list;
    cmds_list.type = TYPE_ARRAY;
cmds_list.data.array_val.length = 4;
cmds_list.data.array_val.elements = malloc(sizeof(Value) * 4);
    cmds_list.data.array_val.elements[0].type = TYPE_STRING;
cmds_list.data.array_val.elements[0].data.str_val = "build";
    cmds_list.data.array_val.elements[1].type = TYPE_STRING;
cmds_list.data.array_val.elements[1].data.str_val = "run";
    cmds_list.data.array_val.elements[2].type = TYPE_STRING;
cmds_list.data.array_val.elements[2].data.str_val = "init";
    cmds_list.data.array_val.elements[3].type = TYPE_STRING;
cmds_list.data.array_val.elements[3].data.str_val = "test";
    print_value("cmds_list", cmds_list);
    Value cmds_desc;
    cmds_desc.type = TYPE_ARRAY;
cmds_desc.data.array_val.length = 4;
cmds_desc.data.array_val.elements = malloc(sizeof(Value) * 4);
    cmds_desc.data.array_val.elements[0].type = TYPE_STRING;
cmds_desc.data.array_val.elements[0].data.str_val = "Build project";
    cmds_desc.data.array_val.elements[1].type = TYPE_STRING;
cmds_desc.data.array_val.elements[1].data.str_val = "Run project";
    cmds_desc.data.array_val.elements[2].type = TYPE_STRING;
cmds_desc.data.array_val.elements[2].data.str_val = "Init new project";
    cmds_desc.data.array_val.elements[3].type = TYPE_STRING;
cmds_desc.data.array_val.elements[3].data.str_val = "Run tests";
    print_value("cmds_desc", cmds_desc);
    Value show_help;
    show_help = command;
    print_value("show_help", show_help);
    Value show_version;
    show_version = command;
    print_value("show_version", show_version);
    Value name_value;
    name_value = opt1;
    print_value("name_value", name_value);
    Value output_value;
    output_value = opt2;
    print_value("output_value", output_value);
    Value mode_value;
    mode_value = opt3;
    print_value("mode_value", mode_value);
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "===";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = APP_NAME;
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "v";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = VERSION;
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "===";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Command:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = command;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    Value is_help;
    is_help = command;
    print_value("is_help", is_help);
    Value is_version;
    is_version = command;
    print_value("is_version", is_version);
    Value is_build;
    is_build = command;
    print_value("is_build", is_build);
    Value is_run;
    is_run = command;
    print_value("is_run", is_run);
    Value is_init;
    is_init = command;
    print_value("is_init", is_init);
    Value is_test;
    is_test = command;
    print_value("is_test", is_test);
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "--- Parsed ---";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Help mode:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = is_help;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Version request:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = is_version;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Build command:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = is_build;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Run command:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = is_run;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Init command:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = is_init;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Test command:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = is_test;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Name:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = name_value;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Output:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = output_value;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Mode:";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = mode_value;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "--- Command matched ---";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    Value matched_cmd;
    matched_cmd = get_array_element(cmds_list, (Value){.type = TYPE_INT, .data.int_val = 0});
    print_value("matched_cmd", matched_cmd);
    Value matched_desc;
    matched_desc = get_array_element(cmds_desc, (Value){.type = TYPE_INT, .data.int_val = 0});
    print_value("matched_desc", matched_desc);
    {
        Value tmp;
        tmp = matched_cmd;
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "->";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = matched_desc;
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "--- Available flags ---";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(flags_short, (Value){.type = TYPE_INT, .data.int_val = 0});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_long, (Value){.type = TYPE_INT, .data.int_val = 0});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_desc, (Value){.type = TYPE_INT, .data.int_val = 0});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(flags_short, (Value){.type = TYPE_INT, .data.int_val = 1});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_long, (Value){.type = TYPE_INT, .data.int_val = 1});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_desc, (Value){.type = TYPE_INT, .data.int_val = 1});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(flags_short, (Value){.type = TYPE_INT, .data.int_val = 2});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_long, (Value){.type = TYPE_INT, .data.int_val = 2});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_desc, (Value){.type = TYPE_INT, .data.int_val = 2});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(flags_short, (Value){.type = TYPE_INT, .data.int_val = 3});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_long, (Value){.type = TYPE_INT, .data.int_val = 3});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_desc, (Value){.type = TYPE_INT, .data.int_val = 3});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(flags_short, (Value){.type = TYPE_INT, .data.int_val = 4});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_long, (Value){.type = TYPE_INT, .data.int_val = 4});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(flags_desc, (Value){.type = TYPE_INT, .data.int_val = 4});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "--- Available commands ---";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(cmds_list, (Value){.type = TYPE_INT, .data.int_val = 0});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(cmds_desc, (Value){.type = TYPE_INT, .data.int_val = 0});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(cmds_list, (Value){.type = TYPE_INT, .data.int_val = 1});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(cmds_desc, (Value){.type = TYPE_INT, .data.int_val = 1});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(cmds_list, (Value){.type = TYPE_INT, .data.int_val = 2});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(cmds_desc, (Value){.type = TYPE_INT, .data.int_val = 2});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    {
        Value tmp;
        tmp = get_array_element(cmds_list, (Value){.type = TYPE_INT, .data.int_val = 3});
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = ":";
        print_value_rec(tmp, 0);
    }
    printf(" ");
    {
        Value tmp;
        tmp = get_array_element(cmds_desc, (Value){.type = TYPE_INT, .data.int_val = 3});
        print_value_rec(tmp, 0);
    }
    printf("\n");
    return 0;
}
