#include<stdio.h>
#include<stdlib.h>

// 目標
// 13+25-4のような加減算をコンパイルできる(空白はなし)
int main(int argc, char **argv){
	if (argc != 2){
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	char *p = argv[1];

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	int sign;
	if (*p == '-'){
		p++;
		sign = -1;
	} else {
		sign = 1;
	}

	printf("    mov rax, %ld\n", strtol(p, &p, 10) * sign);

	while(*p){
		if (*p == '+'){
			p++;
			printf("    add rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		if (*p == '-'){
			p++;
			printf("    sub rax, %ld\n", strtol(p, &p, 10));
			continue;
		}

		fprintf(stderr, "予期しない文字です %c\n", *p);
		return 1;
	}

	printf("    ret\n");
	return 0;
}

