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

bool is_alphabet(char p){
	return (p >= 'a' && p <= 'z') || (p >= 'A' && p <= 'Z') || p == '_';
}

bool is_return(char *p){
	return !memcmp("return", p, 6) && (*(p+6) == ' ');
}

bool is_if(char *p){
	return !memcmp(p, "if", 2) && ( (*(p+2) == ' ') || (*(p+2) == '(') );
}

bool is_else(char *p){
	return !memcmp(p, "else", 4) && !is_alphabet(*(p+4));
}

bool is_while(char *p){
	return !memcmp(p, "while", 5) && !is_alphabet(*(p+5));
}

bool is_for(char *p){
	return !memcmp(p, "for", 3) && !is_alphabet(*(p+3));
}

// 入力文字列pをトークナイズしてそれを返す。
// 現在の文法
// program      = stmt*
// stmt         = expr ";"
//              | "{" stmt* "}"
//              | "if" "(" expr ")" stmt ("else" expr)?
//              | "while" "(" expr ")" stmt
//              | "for" "(" expr? ";" expr? ";" expr ")" stmt
//              | "return" expr ";"
// expr         = assign
// assign       = equality ("=" assign)?
// equality     = relational ("==" relational | "!=" relational)*
// relational   = add ("<" add | "<=" add | ">" add | ">=" add)*
// add          = mul ("+" mul | "-" mul)*
// mul          = unary ("*" unary | "/" unary)
// unary        = ("+" | "-")? primary
// primary      = num | ident | "(" expr ")"

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

		if (*p == '{' || *p == '}'){
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (is_if(p)){
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}

		if (is_else(p)){
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		if (is_while(p)){
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}

		if (is_for(p)){
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}

		if (is_return(p)){
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if (is_alphabet(*p)){
			char *q = p;
			int len = 0;

	                while (is_alphabet(*p)){
				p++;
				len += 1;
			}

			cur = new_token(TK_IDENT, cur, q, len);
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
