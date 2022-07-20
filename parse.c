#include "hcc.h"

static Token *token;



LVar *locals[128];
int cur_func = 0;

LVar* find_lvar(Token *token){
    for(LVar *var = locals[cur_func]; var; var = var->next){
        if(var->len == token->len && !strncmp(token->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}

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

Type* new_type(TypeKind kind, Type *ptr_to){
    Type *type = calloc(1, sizeof(Type));
    type->kind = kind;
    type->ptr_to = ptr_to;
    return type;
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
        error_at(token->str, "%c cant be", *op);
    }
    token = token->next;
}

Token* consume_ident(){
    if(token->kind != TK_IDENT){
        return NULL;
    }
    Token *p = token;
    token = token->next;
    return p;
}

bool consume_tokkind(TokenKind kind){
    if(token->kind != kind){
        return false;
    }
    token = token->next;
    return true;
}

// Type* consume_type(){
//     Type *type;
//     switch(token->kind){
//         case TK_INT:
//             new_type(TY_INT, consume_type());
//     }
// }

Node* consume_decl_val(){
    Type *type;
    switch (token->kind) {
    case TK_INT:
        type = new_type(TY_INT, NULL);
        break;
    default:
        return NULL;
    }
    
    token = token->next;
    while(token->kind == TK_RESERVED && token->str[0] == '*'){
        type = new_type(TY_PTR, type);
        token = token->next;
    }
    Token *tok = consume_ident();
    if(!tok){
        error_at(token->str, "not identifier");
    }
    Node *node = new_node(ND_DECL);
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = locals[cur_func];
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->offset = locals[cur_func]->offset + 8;
    lvar->type = type;
    node->offset = lvar->offset;
    locals[cur_func] = lvar;
    return node;
}

int expect_number(){
    if (token->kind != TK_NUM){
        error_at(token->str, "not number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

static void program();
static Node* function();
static Node* stmt();
static Node* expr();
static Node* assign();
static Node* equality();
static Node* relation();
static Node* add();
static Node* mul();
static Node* primary();
static Node* unary();

Node *new_block(){
    Node *head, **bb;
    bb = &head;
    while(!consume("}")){
        *bb = stmt();
        bb = &(*bb)->block;
    }
    return head;
}

Node *new_args_decl(){
    if(consume(")")) return NULL;
    Node *node = consume_decl_val();
    if(!node){
        error_at(token->str,"must be declaration");
    }
    Node *head = node;
    Node **pp = &head;
    while (!consume(")")){
        expect(",");
        pp = &(*pp)->args;
        node = consume_decl_val();
        if(!node){
            error_at(token->str,"must be declaration");
        }
        *pp = node;
    }
    return head;
}

Node *new_args(){
    if(consume(")")) return NULL;
    Node *head = expr();
    Node **pp = &head;
    while (!consume(")")){
        expect(",");
        pp = &(*pp)->args;
        *pp = expr();
    }
    return head;
}

static Node *code[100];

static void program(){
    int i = 0;
    while(!at_eof()){
        locals[cur_func] = calloc(1, sizeof(LVar));
        if(!consume_tokkind(TK_INT)){
            error_at(token->str, "not declaration");
        }
        code[i++] = function();
        ++cur_func;
    }
    code[i] = NULL;
}

static Node* function(){
    Node *node = new_node(ND_FUNC);
    
    Token *tok = consume_ident();
    if(!tok){
        error_at(tok->str, "function name expected");
    }
    expect("(");
    node->name = tok->str;
    node->len = tok->len;
    node->args = new_args_decl();
    expect("{");
    node->block = new_block();
    return node;
}

static Node* stmt(){
    Node *node;

    if(consume_decl_val()){
        expect(";");
        return new_node(ND_DECL);
    }

    if(consume("{")){
        node = new_node(ND_BLOCK);
        node->block = new_block();
        return node;
    }

    if(consume_tokkind(TK_IF)){
        expect("(");
        node = new_binary(ND_IF, expr(), new_node(ND_ELSE));
        expect(")");
        node->rhs->lhs = stmt();
        if(consume_tokkind(TK_ELSE)){
            node->rhs->rhs = stmt();
        }
        return node;
    }

    if(consume_tokkind(TK_WHILE)){
        expect("(");
        node = new_binary(ND_WHILE, expr(), NULL);
        expect(")");
        node->rhs = stmt();
        return node;
    }

    if(consume_tokkind(TK_FOR)){
        expect("(");
        Node *node = new_binary(ND_FOR, new_node(0), new_node(0));
        if(!consume(";")){
            node->lhs->lhs = expr();
            expect(";");
        }
        if(!consume(";")){
            node->lhs->rhs = expr();
            expect(";");
        }
        if(!consume(")")){
            node->rhs->lhs = expr();
            expect(")");
        }
        node->rhs->rhs = stmt();
        return node;
    }

    if(consume_tokkind(TK_RETURN)){
        node = new_binary(ND_RETURN, expr(), NULL);
    }else{
        node = expr();
    }
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
    if(consume("&")){
        return new_binary(ND_ADDR, unary(), NULL);
    }
    if(consume("*")){
        return new_binary(ND_DEREF, unary(), NULL);
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
        if(consume("(")){
            Node *node = new_node(ND_CALL);
            node->name = tok->str;
            node->len = tok->len;
            node->args = new_args();
            return node;
        }

        Node *node = new_node(ND_LVAR);
        
        LVar *lvar = find_lvar(tok);
        if(lvar){
            node->offset = lvar->offset;
            node->ty = lvar->type;
        }else{
            error_at(tok->str, "variable is not declarated");
        }
        return node;
    }
    return new_num(expect_number());
}

Node** parse(Token *tok){
    token = tok;
    
    program();
    return code;
}