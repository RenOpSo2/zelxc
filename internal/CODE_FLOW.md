# Code Flow and Execution Patterns

## Compiler Execution Flow

### Main Entry Point (src/main.c)
```c
int main(int argc, char** argv) {
    // 1. Validate command-line arguments
    if (argc < 2) {
        printf("Usage: %s <file.zx>\n", argv[0]);
        return 1;
    }
    
    // 2. Open source file
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Cannot open file: %s\n", argv[1]);
        return 1;
    }
    
    // 3. Initialize code generator
    codegen_init(argv[1]);
    
    // 4. Start parsing (triggered by yyparse)
    yyparse();
    
    // 5. Finalize code generation
    codegen_finish();
    
    // 6. Close input file
    fclose(yyin);
    printf("Generated output.c\n");
    return 0;
}
```

## Parsing and Code Generation Flow

### Lexer → Parser → Codegen Pipeline

```
.lex file → Flex → Tokens → Parser.y → Bison → AST → Codegen → C Code
```

### Detailed Flow

1. **Lexer Processing (src/lex.l)**
   - Flex reads source file character by character
   - Pattern matching against regex rules
   - Returns tokens to parser via `yylex()`
   - Sets `yylval` union for token values
   - Tracks line numbers with `yylineno`

2. **Parser Processing (src/parser.y)**
   - Bison receives tokens from lexer
   - Attempts to match tokens against grammar rules
   - Reduces rules using grammar productions
   - Calls semantic actions during reduction
   - Triggers code generation functions

3. **Code Generation (src/codegen.c)**
   - Called from parser semantic actions
   - Generates C code inline during parsing
   - Writes to `output.c` file
   - Injects runtime helpers as needed

## Key Code Patterns

### Pattern 1: AST Node Creation
All AST nodes are created with factory functions:

```c
// Example from codegen.c
struct ValueNode* create_int_value(int val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = 0;  // TYPE_INT
    node->int_val = val;
    return node;
}
```

**Pattern:**
- Allocate memory with `malloc`
- Set type identifier
- Set appropriate value field
- Return node pointer

### Pattern 2: Parser Semantic Actions
Parser rules trigger code generation:

```bison
// Example from parser.y
single_assign:
    IDENTIFIER ASSIGN value { 
        $$ = create_assign_node($1, $3, 0, yylineno); 
    }
    ;

statement:
    single_assign { 
        codegen_assign($1);  // Immediate code generation
    }
    ;
```

**Pattern:**
- Grammar rule with semantic action in braces
- `$$` represents rule result
- `$1`, `$2`, `$3` represent rule components
- Code generation called immediately

### Pattern 3: Code Generation Functions
Code generators write C code directly:

```c
// Example from codegen.c
void codegen_assign(struct AssignmentNode* node) {
    fprintf(output_file, "#line %d \"%s\"\n", node->line_no, source_filename);
    
    // Generate variable declaration if needed
    fprintf(output_file, "Value %s = ", node->name);
    
    // Generate value expression
    generate_value(node->value);
    
    fprintf(output_file, ";\n");
}
```

**Pattern:**
- Emit `#line` directive for debugging
- Generate C code using `fprintf`
- Call helper functions for sub-expressions
- Maintain proper C syntax

### Pattern 4: Value Expression Generation
Recursive descent for expression generation:

```c
void generate_value(struct ValueNode* node) {
    switch(node->type) {
        case 0: // INT
            fprintf(output_file, "(Value){TYPE_INT, .data.int_val = %d}", node->int_val);
            break;
        case 7: // IDENTIFIER
            fprintf(output_file, "%s", node->str_val);
            break;
        case 8: // BINARY_OP
            fprintf(output_file, "eval_binary_op(");
            generate_value(node->left);
            fprintf(output_file, ", %d, ", node->op);
            generate_value(node->right);
            fprintf(output_file, ")");
            break;
        // ... more cases
    }
}
```

**Pattern:**
- Switch on node type
- Recursively handle complex expressions
- Generate runtime function calls for operations
- Maintain type safety

## Control Flow Generation

### If-Else Statement Flow

```bison
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
```

**Code Generation Flow:**
1. `codegen_if_start(condition)`:
   - Generate condition evaluation
   - Emit `if (is_truthy(condition)) {`
   - Set label for else branch

2. Block processing:
   - Generate block statements
   - Emit closing `}`

3. `codegen_else_if_start(condition)`:
   - Emit `} else if (is_truthy(condition)) {`

4. `codegen_else_start()`:
   - Emit `} else {`

5. `codegen_if_end()`:
   - Emit final closing `}`

## Function Call Generation

### Built-in Function Pattern

```bison
value:
    | EXEC LPAREN value RPAREN { 
        $$ = create_exec_node($3); 
    }
    ;
```

**Code Generation:**
```c
case 9: // EXEC
    fprintf(output_file, "run_exec(");
    generate_value(node->index_expr);
    fprintf(output_file, ")");
    break;
```

**Runtime Implementation:**
```c
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
```

## Array Operations Flow

### Array Creation
```bison
array:
    LBRACKET array_elements RBRACKET {
        $$ = $2;
    }
    ;
```

### Array Access
```bison
value:
    | IDENTIFIER LBRACKET value RBRACKET { 
        $$ = create_index_access_value($1, $3); 
    }
    ;
```

**Code Generation:**
```c
case 6: // INDEX_ACCESS
    fprintf(output_file, "get_array_element(%s, ", node->str_val);
    generate_value(node->index_expr);
    fprintf(output_file, ")");
    break;
```

**Runtime Safety:**
```c
Value get_array_element(Value arr, Value index) {
    if (arr.type != TYPE_ARRAY) {
        fprintf(stderr, "Runtime Error: Attempt to index non-array value\n");
        exit(1);
    }
    // ... bounds checking
    return arr.data.array_val.elements[idx];
}
```

## Build Process Flow

### Using build.zx
```
build.zx → zelx build.zx build → Flex → Bison → GCC → zelx binary
```

**Build Steps:**
1. Create build directory
2. Run Bison on `src/parser.y` → `build/parser.tab.c`
3. Run Flex on `src/lex.l` → `build/lex.yy.c`
4. Compile lexer → `build/lex.yy.o`
5. Compile parser → `build/parser.tab.o`
6. Compile codegen → `build/codegen.o`
7. Compile main → `build/main.o`
8. Link all objects → `zelx`

### Using Makefile
```
make → parse Makefile → dependency resolution → parallel compilation → zelx
```

## Test Execution Flow

### Test Runner (run_tests.sh)
```bash
for test_file in test/valid/*.zx; do
    # Compile test with zelx
    ./zelx "$test_file"
    
    # Compile generated C code
    gcc output.c -o test_bin
    
    # Run executable
    ./test_bin
    
    # Check exit code
    if [ $? -eq 0 ]; then
        echo "✓ PASS"
    else
        echo "✗ FAIL"
    fi
done
```

## Error Propagation Flow

### Compile-time Errors
```
Lexer → unrecognized token → error message → continue/exit
Parser → syntax error → yyerror → stderr → exit
```

### Runtime Errors
```
Runtime check → condition failed → fprintf(stderr) → exit(1)
```

**Example:**
```c
if (idx < 0 || idx >= arr.data.array_val.length) {
    fprintf(stderr, "Runtime Error: Index %d out of bounds (length %d)\n", 
            idx, arr.data.array_val.length);
    exit(1);
}
```

## Memory Allocation Flow

### AST Node Lifecycle
```
Parse → malloc(node) → use in codegen → (optional free) → program exit
```

**Pattern:**
- Nodes allocated during parsing
- Used during code generation
- Intentionally not freed (acceptable for compiler)
- All memory reclaimed on process exit

### String Handling
```
Lexer → strdup() → AST node → codegen → C string literal → runtime
```

## Debugging Support

### Line Number Tracking
```c
fprintf(output_file, "#line %d \"%s\"\n", node->line_no, source_filename);
```

**Purpose:**
- Maps generated C code to source `.zx` lines
- Enables proper error messages
- Supports debugging with `gdb`

## Optimization Opportunities

### Current Limitations
- No constant folding
- No dead code elimination
- No register allocation
- Always generates runtime calls

### Potential Optimizations
1. **Constant Folding**: Evaluate constant expressions at compile time
2. **Type Specialization**: Generate type-specific code when types are known
3. **Inline Runtime Functions**: Eliminate function call overhead
4. **Peephole Optimization**: Simplify generated C code patterns

## Extension Points

### Adding New Operators
1. Add token in `lex.l`
2. Add grammar rule in `parser.y`
3. Add node type in `codegen.h`
4. Add creator function in `codegen.c`
5. Add case in `generate_value()`
6. Add runtime implementation if needed

### Adding New Built-in Functions
1. Add keyword in `lex.l`
2. Add grammar rule in `parser.y`
3. Add node type in `codegen.h`
4. Add creator function in `codegen.c`
5. Add case in `generate_value()`
6. Implement runtime function in `codegen_init()`

### Adding New Types
1. Add type enum in `codegen.h`
2. Add to Value union
3. Add creator function
4. Add runtime support functions
5. Update comparison/truthiness logic
6. Add printing support
