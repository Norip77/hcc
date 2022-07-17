#include "hcc.h"

static void gen_lval(Node *node){
    if(node->kind != ND_LVAR){
        error("not lvar");
    }
    printf("\tmov rax, rbp\n");
    printf("\tsub rax, %d\n", node->offset);
    printf("\tpush rax\n");
}

int labelnum;
char *args[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
char name[64];

void codegen(Node *node){
    if(node == NULL) return;
    int tmpln;
    int i = 0;
    switch(node->kind){
        case ND_IF:
            tmpln = labelnum++;
            codegen(node->lhs);
            printf("\tpop rax\n");
            printf("\tcmp rax, 0\n");
            printf("\tje .Lelse%d\n", tmpln);
            codegen(node->rhs->lhs);
            printf("\tjmp .Lend%d\n", tmpln);
            printf(".Lelse%d:\n", tmpln);
            codegen(node->rhs->rhs);
            printf(".Lend%d:\n", tmpln);
            return;
        case ND_WHILE:
            tmpln = labelnum++;
            printf(".Lbegin%d:\n", tmpln);
            codegen(node->lhs);
            printf("\tpop rax\n");
            printf("\tcmp rax, 0\n");
            printf("je .Lend%d\n", tmpln);
            codegen(node->rhs);
            printf("\tjmp .Lbegin%d\n", tmpln);
            printf(".Lend%d:\n", tmpln);
            return;
        case ND_FOR:
            tmpln = labelnum++; 
            codegen(node->lhs->lhs);
            printf(".Lbegin%d:\n", tmpln);
            if(node->lhs->rhs){
                codegen(node->lhs->rhs);
            }else{
                printf("\tpush 1\n");
            }
            printf("\tpop rax\n");
            printf("\tcmp rax, 0\n");
            printf("\tje .Lend%d\n", tmpln);
            codegen(node->rhs->rhs);
            codegen(node->rhs->lhs);
            printf("\tjmp .Lbegin%d\n", tmpln);
            printf(".Lend%d:\n", tmpln);
            return;
        case ND_BLOCK:
            for(Node *n = node->block; n; n = n->block){
                codegen(n);
            }
            return;
        case ND_CALL:
            
            strncpy(name, node->name, node->len);
            for(; node->args; node = node->args, ++i){
                codegen(node->args);
            }
            for(int j = i - 1; j >= 0; --j){
                printf("\tpop %s\n", args[j]);
            }
            printf("\tcall %s\n", name);
            return;
        case ND_ADDR:
            gen_lval(node->lhs);
            return;
        case ND_DEREF:
            codegen(node->lhs);
            printf("\tpop rax\n");
            printf("\tmov rax, [rax]\n");
            printf("\tpush rax\n");
            return;
        case ND_RETURN:
            codegen(node->lhs);
            printf("\tpop rax\n");
            printf("\tmov rsp, rbp\n");
            printf("\tpop rbp\n");
            printf("\tret\n");
            return;
        case ND_NUM:
            printf("\tpush %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("\tpop rax\n");
            printf("\tmov rax, [rax]\n");
            printf("\tpush rax\n");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            codegen(node->rhs);

            printf("\tpop rdi\n");
            printf("\tpop rax\n");
            printf("\tmov [rax], rdi\n");
            printf("\tpush rdi\n");
            return;
    }
    codegen(node->lhs);
    codegen(node->rhs);

    printf("\tpop rdi\n");
    printf("\tpop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("\tadd rax, rdi\n");
        break;
    case ND_SUB:
        printf("\tsub rax, rdi\n");
        break;
    case ND_MUL:
        printf("\timul rax, rdi\n");
        break;
    case ND_DIV:
        printf("\tcqo\n");
        printf("\tidiv rdi\n");
        break;
    case ND_EQ:
        printf("\tcmp rax, rdi\n");
        printf("\tsete al\n");
        printf("\tmovzb rax, al\n");
        break;
    case ND_NE:
        printf("\tcmp rax, rdi\n");
        printf("\tsetne al\n");
        printf("\tmovzb rax, al\n");
        break;
    case ND_LT:
        printf("\tcmp rax, rdi\n");
        printf("\tsetl al\n");
        printf("\tmovzb rax, al\n");
        break;
    case ND_LE:
        printf("\tcmp rax, rdi\n");
        printf("\tsetle al\n");
        printf("\tmovzb rax, al\n");
        break;
    }  
    printf("\tpush rax\n");
}