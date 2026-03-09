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

// 関数の宣言
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();
bool consume(char*);
void expect(char*);
int expect_number();

Node *expr(){
	return equality();
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
};

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
};

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
};


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

	return new_node_num(expect_number());
}

/* *******************  抽象構文木の再帰下降構文木   ******************** */

// 次のトークンが期待している記号の時は、トークンを一つ読み進める。
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op){
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;
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

bool at_eof(){
	return token->kind == TK_EOF;
}

