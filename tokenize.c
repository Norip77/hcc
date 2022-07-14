#include "hcc.h"

Token* new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
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

        if(!strncmp(p, "==", 2) || !strncmp(p, "!=", 2) || !strncmp(p, ">=", 2) || !strncmp(p, "<=", 2)){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p+=2;
            continue;
        }

        if(strchr("+-*/()<>;=", *p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if(!strncmp(p, "return", 6) && !isalnum(p[6]) && p[6] != '_'){
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if(isalpha(*p)){
            int len = 1;
            ++p;
            while(isalnum(*p) || *p == '_'){
                ++len;
                ++p;
            }
            cur = new_token(TK_IDENT, cur, p - len, len);
            continue;
        }

        error_at(p, "disable to tokenize");
    }
    
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}