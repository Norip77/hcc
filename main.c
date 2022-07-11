#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
    TK_RESERVED,
    TK_NUM,
    TK_EOF
}TokenKind;

typedef struct Token Token;

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char* str;
};

Token *token;

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

bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token->next;
    return true;
}

void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        error("%c cant be", op);
    }
    token = token->next;
}

int expect_number(){
    if (token->kind != TK_NUM){
        error("not number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token* tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if(strchr("+-*/()", *p)){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "disable to tokenize");
    }
    
    new_token(TK_EOF, cur, p);
    return head.next;
}

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM
} NodeKind;

typedef struct Node Node;
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node* new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node* new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val){
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node* expr();
Node* mul();
Node* primary();
Node* unary();

Node* expr(){
    Node *node = mul();

    for(;;){
        if(consume('+')){
            node = new_binary(ND_ADD, node, mul());
        }else if(consume('-')){
            node = new_binary(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node* mul(){
    Node *node = unary();

    for(;;){
        if(consume('*')){
            node = new_binary(ND_MUL, node, unary());
        }else if(consume('/')){
            node = new_binary(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node* unary(){
    if(consume('+')){
        return primary();
    }
    if(consume('-')){
        return new_binary(ND_SUB, new_num(0), primary());
    }
    return primary();
}


Node* primary(){
    if(consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }
    return new_num(expect_number());
}

void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("\tpush %d\n", node->val);
        return;
    }
    gen(node->lhs);
    gen(node->rhs);

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
    }  
    printf("\tpush rax\n");
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "the number of argument must be 2\n");
        return 1;
    }

    user_input = argv[1];

    token = tokenize(argv[1]);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);
    printf("\tpop rax\n");
        
    printf("\tret\n");
    return 0;
}
