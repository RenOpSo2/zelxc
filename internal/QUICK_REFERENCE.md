# Quick Reference Guide

## Compiler Commands

### Building
```bash
# Using build.zx (recommended)
./zelx build.zx build

# Using Makefile
make

# Manual build
mkdir -p build
bison -d -o build/parser.tab.c src/parser.y
flex -o build/lex.yy.c src/lex.l
gcc -Wall -Wextra -g -Ibuild -Isrc -c build/lex.yy.c -o build/lex.yy.o
gcc -Wall -Wextra -g -Ibuild -Isrc -c build/parser.tab.c -o build/parser.tab.o
gcc -Wall -Wextra -g -Ibuild -Isrc -c src/codegen.c -o build/codegen.o
gcc -Wall -Wextra -g -Ibuild -Isrc -c src/main.c -o build/main.o
gcc -Wall -Wextra -g -o zelx build/lex.yy.o build/parser.tab.o build/codegen.o build/main.o -lfl
```

### Cleaning
```bash
./zelx build.zx clean
# or
make clean
```

### Testing
```bash
./zelx build.zx test
# or
make test
# or
./run_tests.sh
```

### Compiling Zelx Programs
```bash
./zelx program.zx
gcc output.c -o program
./program
```

## File Structure

```
src/
├── main.c          # Entry point, pipeline orchestration
├── codegen.c       # Code generation, runtime helpers
├── codegen.h       # AST structures, function declarations
├── parser.y        # Bison grammar, semantic actions
└── lex.l           # Flex lexer, token definitions

build/              # Generated files (don't edit)
├── parser.tab.c    # Generated parser
├── parser.tab.h    # Parser header
├── lex.yy.c        # Generated lexer
└── *.o             # Object files

test/
├── valid/          # Valid test cases
└── run_tests.sh    # Test runner

internal/           # This documentation
├── ARCHITECTURE.md         # System architecture
├── CODE_FLOW.md            # Code execution patterns
├── DEVELOPER_GUIDE.md      # Development guide
├── LANGUAGE_SPEC.md        # Language specification
├── IMPLEMENTATION_DETAILS.md  # Technical decisions
└── QUICK_REFERENCE.md      # This file
```

## AST Node Types

### ValueNode Types
```c
0: int
1: float
2: string
3: bool
4: array
5: object (placeholder)
6: index_access
7: identifier
8: binary_op
9: exec
10: len
11: must
12: has_flag
13: get_flag
```

### Binary Operators
```c
0: EQ   (==)
1: NE   (!=)
2: LT   (<)
3: GT   (>)
4: LE   (<=)
5: GE   (>=)
6: ADD  (+)
7: AND  (and)
8: OR   (or)
9: NOT  (not)
```

## Language Syntax Quick Reference

### Variables
```zx
name = "value"
const PI = 3.14
a, b = 1, 2
```

### Arrays
```zx
arr = [1, 2, 3]
element = arr[0]
length = len(arr)
```

### Control Flow
```zx
if condition {
    # code
} else if condition {
    # code
} else {
    # code
}
```

### Built-in Functions
```zx
print("Hello", name)
len(array)
exec("command")
must(condition)
has_flag("--flag")
get_flag("--flag", "default")
```

### Operators
```zx
# Arithmetic
result = a + b

# Comparison
a == b, a != b, a < b, a > b, a <= b, a >= b

# Logical
a and b, a or b, not a
```

## Common Tasks

### Adding a New Token
1. Add pattern to `src/lex.l`
2. Add token to `src/parser.y` `%token` section
3. Use in grammar rules

### Adding a New Operator
1. Add token to lexer
2. Add to precedence in parser
3. Add grammar rule
4. Add case in codegen
5. Add runtime function

### Adding a Built-in Function
1. Add keyword to lexer
2. Add grammar rule
3. Add node type
4. Add creator function
5. Add codegen case
6. Add runtime implementation

### Debugging Parser
```bash
bison -v src/parser.y
cat build/parser.output
```

### Debugging Generated Code
```bash
./zelx program.zx
cat output.c
gcc -g output.c -o program
gdb ./program
```

## Error Messages

### Common Compile Errors
- **"Cannot open file"**: Check file path and permissions
- **"Syntax error"**: Check grammar and lexer rules
- **"Undefined reference"**: Check linking order and libraries

### Common Runtime Errors
- **"Index out of bounds"**: Array access beyond length
- **"Attempt to index non-array"**: Using index on non-array
- **"must() assertion failed"**: Condition in must() was false
- **"exec command must be a string"**: Wrong type passed to exec()

## Code Patterns

### AST Node Creation
```c
struct ValueNode* node = malloc(sizeof(struct ValueNode));
node->type = TYPE_XXX;
node->xxx_val = value;
return node;
```

### Code Generation
```c
fprintf(output_file, "#line %d \"%s\"\n", line_no, source_filename);
fprintf(output_file, "/* C code */;\n");
```

### Error Handling
```c
if (error_condition) {
    fprintf(stderr, "Runtime Error: message\n");
    exit(1);
}
```

## Testing

### Running All Tests
```bash
./run_tests.sh
```

### Running Single Test
```bash
./zelx test/valid/01_basic.zx
gcc output.c -o test_bin
./test_bin
```

### Writing New Test
1. Create file in `test/valid/`
2. Name with descriptive prefix
3. Test specific feature
4. Add to test suite automatically

## Performance Tips

### Compilation Speed
- Use single-pass (already implemented)
- Avoid complex grammar rules
- Minimize lookahead

### Runtime Speed
- Pre-allocate when possible
- Avoid excessive string operations
- Use array length caching

### Memory Usage
- Compiler intentionally leaks AST (acceptable)
- User programs should manage memory
- Consider garbage collection for future

## Troubleshooting

### Build Fails
1. Check Flex/Bison installed
2. Check GCC installed
3. Check file permissions
4. Clean and rebuild

### Parser Conflicts
1. Run `bison -v src/parser.y`
2. Check `parser.output`
3. Adjust precedence
4. Simplify grammar

### Runtime Segfault
1. Compile with `-g`
2. Run with GDB
3. Check backtrace
4. Look for null pointers

### Generated Code Wrong
1. Check AST node types
2. Verify codegen switches
3. Check runtime functions
4. Add debug output

## Key Contacts and Resources

### Documentation
- `ARCHITECTURE.md` - System design
- `CODE_FLOW.md` - Execution patterns
- `DEVELOPER_GUIDE.md` - Development guide
- `LANGUAGE_SPEC.md` - Language reference
- `IMPLEMENTATION_DETAILS.md` - Technical details

### External Resources
- Flex Manual: https://westes.github.io/flex/manual/
- Bison Manual: https://www.gnu.org/software/bison/manual/
- GCC Documentation: https://gcc.gnu.org/onlinedocs/

### Tools
- GDB: Debugging
- Valgrind: Memory analysis
- GCC: Compilation
- Make: Build automation

## Version Information

### Current Version
- Check `changelog.txt` for version history
- Version defined in build scripts

### Dependencies
- Flex (version 2.6+)
- Bison (version 3.0+)
- GCC (version 4.8+)

## Quick Commands Reference

```bash
# Build
./zelx build.zx build

# Clean
./zelx build.zx clean

# Test
./zelx build.zx test

# Compile program
./zelx program.zx
gcc output.c -o program
./program

# Debug build
gcc -g output.c -o program
gdb ./program

# Parser debug
bison -v src/parser.y
cat build/parser.output

# Run specific test
./zelx test/valid/01_basic.zx
gcc output.c -o test_bin
./test_bin
```

## Common Modifications

### Change Operator Precedence
Edit `src/parser.y`:
```bison
%left NEW_OPERATOR
%left EXISTING_OPERATOR
```

### Add New Type
1. Add to ValueType enum in codegen
2. Add to Value union
3. Add creator function
4. Add runtime support
5. Update printing

### Modify Error Messages
Edit `src/codegen.c` runtime functions:
```c
fprintf(stderr, "Custom error message\n");
```

### Change Build Flags
Edit `build.zx` or `Makefile`:
```bash
gcc -Wall -Wextra -g -O2  # Add optimization
```

## Best Practices

### Code Style
- 4-space indentation
- Descriptive names
- Comment complex logic
- Follow existing patterns

### Testing
- Test each new feature
- Add edge cases
- Test error conditions
- Update documentation

### Documentation
- Update this guide
- Add code comments
- Document non-obvious decisions
- Keep changelog updated

### Git Workflow
- Commit frequently
- Write clear messages
- Test before commit
- Update documentation

## Emergency Procedures

### Compiler Won't Build
```bash
make clean
./zelx build.zx clean
./zelx build.zx build
```

### Tests Failing
```bash
# Check recent changes
git diff

# Revert if needed
git checkout src/file.c

# Rebuild
make clean && make
```

### Generated Code Broken
```bash
# Check source .zx file
# Check AST construction
# Check codegen switches
# Add debug output
```

### System Crash
```bash
# Check memory
valgrind ./program

# Check for infinite loops
# Check stack overflow
# Check recursion depth
```
