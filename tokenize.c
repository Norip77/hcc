#include "hcc.h"

static struct
{
    char *key;
    int token;
}keywords[] = {
    {"return", TK_RETURN},
    {"if", TK_IF},
    {"else", TK_ELSE},
    {"while", TK_WHILE},
    {"for", TK_FOR},
    {NULL, 0}
};

Token* new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

int find_keyword(char *key){
    int len;
    for(int i = 0;keywords[i].key;++i){
        len = strlen(keywords[i].key);
        if(!strncmp(keywords[i].key, key, len) && !isalnum(key[len]) && key[len] != '_'){
            return i;
        }
    }
    return -1;
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

        if(strchr("+-*/()<>;={},&", *p)){
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

        int id = find_keyword(p);
        if(id != -1){
            int len = strlen(keywords[id].key);
            cur = new_token(keywords[id].token, cur, p, len);
            p += len;
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