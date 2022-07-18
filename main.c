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

    Node **code = parse(token);

    printf(".intel_syntax noprefix\n");

    for(int i = 0; code[i]; ++i){    
        
        char name[64];
        strncpy(name, code[i]->name, code[i]->len); 
        printf(".globl %s\n%s:\n", name, name);
        printf("\tpush rbp\n");
        printf("\tmov rbp, rsp\n");
        printf("\tsub rsp, 208\n");
        for(Node *node = code[i]->block; node; node = node->block){

            codegen(node);
        }
        printf("\tpop rax\n");
    }
    printf("\tmov rsp,rbp\n");
    printf("\tpop rbx\n");
        
    printf("\tret\n");
    return 0;
}
