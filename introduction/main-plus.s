.intel_syntax noprefix
.globl main, plus

plus:
    add rdi, rsi
    mov rax, rdi
    ret

main:
    mov rdi, 3
    mov rsi, 4
    call plus
    ret
