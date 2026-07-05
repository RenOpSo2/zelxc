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

int is_truthy(Value v) {
    switch (v.type) {
        case TYPE_BOOL: return v.data.bool_val;
        case TYPE_INT: return v.data.int_val != 0;
        case TYPE_FLOAT: return v.data.float_val != 0.0;
        case TYPE_STRING: return v.data.str_val && strlen(v.data.str_val) > 0;
        default: return 0;
    }
}

Value eval_add(Value left, Value right) {
    Value res;
    if (left.type == TYPE_STRING || right.type == TYPE_STRING) {
        res.type = TYPE_STRING;
        char l_buf[256] = "";
        char r_buf[256] = "";
        if (left.type == TYPE_INT) sprintf(l_buf, "%d", left.data.int_val);
        else if (left.type == TYPE_FLOAT) sprintf(l_buf, "%f", left.data.float_val);
        else if (left.type == TYPE_STRING) strcpy(l_buf, left.data.str_val);
        else if (left.type == TYPE_BOOL) strcpy(l_buf, left.data.bool_val ? "true" : "false");
        if (right.type == TYPE_INT) sprintf(r_buf, "%d", right.data.int_val);
        else if (right.type == TYPE_FLOAT) sprintf(r_buf, "%f", right.data.float_val);
        else if (right.type == TYPE_STRING) strcpy(r_buf, right.data.str_val);
        else if (right.type == TYPE_BOOL) strcpy(r_buf, right.data.bool_val ? "true" : "false");
        char* new_str = malloc(strlen(l_buf) + strlen(r_buf) + 1);
        strcpy(new_str, l_buf);
        strcat(new_str, r_buf);
        res.data.str_val = new_str;
    } else {
        if (left.type == TYPE_FLOAT || right.type == TYPE_FLOAT) {
            res.type = TYPE_FLOAT;
            double l = (left.type == TYPE_INT) ? left.data.int_val : left.data.float_val;
            double r = (right.type == TYPE_INT) ? right.data.int_val : right.data.float_val;
            res.data.float_val = l + r;
        } else {
            res.type = TYPE_INT;
            int l = (left.type == TYPE_BOOL) ? left.data.bool_val : left.data.int_val;
            int r = (right.type == TYPE_BOOL) ? right.data.bool_val : right.data.int_val;
            res.data.int_val = l + r;
        }
    }
    return res;
}

Value run_exec(Value cmd) {
    Value res;
    res.type = TYPE_INT;
    res.data.int_val = -1;
    if (cmd.type == TYPE_STRING) {
        res.data.int_val = system(cmd.data.str_val);
    } else {
        fprintf(stderr, "Runtime Error: exec command must be a string\n");
        exit(1);
    }
    return res;
}

Value eval_len(Value v) {
    Value res;
    res.type = TYPE_INT;
    res.data.int_val = 0;
    if (v.type == TYPE_ARRAY) {
        res.data.int_val = v.data.array_val.length;
    } else if (v.type == TYPE_STRING) {
        res.data.int_val = v.data.str_val ? strlen(v.data.str_val) : 0;
    } else {
        fprintf(stderr, "Runtime Error: len() argument must be an array or string\n");
        exit(1);
    }
    return res;
}

Value eval_binary_op(Value left, int op, Value right) {
    Value res;
    res.type = TYPE_BOOL;
    res.data.bool_val = 0;
    double l_num = 0, r_num = 0;
    int is_numeric = 0;
    if ((left.type == TYPE_INT || left.type == TYPE_FLOAT) && (right.type == TYPE_INT || right.type == TYPE_FLOAT)) {
        is_numeric = 1;
        l_num = (left.type == TYPE_INT) ? left.data.int_val : left.data.float_val;
        r_num = (right.type == TYPE_INT) ? right.data.int_val : right.data.float_val;
    }
    switch(op) {
        case 0: // EQ
            if (is_numeric) {
                res.data.bool_val = (l_num == r_num);
            } else if (left.type == right.type) {
                if (left.type == TYPE_STRING) {
                    res.data.bool_val = (strcmp(left.data.str_val, right.data.str_val) == 0);
                } else if (left.type == TYPE_BOOL) {
                    res.data.bool_val = (left.data.bool_val == right.data.bool_val);
                } else {
                    res.data.bool_val = 0;
                }
            } else {
                res.data.bool_val = 0;
            }
            break;
        case 1: // NE
            if (is_numeric) {
                res.data.bool_val = (l_num != r_num);
            } else if (left.type == right.type) {
                if (left.type == TYPE_STRING) {
                    res.data.bool_val = (strcmp(left.data.str_val, right.data.str_val) != 0);
                } else if (left.type == TYPE_BOOL) {
                    res.data.bool_val = (left.data.bool_val != right.data.bool_val);
                } else {
                    res.data.bool_val = 1;
                }
            } else {
                res.data.bool_val = 1;
            }
            break;
        case 2: // LT
            if (is_numeric) res.data.bool_val = (l_num < r_num);
            break;
        case 3: // GT
            if (is_numeric) res.data.bool_val = (l_num > r_num);
            break;
        case 4: // LE
            if (is_numeric) res.data.bool_val = (l_num <= r_num);
            break;
        case 5: // GE
            if (is_numeric) res.data.bool_val = (l_num >= r_num);
            break;
        case 6: // ADD
            return eval_add(left, right);
        case 7: // AND
            res.data.bool_val = (is_truthy(left) && is_truthy(right));
            break;
        case 8: // OR
            res.data.bool_val = (is_truthy(left) || is_truthy(right));
            break;
        case 9: // NOT
            res.data.bool_val = !is_truthy(left);
            break;
    }
    return res;
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


#line 4 "build.zx"
    Value command;
    command.type = TYPE_STRING;
command.data.str_val = "build";
    print_value("command", command);

#line 4 "build.zx"
    if (is_truthy(eval_binary_op(eval_len(args), 3, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 6 "build.zx"
    Value command;
    command = get_array_element(args, (Value){.type = TYPE_INT, .data.int_val = 0});
    print_value("command", command);
    }

#line 8 "build.zx"
    if (is_truthy(eval_binary_op(command, 0, (Value){.type = TYPE_STRING, .data.str_val = "clean"}))) {

#line 9 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "🧹 Cleaning build directory...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 11 "build.zx"
    run_exec((Value){.type = TYPE_STRING, .data.str_val = "rm -f zelx build/*.c build/*.h build/*.o output.c test_bin"});

#line 11 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "✨ Clean complete!";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 12 "build.zx"
    } else if (is_truthy(eval_binary_op(command, 0, (Value){.type = TYPE_STRING, .data.str_val = "build"}))) {

#line 13 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "🚀 Building Zelx Compiler...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 18 "build.zx"
    run_exec((Value){.type = TYPE_STRING, .data.str_val = "mkdir -p build"});

#line 18 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "📦 Running Bison on src/parser.y...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 21 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "bison -d -o build/parser.tab.c src/parser.y"});
    print_value("status", status);

#line 21 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 22 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "📦 Running Flex on src/lex.l...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 25 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "flex -o build/lex.yy.c src/lex.l"});
    print_value("status", status);

#line 25 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 26 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "⚙️ Compiling lexer...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 29 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "gcc -Wall -Wextra -g -Ibuild -Isrc -c build/lex.yy.c -o build/lex.yy.o"});
    print_value("status", status);

#line 29 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 30 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "⚙️ Compiling parser...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 33 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "gcc -Wall -Wextra -g -Ibuild -Isrc -c build/parser.tab.c -o build/parser.tab.o"});
    print_value("status", status);

#line 33 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 34 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "⚙️ Compiling codegen...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 37 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "gcc -Wall -Wextra -g -Ibuild -Isrc -c src/codegen.c -o build/codegen.o"});
    print_value("status", status);

#line 37 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 38 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "⚙️ Compiling main...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 41 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "gcc -Wall -Wextra -g -Ibuild -Isrc -c src/main.c -o build/main.o"});
    print_value("status", status);

#line 41 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 42 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "🔗 Linking objects...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 45 "build.zx"
    Value status;
    status = run_exec((Value){.type = TYPE_STRING, .data.str_val = "gcc -Wall -Wextra -g -o zelx build/lex.yy.o build/parser.tab.o build/codegen.o build/main.o -lfl"});
    print_value("status", status);

#line 45 "build.zx"
    if (is_truthy(eval_binary_op(status, 0, (Value){.type = TYPE_INT, .data.int_val = 0}))) {

#line 46 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "🎉 Zelx built successfully!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    } else {

#line 48 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Linking failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    } else {

#line 51 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Compiling main.c failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    } else {

#line 54 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Compiling codegen.c failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    } else {

#line 57 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Compiling parser failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    } else {

#line 60 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Compiling lexer failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    } else {

#line 63 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Flex failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    } else {

#line 66 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "❌ Bison failed!";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }

#line 68 "build.zx"
    } else if (is_truthy(eval_binary_op(command, 0, (Value){.type = TYPE_STRING, .data.str_val = "test"}))) {

#line 69 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "🧪 Running Zelx test suite...";
        print_value_rec(tmp, 0);
    }
    printf("\n");

#line 71 "build.zx"
    run_exec((Value){.type = TYPE_STRING, .data.str_val = "./run_tests.sh"});
    } else {

#line 72 "build.zx"
    {
        Value tmp;
        tmp.type = TYPE_STRING;
tmp.data.str_val = "Usage: zelx build.zx [build | clean | test]";
        print_value_rec(tmp, 0);
    }
    printf("\n");
    }
    return 0;
}
