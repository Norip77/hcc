#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of argument must be 2\n");
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:");
    printf("\tmov rax, %d\n", atoi(argv[1]));
    printf("\tret\n");
    return 0;
}