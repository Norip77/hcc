#include "hcc.h"

static Token *token;



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

bool consume(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len || strncmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

void expect(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len || strncmp(token->str, op, token->len)){
        error_at(token->str, "%c cant be", op);
    }
    token = token->next;
}

Token* consume_ident(){
    if(token->kind != TK_IDENT){
        return NULL;
    }
    Token* p = token;
    token = token->next;
    return p;
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

static void program();
static Node* stmt();
static Node* expr();
static Node* assign();
static Node* equality();
static Node* relation();
static Node* add();
static Node* mul();
static Node* primary();
static Node* unary();

static Node *code[100];

static void program(){
    int i = 0;
    while(!at_eof()){
        code[i++] = stmt();
    }
    code[i] = NULL;
}

static Node* stmt(){
    Node *node = expr();
    expect(";");
    return node;
}

static Node* expr(){
    return assign();
}

static Node* assign(){
    Node *node = equality();
    if(consume("=")){
        node = new_binary(ND_ASSIGN, node, equality());
    }
    return node;
}

static Node* equality(){
    Node *node = relation();

    for(;;){
        if(consume("==")){
            node = new_binary(ND_EQ, node, relation());
        }else if(consume("!=")){
            node = new_binary(ND_NE, node, relation());
        }else{
            return node;
        }
    }
}

static Node* relation(){
    Node *node = add();

    for(;;){
        if(consume("<")){
            node = new_binary(ND_LT, node, add());
        }else if(consume("<=")){
            node = new_binary(ND_LE, node, add());
        }else if(consume(">")){
            node = new_binary(ND_LT, add(), node);
        }else if(consume(">=")){
            node = new_binary(ND_LE, add(), node);
        }else{
            return node;
        }
    }
}
static Node* add(){
    Node *node = mul();

    for(;;){
        if(consume("+")){
            node = new_binary(ND_ADD, node, mul());
        }else if(consume("-")){
            node = new_binary(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

static Node* mul(){
    Node *node = unary();

    for(;;){
        if(consume("*")){
            node = new_binary(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_binary(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

static Node* unary(){
    if(consume("+")){
        return primary();
    }
    if(consume("-")){
        return new_binary(ND_SUB, new_num(0), primary());
    }
    return primary();
}


static Node* primary(){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    Token* tok = consume_ident();
    if(tok){
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }
    return new_num(expect_number());
}

Node** parse(Token *tok){
    token = tok;
    program();
    return code;
}