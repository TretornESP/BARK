#ifndef _ISET_H
#define _ISET_H
#include "vm_structs.h"

#define VM_ISET_INVALID    0x0
#define VM_ISET_HALT       0x1
#define VM_ISET_PUSH       0x2
#define VM_ISET_ADD        0x3
#define VM_ISET_SUB        0x4
#define VM_ISET_MUL        0x5
#define VM_ISET_DIV        0x6
#define VM_ISET_MOD        0x7
#define VM_ISET_AND        0x8
#define VM_ISET_OR         0x9
#define VM_ISET_XOR        0xA
#define VM_ISET_NOT        0xB
#define VM_ISET_SHL        0xC
#define VM_ISET_SHR        0xD
#define VM_ISET_JMP        0xE
#define VM_ISET_JZ         0xF
#define VM_ISET_CALL       0x10
#define VM_ISET_RET        0x11
#define VM_ISET_POP        0x12
#define VM_ISET_DUP        0x13
#define VM_ISET_SWAP       0x14
#define VM_ISET_LOAD       0x15
#define VM_ISET_STORE      0x16
#define VM_ISET_HOSTCALL   0x17
#define VM_ISET_HOSTWRITE  0x18
#define VM_ISET_HOSTREAD   0x19
#define VM_ISET_NOP        0x1A
#define VM_ISET_LOADS      0x1B
#define VM_ISET_STORES     0x1C

#define VM_ARG_INVALID     0x0
#define VM_ARG_IMMEDIATE   0x1
#define VM_ARG_STACK       0x2

struct vm_command* vm_get_command(uint8_t opcode);
#endif