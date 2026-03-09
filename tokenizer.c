#include"mcc.h"

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
        Token *tok = calloc(1, sizeof(Token));
        tok->kind = kind;
        tok->str = str;
        tok->len = len;
        cur->next = tok;
        return tok;
}

bool startswith(char *p, char *q){
        return memcmp(p, q, strlen(q)) == 0;
}

//
// 入力文字列pをトークナイズしてそれを返す。
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p){
		if (isspace(*p)){
			p++;
			continue;
		}
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == ';'){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}
		if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=") ){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p+=2;
			continue;
		}
		if (*p == '<' || *p == '>' || *p == '='){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (*p >= 'a' && *p <= 'z'){
			cur = new_token(TK_IDENT, cur, p++, 1);
			continue;
		}

		if (isdigit(*p)){
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p-q;
			continue;
		}

		error("トークナイズできません。");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}
