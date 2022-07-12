#include "hcc.h"

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

char *user_input;

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of argument must be 2\n");
        return 1;
    }

    user_input = argv[1];

    Token *token = tokenize(argv[1]);

    Node *node = parse(token);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    codegen(node);
    printf("\tpop rax\n");
        
    printf("\tret\n");
    return 0;
}
