#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of argument must be 2\n");
        return 1;
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:");
    printf("\tmov rax, %ld\n", strtol(p, &p, 10));

    while(*p){
        if(*p == '+'){
            p++;
            printf("\tadd rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        if(*p == '-'){
            p++;
            printf("\tsub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "unexpected token\n");
        return 1;
    }
        
    printf("\tret\n");
    return 0;
}