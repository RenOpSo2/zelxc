# Zelx Language Specification

## Overview
Zelx is a dynamically-typed scripting language that transpiles to C. It provides high-level features like dynamic typing, arrays, built-in functions, and shell command execution while maintaining simplicity and performance through C compilation.

## Lexical Structure

### Comments
```zx
# Single-line comments
# Everything after # is ignored until end of line
```

### Identifiers
```zx
# Variable names
variable_name
counter123
_private_var
```

**Rules:**
- Start with letter or underscore
- Followed by letters, digits, or underscores
- Case-sensitive

### Keywords
```zx
const      # Constant declaration
print      # Print function
if         # Conditional
else       # Conditional alternative
exec       # Execute shell command
and        # Logical AND
or         # Logical OR
not        # Logical NOT
len        # Length function
must       # Assertion function
has_flag   # CLI flag check
get_flag   # CLI flag value
true       # Boolean true
false      # Boolean false
```

### Literals

#### Integer Literals
```zx
42
-17
0
1000000
```

#### Float Literals
```zx
3.14
-0.001
2.0
```

#### String Literals
```zx
"Hello, World!"
"Multi\nline\nstring"
""
```

#### Boolean Literals
```zx
true
false
```

#### Array Literals
```zx
[]
[1, 2, 3]
["a", "b", "c"]
[1, "mixed", true, 3.14]
```

### Operators

#### Arithmetic Operators
```zx
+    # Addition and string concatenation
```

#### Comparison Operators
```zx
==   # Equal
!=   # Not equal
<    # Less than
>    # Greater than
<=   # Less than or equal
>=   # Greater than or equal
```

#### Logical Operators
```zx
and  # Logical AND
or   # Logical OR
not  # Logical NOT
```

#### Assignment Operator
```zx
=    # Assignment
```

## Syntax

### Variable Declaration

#### Regular Variables
```zx
name = "John"
age = 25
pi = 3.14
is_active = true
```

#### Constant Variables
```zx
const VERSION = "1.0.0"
const MAX_SIZE = 100
```

**Rules:**
- Constants cannot be reassigned
- Compile-time error if reassignment attempted
- Use `const` keyword for immutable values

### Multi-Assignment
```zx
a, b = 1, 2
x, y, z = 10, 20, 30
name, age = "Alice", 30
```

**Rules:**
- Multiple variables assigned in single statement
- Values assigned left-to-right
- If fewer values than variables, remaining get 0

### Arrays

#### Array Creation
```zx
numbers = [1, 2, 3, 4, 5]
names = ["Alice", "Bob", "Charlie"]
mixed = [1, "two", true, 4.0]
empty = []
```

#### Array Access
```zx
numbers = [10, 20, 30, 40]
first = numbers[0]    # 10
second = numbers[1]   # 20
last = numbers[3]     # 40
```

**Rules:**
- Zero-based indexing
- Runtime bounds checking
- Out-of-bounds access causes runtime error

### Command-Line Arguments

#### Accessing Arguments
```zx
# args is built-in array variable
command = args[0]      # First argument
filename = args[1]     # Second argument
```

**Rules:**
- `args` contains command-line arguments
- Excludes program name
- Available at program start

### CLI Flag Parsing

#### Check Flag Presence
```zx
show_help = has_flag("--help") or has_flag("-h")
verbose = has_flag("--verbose")
```

#### Get Flag Value
```zx
output = get_flag("--output", "default.txt")
name = get_flag("--name", "unknown")
count = get_flag("--count", "0")
```

**Rules:**
- `has_flag(flag)` returns boolean
- `get_flag(flag, default)` returns value or default
- Supports both `--long` and `-short` formats

### Expressions

#### Arithmetic
```zx
result = 10 + 5
total = price + tax
```

#### String Concatenation
```zx
message = "Hello, " + name
greeting = "Count: " + count
```

#### Comparison
```zx
is_equal = a == b
is_greater = x > y
in_range = 0 <= value <= 100
```

#### Logical
```zx
is_valid = x > 0 and x < 100
is_ready = not is_loading
can_proceed = is_valid or is_admin
```

#### Parentheses
```zx
result = (a + b) * c
condition = (x > 0) and (y < 10)
```

### Control Flow

#### If-Else Statements
```zx
if condition {
    # code block
}
```

#### If-Else-If
```zx
if condition1 {
    # code block 1
} else if condition2 {
    # code block 2
} else {
    # code block 3
}
```

**Rules:**
- Curly braces required for blocks
- Supports nested conditionals
- `else if` chain supported
- Final `else` optional

### Built-in Functions

#### Print Function
```zx
print("Hello, World!")
print("Name:", name, "Age:", age)
print("Array:", numbers)
print("Boolean:", is_valid)
```

**Rules:**
- Multiple arguments separated by commas
- Arguments separated by spaces in output
- Automatic string conversion
- Newline at end

#### Length Function
```zx
length = len(my_array)
size = len(my_string)
```

**Rules:**
- Works on arrays and strings
- Returns integer count
- Runtime error for other types

#### Exec Function
```zx
exit_code = exec("ls -la")
result = exec("gcc program.c -o program")
```

**Rules:**
- Executes shell command
- Returns exit code (integer)
- Command must be string
- Non-zero exit indicates failure

#### Must Function
```zx
must(exec("make build"))
must(file_exists)
must(success_condition)
```

**Rules:**
- Asserts expression is truthy
- Exits with error if false
- Prevents "arrow anti-pattern"
- Used for error handling

#### Has Flag Function
```zx
help_mode = has_flag("--help")
verbose = has_flag("-v")
```

**Rules:**
- Checks for flag presence
- Returns boolean
- Used for CLI parsing

#### Get Flag Function
```zx
output_file = get_flag("--output", "default.out")
threads = get_flag("--threads", "4")
```

**Rules:**
- Gets flag value or default
- Returns string
- Used for CLI parsing

## Type System

### Dynamic Typing
Variables are dynamically typed:
```zx
x = 42           # int
x = "hello"      # string
x = 3.14         # float
x = true         # bool
x = [1, 2, 3]    # array
```

### Type Coercion

#### Arithmetic Operations
```zx
# Int + Int → Int
result = 5 + 3        # 8

# Float + Float → Float
result = 3.14 + 2.86  # 6.0

# Int + Float → Float
result = 5 + 3.14     # 8.14

# String + Any → String
result = "Value: " + 42       # "Value: 42"
result = "Pi: " + 3.14        # "Pi: 3.14"
result = "Bool: " + true      # "Bool: true"
```

#### Boolean Conversion
```zx
# Truthy values
if 42 { print("truthy") }        # true
if 3.14 { print("truthy") }      # true
if "hello" { print("truthy") }   # true
if true { print("truthy") }      # true
if [1, 2] { print("truthy") }    # true

# Falsy values
if 0 { print("falsy") }          # false
if 0.0 { print("falsy") }        # false
if "" { print("falsy") }         # false
if false { print("falsy") }      # false
if [] { print("falsy") }         # false
```

## Semantics

### Variable Scope
- Global scope only (current implementation)
- All variables accessible throughout program
- No block-level scoping yet

### Execution Order
- Sequential execution from top to bottom
- Conditional branches control flow
- No function calls yet (planned feature)

### Error Handling

#### Compile-Time Errors
- Syntax errors
- Invalid token sequences
- Grammar rule violations

#### Runtime Errors
- Array index out of bounds
- Type mismatches in operations
- Failed assertions (must)
- Invalid exec commands

## Standard Library

### Built-in Variables
- `args`: Command-line argument array

### Built-in Functions
- `print(...)`: Output values
- `len(value)`: Get length
- `exec(command)`: Execute shell command
- `must(condition)`: Assert condition
- `has_flag(flag)`: Check CLI flag
- `get_flag(flag, default)`: Get CLI flag value

## Examples

### Hello World
```zx
print("Hello, World!")
```

### Variables and Types
```zx
name = "Alice"
age = 30
height = 5.7
is_student = true
grades = [95, 87, 92, 88]

print("Name:", name)
print("Age:", age)
print("Height:", height)
print("Student:", is_student)
print("Grades:", grades)
```

### Control Flow
```zx
score = 85

if score >= 90 {
    print("Grade: A")
} else if score >= 80 {
    print("Grade: B")
} else if score >= 70 {
    print("Grade: C")
} else {
    print("Grade: F")
}
```

### Array Operations
```zx
numbers = [10, 20, 30, 40, 50]
count = len(numbers)
first = numbers[0]
last = numbers[count - 1]

print("Count:", count)
print("First:", first)
print("Last:", last)
```

### String Operations
```zx
first_name = "John"
last_name = "Doe"
full_name = first_name + " " + last_name

print("Full name:", full_name)
```

### CLI Argument Parsing
```zx
command = args[0]
verbose = has_flag("--verbose")
output = get_flag("--output", "output.txt")

print("Command:", command)
print("Verbose:", verbose)
print("Output:", output)
```

### Shell Execution
```zx
print("Building project...")
must(exec("make clean"))
must(exec("make build"))
print("Build complete!")
```

### Build Script Example
```zx
command = "build"
if len(args) > 0 {
    command = args[0]
}

if command == "clean" {
    print("Cleaning...")
    exec("rm -rf build/")
} else if command == "build" {
    print("Building...")
    must(exec("mkdir -p build"))
    must(exec("gcc -o build/app src/main.c"))
    print("Done!")
} else {
    print("Usage: build.zx [clean|build]")
}
```

## Future Features (Planned)

### Functions
```zx
function add(a, b) {
    return a + b
}

result = add(5, 3)
```

### Loops
```zx
for i in range(10) {
    print(i)
}

while condition {
    # code
}
```

### Objects
```zx
person = {
    name: "Alice",
    age: 30,
    city: "NYC"
}

print(person.name)
```

### Modules
```zx
import math
result = math.sqrt(16)
```

### Error Handling
```zx
try {
    must(exec("risky_command"))
} catch error {
    print("Error:", error)
}
```

## Grammar Reference

### BNF Grammar (Simplified)
```
program         → statement_list
statement_list  → statement statement_list | ε
statement       → single_assign | const_assign | multi_assign 
                | print_stmt | if_stmt | expression
single_assign   → IDENTIFIER ASSIGN value
const_assign    → CONST IDENTIFIER ASSIGN value
multi_assign    → identifier_list ASSIGN value_list
print_stmt      → PRINT LPAREN print_args RPAREN
if_stmt         → IF value block elif_list else_opt
value           → literal | IDENTIFIER | array | object 
                | value binary_op value | NOT value
                | function_call | LPAREN value RPAREN
array           → LBRACKET array_elements RBRACKET
object          → LBRACE object_pairs RBRACE
```

## Implementation Notes

### Transpilation Process
1. Lexer tokenizes source code
2. Parser builds AST
3. Code generator emits C code
4. GCC compiles to executable

### Runtime Representation
- Dynamic types via C union
- Reference counting for strings
- Bounds checking for arrays
- Automatic memory management

### Performance Characteristics
- Type checking at runtime
- No JIT compilation
- Direct C execution
- Minimal runtime overhead
