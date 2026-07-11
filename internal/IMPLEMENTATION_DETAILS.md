# Implementation Details and Technical Decisions

## AST Node Type System

### ValueNode Type Enumeration
```c
// Type identifiers in ValueNode
int type; // 0:int, 1:float, 2:string, 3:bool, 4:array, 5:object, 
          // 6:index_access, 7:identifier, 8:binary_op, 9:exec, 
          // 10:len, 11:must, 12:has_flag, 13:get_flag
```

**Rationale:** 
- Simple integer-based type system for quick dispatch
- Avoids enum overhead in AST representation
- Types mapped to runtime ValueType enum in generated code

### Union Field Usage
```c
struct ValueNode {
    int type;
    int int_val;           // Used for: int, bool
    double float_val;      // Used for: float
    char* str_val;         // Used for: string, identifier
    struct ValueList* list_val;  // Used for: array
    struct ValueNode* index_expr; // Used for: index_access, exec, len, must, has_flag
    int op;                // Used for: binary_op
    struct ValueNode* left;     // Used for: binary_op, get_flag
    struct ValueNode* right;    // Used for: binary_op, get_flag
};
```

**Memory Optimization:**
- Single union for all value types
- Fields reused across different node types
- Trade-off: memory waste vs. simplicity

## Parser Implementation Details

### Grammar Design Decisions

#### Precedence Declaration
```bison
%left OR
%left AND
%left EQ NE LT GT LE GE
%left PLUS
%right NOT
```

**Operator Precedence (highest to lowest):**
1. NOT (right-associative)
2. PLUS (addition/concatenation)
3. Comparison operators (left-associative)
4. AND (logical AND)
5. OR (logical OR)

**Rationale:** Follows conventional programming language precedence

#### Expect Directive
```bison
%expect 2
```

**Purpose:** 
- Suppresses warnings for 2 known shift/reduce conflicts
- Conflicts are from if-else grammar ambiguity
- Standard resolved by "shift" (else binds to nearest if)

### Semantic Action Strategy

#### Immediate Code Generation
```bison
statement:
    single_assign { codegen_assign($1); }
    ;
```

**Rationale:**
- Single-pass compilation
- No separate AST traversal
- Simpler implementation
- Trade-off: harder to optimize

#### Line Number Tracking
```bison
statement:
    single_assign { codegen_assign($1); }
    ;

single_assign:
    IDENTIFIER ASSIGN value { 
        $$ = create_assign_node($1, $3, 0, yylineno); 
    }
    ;
```

**Implementation:**
- Flex tracks line numbers with `yylineno`
- Line numbers stored in AST nodes
- Emitted as `#line` directives in C code
- Enables proper error messages and debugging

## Code Generation Strategy

### Inline Code Generation

#### Approach
- Code generated during parsing (single-pass)
- Direct C code emission via `fprintf`
- No intermediate representation after AST

**Advantages:**
- Simple implementation
- Fast compilation
- Low memory usage

**Disadvantages:**
- No optimization passes
- Harder to generate optimal code
- Limited error recovery

### Runtime System Design

#### Value Union Type
```c
typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_BOOL, TYPE_ARRAY, TYPE_OBJECT } ValueType;

typedef struct Value {
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
} Value;
```

**Design Decisions:**
- Self-referential struct for arrays
- Enum for type safety in generated code
- Union for memory efficiency
- Struct wrapper for array metadata

#### Runtime Helper Functions

##### Truthiness Evaluation
```c
int is_truthy(Value v) {
    switch (v.type) {
        case TYPE_BOOL: return v.data.bool_val;
        case TYPE_INT: return v.data.int_val != 0;
        case TYPE_FLOAT: return v.data.float_val != 0.0;
        case TYPE_STRING: return v.data.str_val && strlen(v.data.str_val) > 0;
        default: return 0;
    }
}
```

**Rationale:**
- Consistent truthiness across types
- Follows JavaScript/Python conventions
- Empty strings and zero are falsy

##### Array Safety
```c
Value get_array_element(Value arr, Value index) {
    if (arr.type != TYPE_ARRAY) {
        fprintf(stderr, "Runtime Error: Attempt to index non-array value\n");
        exit(1);
    }
    // ... bounds checking
    if (idx < 0 || idx >= arr.data.array_val.length) {
        fprintf(stderr, "Runtime Error: Index %d out of bounds (length %d)\n", idx, arr.data.array_val.length);
        exit(1);
    }
    return arr.data.array_val.elements[idx];
}
```

**Safety Features:**
- Type checking before access
- Bounds checking with clear error messages
- Immediate exit on errors (no exception handling)

##### String Concatenation
```c
Value eval_add(Value left, Value right) {
    if (left.type == TYPE_STRING || right.type == TYPE_STRING) {
        // String concatenation path
        char l_buf[256], r_buf[256];
        // Convert to strings
        // Allocate new string
        // Concatenate
    } else {
        // Numeric addition path
    }
}
```

**Implementation Notes:**
- Fixed buffer size (256 chars) for conversion
- Dynamic allocation for result
- Auto-conversion of numbers to strings
- Trade-off: buffer overflow risk vs. simplicity

## Memory Management

### AST Memory Policy

#### Intentional Leaks
```c
struct ValueNode* create_int_value(int val) {
    struct ValueNode* node = malloc(sizeof(struct ValueNode));
    // ... set fields
    return node;
}
// Note: No free() called anywhere
```

**Rationale:**
- Compiler short-lived process
- OS reclaims all memory on exit
- Simplifies implementation
- No need for complex cleanup

#### String Handling
```c
"string_literal"  {
    yytext[strlen(yytext)-1] = '\0';
    yylval.strval = strdup(yytext + 1);
    return STRING_LIT;
}
```

**Implementation:**
- `strdup()` duplicates string
- Removes quotes from literal
- Lexer allocates, never frees
- Acceptable for compiler use

### Generated Code Memory

#### Runtime Allocation
```c
// In generated code
char* new_str = malloc(strlen(l_buf) + strlen(r_buf) + 1);
strcpy(new_str, l_buf);
strcat(new_str, r_buf);
```

**Policy:**
- Dynamic allocation for strings
- No garbage collection
- Memory leaks in user programs possible
- Trade-off: simplicity vs. memory efficiency

## Error Handling Strategy

### Compile-Time Errors

#### Parser Error Handling
```c
void yyerror(const char* s) {
    fprintf(stderr, "Error: %s at '%s'\n", s, yytext);
}
```

**Characteristics:**
- Errors reported to stderr
- Includes error location (token)
- Parser continues after errors (when possible)
- No sophisticated error recovery

### Runtime Errors

#### Error Handling Pattern
```c
if (error_condition) {
    fprintf(stderr, "Runtime Error: descriptive message\n");
    exit(1);
}
```

**Design Decisions:**
- All runtime errors fatal
- No exception handling
- Clear error messages
- Immediate program termination

#### Assertion System
```c
Value run_must(Value v) {
    if (v.type == TYPE_INT && v.data.int_val != 0) {
        fprintf(stderr, "Runtime Error: must() assertion failed with exit code %d\n", v.data.int_val);
        exit(1);
    } else if (v.type == TYPE_BOOL && v.data.bool_val == 0) {
        fprintf(stderr, "Runtime Error: must() assertion failed (false)\n");
        exit(1);
    }
    return v;
}
```

**Purpose:**
- Prevents "arrow anti-pattern" (deep nesting)
- Provides clean error handling
- Early termination on failure
- Clear error messages

## Build System Design

### build.zx Implementation

#### Self-Hosting Build
```zx
print("🚀 Building Zelx Compiler...")
must(exec("bison -d -o build/parser.tab.c src/parser.y"))
must(exec("flex -o build/lex.yy.c src/lex.l"))
```

**Advantages:**
- Demonstrates language capabilities
- Single build tool dependency
- Consistent with language philosophy
- Easy to understand

**Challenges:**
- Chicken-and-egg problem (need compiler to build compiler)
- Bootstrapping required
- Error handling via `must()`

#### Dependency Management
```zx
print("📦 Running Bison on src/parser.y...")
must(exec("bison -d -o build/parser.tab.c src/parser.y"))

print("📦 Running Flex on src/lex.l...")
must(exec("flex -o build/lex.yy.c src/lex.l"))
```

**Order:**
1. Parser generation (creates header for lexer)
2. Lexer generation (needs parser header)
3. Compilation (all generated files)
4. Linking (all object files)

### Makefile Alternative

#### Dependency Tracking
```makefile
$(BUILD_DIR)/lex.yy.c: $(SRC_DIR)/lex.l $(BUILD_DIR)/parser.tab.h
	$(FLEX) -o $@ $<
```

**Features:**
- Automatic dependency tracking
- Parallel compilation support
- Incremental builds
- Standard Unix tool

## Testing Infrastructure

### Test Organization

#### File Structure
```
test/
├── valid/
│   ├── 01_basic.zx
│   ├── 02_array.zx
│   └── ...
└── run_tests.sh
```

**Naming Convention:**
- Numbered prefix for ordering
- Descriptive names
- Separate valid/invalid directories (planned)

#### Test Runner Script
```bash
for test_file in test/valid/*.zx; do
    echo "Testing: $test_file"
    ./zelx "$test_file"
    gcc output.c -o test_bin
    ./test_bin
    # Check exit code
done
```

**Features:**
- Automatic test discovery
- Colored output
- Exit code checking
- Clear pass/fail indication

## Performance Considerations

### Compilation Performance

#### Single-Pass Advantage
- No multiple AST traversals
- Minimal intermediate representation
- Direct code emission
- Fast for small to medium programs

#### Flex/Bison Performance
- Lexer: O(n) where n is input size
- Parser: O(n) for LALR(1) grammar
- Grammar designed for efficient parsing

### Runtime Performance

#### Type Checking Overhead
```c
// Every operation checks types
if (left.type == TYPE_STRING || right.type == TYPE_STRING) {
    // String path
} else {
    // Numeric path
}
```

**Impact:**
- Branch prediction misses
- Cache inefficiency
- Trade-off: safety vs. performance

#### Array Access Safety
```c
// Every array access checked
if (idx < 0 || idx >= arr.data.array_val.length) {
    exit(1);
}
```

**Impact:**
- Bounds checking overhead
- Branch prediction
- Trade-off: safety vs. performance

#### String Operations
```c
// String concatenation involves:
// 1. Type check
// 2. Buffer allocation
// 3. Conversion to string
// 4. Memory allocation
// 5. Copying
```

**Impact:**
- Multiple allocations
- Copy overhead
- No string interning
- Trade-off: simplicity vs. performance

## Debugging Support

### Source Mapping

#### Line Directives
```c
fprintf(output_file, "#line %d \"%s\"\n", node->line_no, source_filename);
```

**Purpose:**
- Maps generated C to source .zx
- Enables proper error messages
- Supports GDB debugging
- Essential for maintenance

### Debug Builds

#### GCC Flags
```makefile
CFLAGS = -Wall -Wextra -g
```

**Features:**
- `-g`: Debug symbols
- `-Wall`: All warnings
- `-Wextra`: Extra warnings
- Essential for development

## Known Limitations

### Language Limitations
- No function definitions
- No loops (for, while)
- No object literals
- No module system
- No exception handling
- No type annotations

### Compiler Limitations
- No optimization passes
- Single-pass only
- Limited error recovery
- No source maps
- Basic error messages

### Runtime Limitations
- No garbage collection
- Manual memory management
- No standard library
- Limited built-in functions
- No FFI (Foreign Function Interface)

## Future Implementation Plans

### Short-term
- Add loop constructs
- Implement function definitions
- Improve error messages
- Add more built-in functions

### Medium-term
- Add module system
- Implement object literals
- Add optimization passes
- Improve runtime performance

### Long-term
- Garbage collection
- Standard library
- FFI support
- JIT compilation option

## Portability Considerations

### Platform Dependencies
- Flex/Bison available on most Unix systems
- GCC widely available
- Standard C library only
- No platform-specific code

### Build System
- Makefile for Unix-like systems
- Potential for CMake support
- build.zx requires working compiler first

### Testing
- Shell scripts assume Unix
- Test runner uses bash features
- Could be adapted for other platforms

## Security Considerations

### Exec Function
```zx
exec(user_input)  # Dangerous!
```

**Risks:**
- Command injection
- Arbitrary code execution
- Privilege escalation

**Mitigations:**
- Document security risks
- Recommend input validation
- Consider sandboxing in future

### Runtime Errors
- All runtime errors exit immediately
- No graceful degradation
- Potential for DoS in critical systems

**Mitigations:**
- Document error behavior
- Recommend proper testing
- Consider error recovery in future

## Maintenance Guidelines

### Adding New Features
1. Update lexer (tokens)
2. Update parser (grammar)
3. Update AST (structures)
4. Update codegen (generation)
5. Add runtime support
6. Write tests
7. Update documentation

### Debugging Issues
1. Reproduce with minimal example
2. Check lexer output (if needed)
3. Examine parser.output (for conflicts)
4. Inspect generated C code
5. Use GDB for runtime issues
6. Add debug output if needed

### Performance Profiling
1. Use GCC profiling (`-pg`)
2. Analyze with `gprof`
3. Identify hot paths
4. Optimize critical sections
5. Measure improvements

### Code Quality
- Follow existing patterns
- Keep functions small
- Document non-obvious code
- Add tests for bugs fixed
- Update changelog
