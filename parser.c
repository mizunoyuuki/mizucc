#include"mcc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

// 現在の文法
// program    = stmt*
// stmt       = expr ";"
//            | "if" "(" expr ")" stmt ( "else" stmt )?
//            | "while" "(" expr ")" stmt
//            | "for" "(" expr? ";" expr? ";" expr?  ")" stmt
//            | "return" expr ";"
// expr       = assign
// assign     = equality ("=" assign)?
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | ident | "(" expr ")"
//
// identを使った
// ident = ident | num;
// ident = num + ident;

// 関数の宣言
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();
bool consume(char*);
void expect(char*);
int expect_number();
bool at_eof();
LVar *find_lvar(Token *);

Token *consume_ident(void);
bool consume_return(char *op);
bool consume_if(char *op);
bool consume_else(char *op);
bool consume_while(char *op);
bool consume_for(char *op);

void program(){
	int i = 0;
	while (!at_eof()){
		code[i++] = stmt();
	}

	code[i] = NULL;
}

Node *stmt(){
	Node *node;

	if (consume_return("return")){ node = calloc(1, sizeof(Node)); node->kind = ND_RETURN;
		node->lhs = expr();
		expect(";");
		return node;
	}

	if (consume_if("if")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_IF;
		expect("(");
		node->cond = expr();
		expect(")");
		node->then = stmt();
		if(consume_else("else")){
			node->els = stmt();
		}
		return node;
	}

	// node->blockは、Node型のポインタの配列となっている
	// Node **block;
	if (consume("{")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;
		node->blocks = calloc(100, sizeof(Node*));
		Node **blocks = node->blocks;

		int i = 0;
		while (!consume("}")){
			blocks[i] = stmt();
			i += 1;
		}
		node->block_len = i;
		return node;
	}

	if (consume_while("while")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;
		expect("(");
		node->lhs = expr();
		expect(")");
		node->rhs = stmt();
		return node;
	}

	if (consume_for("for")){
		node = calloc(1, sizeof(Node));
		node->kind = ND_FOR;
		expect("(");
		if (!consume(";")){
			node->finit = expr();
			expect(";");
		}

		if (!consume(";")){
			node->fcond = expr();
			expect(";");
		}

		if (!consume(")")){
			node->finc = expr();
			expect(")");
		}
		node->fthen = stmt();
		return node;
	}

	node = expr();
	expect(";");

	return node;
}


Node *expr(){
	return assign();
}

Node *assign(){
	Node *node = equality();

	if (consume("=")){
		node = new_node(ND_ASSIGN, node, assign());
	}
	return node;
}


Node *equality(){
	Node *node = relational();

	for(;;){
		if (consume("==")){
			node = new_node(ND_EQ, node, relational());
		} else if(consume("!=")){
			node = new_node(ND_NE, node, relational());
		} else {
			return node;
		}
	}
}

Node *relational(){
	Node *node = add();

	for (;;){
		if (consume("<"))
			node = new_node(ND_LT, node, add());
		else if (consume(">"))
			node = new_node(ND_GT, node, add());
		else if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if (consume(">="))
			node = new_node(ND_GE, node, add());
		else
			return node;
	}
}

Node *add(){
	Node *node = mul();

	for(;;){
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}


Node *mul(){
	Node *node = unary();

	for(;;){
		if (consume("*")){
			node = new_node(ND_MUL, node, unary());
		} else if (consume("/")) {
			node = new_node(ND_DIV, node, unary());
		} else {
			return node;
		}
	}
}

Node *unary(){
	if (consume("+"))
		return primary();
	if (consume("-"))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

Node *primary(){
	if (consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_ident();

	if (tok){
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;

		LVar *lvar = find_lvar(tok);
		if (lvar){
			node->offset = lvar->offset;
		} else {
			lvar = calloc(1, sizeof(LVar));
			lvar->next = locals;
			lvar->name = tok->str;
			lvar->len = tok->len;
			lvar->offset = locals ? locals->offset + 8 : 8;
			node->offset = lvar->offset;
			locals = lvar;
		}
		return node;
	}

	return new_node_num(expect_number());
}

// 変数を名前で検索する。見つからなかったらNULLを返す。
LVar *find_lvar(Token *tok){
	for (LVar *var = locals; var; var = var->next){
		if (var->len == tok->len &&  !memcmp(tok->str, var->name, var->len))
			return var;
	}

	return NULL;
}

// 次のトークンが期待している記号の時は、トークンを一つ読み進める。
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op){
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

bool consume_return(char *op){
	if (token->kind != TK_RETURN || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;

	token = token->next;
	return true;
}

bool consume_if(char *op){
	if (token->kind != TK_IF || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

bool consume_else(char *op){
	if (token->kind != TK_ELSE || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;

	token = token->next;
	return true;
}

bool consume_while(char *op){
	if (token->kind != TK_WHILE || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

bool consume_for(char *op){
	if (token->kind != TK_FOR || strlen(op) != token->len || memcmp(token->str, op, token->len)){
		return false;
	}

	token = token->next;
	return true;
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op){
	if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
		error("'%c'ではありません。", op);
	token = token->next;
}


// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す
// それ以外の場合はエラーを報告する。
int expect_number() {
	if (token->kind != TK_NUM )
		error("数ではありません。");
	int val = token->val;
	token = token->next;
	return val;
}

Token *consume_ident(){
	if (token->kind == TK_IDENT) {
		Token *tok = token;
		token = token->next;
		return tok;
	}
	return NULL;
}

bool at_eof(){
	return token->kind == TK_EOF;
}
