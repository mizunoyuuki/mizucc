// 現状の加減乗除演算 | 比較演算 プログラムの文法
// expr       = equaloity
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = NUM | "(" expr ")"

#include<stdio.h>
#include"mcc.h"

Token *tokenize(char *);
Node *expr(void);
void gen(Node *);
void program(void);

// 現在のtokenの外部変数
Token *token;
Node *code[100];

int main(int argc, char *argv[]){
        if (argc != 2){
                error("引数の個数が正しくありません。");
                return 1;
        }

        token = tokenize(argv[1]);
	program();

        printf(".intel_syntax noprefix\n");
        printf(".globl main\n");
        printf("main: \n");

	// プロローグ
	// 変数26個数分の領域を確保する
	//
	printf("    push rbp\n");
	printf("    mov rbp, rsp\n");
	printf("    sub rsp, 208\n");

	//先頭の式から
	for (int i=0; code[i]; i++){
		gen(code[i]);

		// 式の評価結果としてスタックに一つの値が残っている
		// はずなので、スタックが煽れないようにpopしておく
		printf("    pop rax\n");
	}

	// エピローグ
	// 最後の式の結果がRAXに残っているので、それが返り血になる
	printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");

        return 0;
}
