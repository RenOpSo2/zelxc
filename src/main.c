#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "parser.tab.h"

extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <file.zx>\n", argv[0]);
        return 1;
    }
    
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Cannot open file: %s\n", argv[1]);
        return 1;
    }
    
    codegen_init(argv[1]);
    yyparse();
    codegen_finish();
    
    fclose(yyin);
    printf("Generated output.c\n");
    return 0;
}
