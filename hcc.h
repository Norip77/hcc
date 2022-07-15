#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_EOF
}TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char* str;
    int len;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Token* tokenize(char *input);

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_NUM,
    ND_ASSIGN,
    ND_RETURN,
    ND_LVAR,
    ND_IF,
    ND_ELSE,
    ND_WHILE,
    ND_FOR
} NodeKind;

typedef struct Node Node;
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};


Node** parse(Token *tok);

void codegen(Node *node);