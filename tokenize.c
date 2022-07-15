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

        if(!strncmp(p, "if", 2) && !isalnum(p[2]) && p[2] != '_'){
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }
        
        if(!strncmp(p, "else", 2) && !isalnum(p[4]) && p[4] != '_'){
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        if(!strncmp(p, "while", 5) && !isalnum(p[5]) && p[5] != '_'){
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        if(!strncmp(p, "for", 3) && !isalnum(p[3]) && p[3] != '_'){
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
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