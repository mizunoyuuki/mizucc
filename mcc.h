#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>
#include<stdarg.h>
#include<string.h>

// tokenの種類
typedef enum {
	TK_RESERVED,  // 記号
	TK_IDENT,     // 識別子
	TK_RETURN,    // returnキーワード
        TK_IF,        // if
	TK_ELSE,      // else
	TK_WHILE,     // while
        TK_FOR,       // for
	TK_NUM,       // 整数トークン
	TK_EOF        // 入力の終わりを表すトークン
} TokenKind;


// Token型の定義
// 演算子が文字列の場合もあるため、lenを追加する
typedef struct Token Token;
struct Token {
	TokenKind kind; Token *next; int val;
	char *str;
	int len;
};

// Node型のNodeKindの列挙型
typedef enum {
	ND_ADD,      // +
	ND_SUB,      // -
	ND_MUL,      // *
	ND_DIV,      // /
	ND_EQ,       // ==
	ND_NE,       // !=
	ND_LT,       // <
	ND_LE,       // <=
	ND_GT,       // >
	ND_GE,       // >=
	ND_ASSIGN,   // =(代入)
	ND_LVAR,     // ローカル変
        ND_RETURN,   // return
        ND_IF,       // if
	ND_WHILE,    // while
	ND_FOR,      // for
	ND_BLOCK,    // {} ブロック
	ND_FUNCALL,  // 関数呼び出し
	ND_NUM,      // 整数
} NodeKind;

// Node型
typedef struct Node Node;

struct Node {
	NodeKind kind; // ノードの型
	Node *lhs;     // 左辺
	Node *rhs;     // 右辺
        
	// if文 Node
	Node *cond;   // ifの条件式
	Node *then;    // ifのthen節
	Node *els;     // ifのelse節
		       //
		       //
        // for文 Node
	Node *finit;    // 初期化(expr?)
	Node *fcond;    // 条件式(expr?)
	Node *finc;     // 更新式(expr?)
	Node *fthen;    // ループの中身(stmt)
		
	// Block(複文) / Node構造体を連結リストとして使えるように追加
	Node *next;
	Node *body;

	// 関数呼び出し
	char *funcname;
	int funcname_len;


	int val;       // kindがND_NUMの場合のみ扱う
        int offset;    // kindがND_LVARの場合のみ使う
};

// LVar型 ローカル変数の型
typedef struct LVar LVar;

struct LVar {
	LVar *next;   // 次の変数かNULL
	char *name;   // 変数名
	int len;      // 変数の長さ
	int offset;   // RBPからアクセスするためのオフセット値
};


// 外部変数の宣言
extern Token *token;
extern Node *code[100];
extern LVar *locals;

extern void error(char*, ...);
