#!/bin/bash

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
BLUE='\033[0;34m'

echo -e "${BLUE}=== Building Zelx Compiler ===${NC}"
make clean && make > /dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}Compiler build failed!${NC}"
    exit 1
fi
echo -e "${GREEN}Compiler built successfully!${NC}\n"

echo -e "${BLUE}=== Running Test Cases ===${NC}"

for file in test/valid/*.zx; do
    filename=$(basename "$file")
    echo -e "Running test: ${BLUE}$filename${NC}..."
    
    # Run transpiler
    ./zelx "$file" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
         echo -e "  Transpiling: ${RED}[FAIL]${NC}"
         continue
    fi
    
    # Compile output.c
    gcc output.c -o test_run > /dev/null 2>&1
    if [ $? -ne 0 ]; then
         echo -e "  C Compilation: ${RED}[FAIL]${NC}"
         continue
    fi
    
    # Execute (pass arguments if it is the CLI args test)
    if [ "$filename" == "04_args.zx" ]; then
        output=$(./test_run "foo" "bar" 2>&1)
    else
        output=$(./test_run 2>&1)
    fi
    status=$?
    
    if [ $status -eq 0 ]; then
        echo -e "  Status: ${GREEN}[PASS]${NC}"
        echo "  Output:"
        echo "$output" | sed 's/^/    /'
    else
        echo -e "  Status: ${RED}[FAIL]${NC} (Exit code: $status)"
        echo "  Error Output:"
        echo "$output" | sed 's/^/    /'
    fi
    echo ""
done

# Cleanup
rm -f test_run output.c
