# Developer Guide

## Getting Started

### Prerequisites
- GCC compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make (optional, for alternative build system)

### Building the Compiler

#### Method 1: Using build.zx (Recommended)
```bash
# Build the compiler
./zelx build.zx build

# Clean build artifacts
./zelx build.zx clean

# Run tests
./zelx build.zx test
```

#### Method 2: Using Makefile
```bash
# Build
make

# Clean
make clean

# Test
make test
```

### Manual Build Steps
```bash
# Create build directory
mkdir -p build

# Generate parser
bison -d -o build/parser.tab.c src/parser.y

# Generate lexer
flex -o build/lex.yy.c src/lex.l

# Compile
gcc -Wall -Wextra -g -Ibuild -Isrc -c build/lex.yy.c -o build/lex.yy.o
gcc -Wall -Wextra -g -Ibuild -Isrc -c build/parser.tab.c -o build/parser.tab.o
gcc -Wall -Wextra -g -Ibuild -Isrc -c src/codegen.c -o build/codegen.o
gcc -Wall -Wextra -g -Ibuild -Isrc -c src/main.c -o build/main.o

# Link
gcc -Wall -Wextra -g -o zelx build/lex.yy.o build/parser.tab.o build/codegen.o build/main.o -lfl
```

## Code Organization

### Source Files Overview

#### src/main.c
- Entry point for the compiler
- Command-line argument handling
- File I/O coordination
- Pipeline orchestration

**Key Functions:**
- `main()`: Orchestrates compilation pipeline

#### src/lex.l
- Flex lexer specification
- Token definitions and patterns
- Character-level processing

**Key Patterns:**
- Keywords: `const`, `print`, `if`, `else`, etc.
- Operators: `+`, `==`, `!=`, etc.
- Literals: integers, floats, strings, booleans
- Identifiers: variable names

**Adding New Tokens:**
```lex
"new_keyword"    { return NEW_KEYWORD; }
```

#### src/parser.y
- Bison grammar specification
- AST construction rules
- Semantic actions for code generation

**Key Sections:**
- `%union`: AST node types
- `%token`: Token declarations
- `%type`: Non-terminal types
- `%%`: Grammar rules
- Grammar rules with semantic actions

**Adding New Grammar Rules:**
```bison
expression:
    expression NEW_OP expression {
        $$ = create_new_op_node($1, $3);
    }
    ;
```

#### src/codegen.h
- AST structure definitions
- Function declarations
- Type definitions

**Key Structures:**
- `ValueNode`: Expression and value representation
- `AssignmentNode`: Variable assignment
- `MultiAssignNode`: Multiple variable assignment
- `StringList`: String linked list
- `ValueList`: Value linked list

#### src/codegen.c
- Code generation implementation
- Runtime helper functions
- AST node factories

**Key Functions:**
- `codegen_init()`: Initialize output file and runtime
- `codegen_finish()`: Finalize output file
- `codegen_assign()`: Generate assignment code
- `generate_value()`: Generate expression code
- Runtime helpers: `eval_add()`, `is_truthy()`, etc.

## Development Workflow

### Adding a New Language Feature

#### Example: Adding a Modulo Operator

**Step 1: Update Lexer (src/lex.l)**
```lex
"%"                   { return MODULO; }
```

**Step 2: Update Parser (src/parser.y)**
```bison
%token MODULO

%left PLUS MODULO

value:
    | value MODULO value { $$ = create_binary_op_node(10, $1, $3); }
    ;
```

**Step 3: Update Header (src/codegen.h)**
```c
struct ValueNode* create_binary_op_node(int op, struct ValueNode* left, struct ValueNode* right);
```

**Step 4: Update Code Generator (src/codegen.c)**
```c
// In generate_value():
case 10: // MODULO
    fprintf(output_file, "eval_modulo(");
    generate_value(node->left);
    fprintf(output_file, ", ");
    generate_value(node->right);
    fprintf(output_file, ")");
    break;
```

**Step 5: Add Runtime Helper (src/codegen.c)**
```c
Value eval_modulo(Value left, Value right) {
    Value res;
    res.type = TYPE_INT;
    if (left.type == TYPE_INT && right.type == TYPE_INT) {
        res.data.int_val = left.data.int_val % right.data.int_val;
    } else {
        fprintf(stderr, "Runtime Error: Modulo requires integers\n");
        exit(1);
    }
    return res;
}
```

**Step 6: Add to codegen_init()**
```c
fprintf(output_file, "Value eval_modulo(Value left, Value right) {\n");
// ... implementation
fprintf(output_file, "}\n\n");
```

**Step 7: Test**
Create `test/valid/10_modulo.zx`:
```zx
result = 10 % 3
print(result)  # Should print 1
```

### Debugging Techniques

#### Compiler Debugging
```bash
# Debug with verbose output
bison -v src/parser.y  # Generates parser.output

# Check generated parser
cat build/parser.output

# Debug generated C code
cat output.c
```

#### Runtime Debugging
```bash
# Compile with debug symbols
gcc -g output.c -o test_bin

# Run with GDB
gdb ./test_bin

# In GDB:
(gdb) break main
(gdb) run
(gdb) step
(gdb) print variable_name
```

#### Lexer Debugging
```bash
# Test lexer in isolation
flex --debug src/lex.l
gcc lex.yy.c -lfl -o lexer
./lexer < test.zx
```

## Testing

### Running Tests
```bash
# Run all tests
./run_tests.sh

# Run specific test
./zelx test/valid/01_basic.zx
gcc output.c -o test_bin
./test_bin
```

### Writing Tests

#### Test File Structure
```zx
# test/valid/feature_name.zx
# Description: Test feature description

# Test case 1
variable = value
print(variable)

# Test case 2
# ...
```

#### Test Categories
- `test/valid/`: Tests that should pass
- Future: `test/invalid/`: Tests that should fail with specific errors

### Test Coverage Goals
- All language features
- Edge cases (empty arrays, boundary values)
- Error conditions
- Type combinations

## Code Style Guidelines

### C Code Style
- Use 4-space indentation
- Opening braces on same line
- Space after keywords (`if (`, `while (`)
- No trailing whitespace
- Descriptive variable names

### Bison/Flex Style
- Keep rules simple and readable
- Use meaningful token names
- Comment complex grammar rules
- Maintain precedence declarations

### Documentation
- Document non-obvious functions
- Explain complex algorithms
- Provide usage examples in comments
- Update this guide for architectural changes

## Common Patterns

### AST Node Creation
```c
struct ValueNode* create_xxx_value(type val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    node->type = TYPE_XXX;
    node->xxx_val = val;
    return node;
}
```

### Code Generation
```c
void codegen_xxx(struct XXXNode* node) {
    fprintf(output_file, "#line %d \"%s\"\n", node->line_no, source_filename);
    // Generate C code
    fprintf(output_file, "/* generated code */;\n");
}
```

### Error Handling
```c
if (error_condition) {
    fprintf(stderr, "Runtime Error: descriptive message\n");
    exit(1);
}
```

## Performance Considerations

### Memory Management
- AST nodes are intentionally not freed (acceptable for compiler)
- Use `strdup()` for strings to avoid modification
- Be careful with circular references

### Code Generation
- Prefer simple, straightforward code
- Avoid premature optimization
- Generated code readability matters for debugging

### Runtime Performance
- Type checking has overhead
- Array bounds checking is necessary
- String operations can be expensive

## Troubleshooting

### Build Issues

**Problem: Bison version conflict**
```bash
# Check version
bison --version

# Update grammar syntax if needed
# Modern Bison uses %expect instead of %expect
```

**Problem: Flex cannot find parser header**
```bash
# Ensure parser is generated first
bison -d -o build/parser.tab.c src/parser.y
flex -o build/lex.yy.c src/lex.l
```

### Runtime Issues

**Problem: Segmentation fault**
```bash
# Compile with debug symbols
gcc -g output.c -o test_bin

# Run with GDB
gdb ./test_bin
(gdb) run
(gdb) backtrace
```

**Problem: Array index out of bounds**
```bash
# Check array length before access
# Use len() function
length = len(my_array)
if index < length {
    value = my_array[index]
}
```

### Parser Issues

**Problem: Shift/reduce conflicts**
```bash
# Generate parser report
bison -v src/parser.y

# Check parser.output
# Look for conflict descriptions
# Adjust precedence or grammar
```

**Problem: Syntax error not caught**
```bash
# Check lexer is returning correct tokens
# Check grammar rules are complete
# Verify error handling in parser
```

## Contributing

### Contribution Process
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new features
5. Update documentation
6. Submit a pull request

### Code Review Checklist
- [ ] Code follows style guidelines
- [ ] Tests pass for new features
- [ ] Documentation is updated
- [ ] No memory leaks introduced
- [ ] Error handling is appropriate
- [ ] Performance impact is considered

### Release Process
1. Update version in changelog
2. Tag release in git
3. Update documentation
4. Announce changes

## Resources

### Internal Documentation
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [CODE_FLOW.md](CODE_FLOW.md) - Code execution patterns
- [LANGUAGE_SPEC.md](LANGUAGE_SPEC.md) - Language specification

### External Resources
- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)

### Tools
- **GDB**: GNU Debugger for runtime debugging
- **Valgrind**: Memory leak detection
- **Clang Static Analyzer**: Code analysis
