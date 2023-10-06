#include "iset.h"
#include "frame.h"
#include "stack/stack.h"
#include <stdio.h>

#define GET_ISET_SIZE(x) (sizeof(x) / sizeof(struct vm_command))
#define ENSURE_ARGNO(x) ((argc != x) ? (vm->crash(vm, "ARGNO")) : ((void)0))
#define ENSURE(x) ENSURE_VM(); ENSURE_ARGNO(x);

int64_t _iset_inv(struct vm* vm, int64_t argc, int64_t* argv) {
    (void)argv;
    ENSURE(0);
    vm->running = 0;
    vm->crash(vm, "Invalid operation");
    return 0;
}

int64_t _iset_halt(struct vm* vm, int64_t argc, int64_t* argv) {
    (void)argv;
    ENSURE(0);
    vm->running = 0;
    return 0;
}

int64_t _iset_nop(struct vm* vm, int64_t argc, int64_t* argv) {
    (void)argv;
    (void)argc;
    (void)vm;
    ENSURE(0);
    return 0;
}

int64_t _iset_push(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(1);
    return argv[0];
}

int64_t _iset_add(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] + argv[1];
}

int64_t _iset_sub(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] - argv[1];
}

int64_t _iset_mul(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] * argv[1];
}

int64_t _iset_div(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] / argv[1];
}

int64_t _iset_mod(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] % argv[1];
}

int64_t _iset_and(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] & argv[1];
}

int64_t _iset_or(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] | argv[1];
}

int64_t _iset_xor(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] ^ argv[1];
}

int64_t _iset_not(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(1);
    return ~argv[0];
}

int64_t _iset_shl(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] << argv[1];
}

int64_t _iset_shr(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    return argv[0] >> argv[1];
}

int64_t _iset_jmp(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(1);
    vm->ip = argv[0];
    return 0;
}

int64_t _iset_jz(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    vm->ip = (!argv[0]) ? argv[1] : vm->ip;
    return 0;
}

int64_t _iset_call(struct vm* vm, int64_t argc, int64_t* argv) {
    (void)argv;
    ENSURE(1);
    vm->fr = frame_enter_frame(vm->fr, vm->ip);
    vm->ip = argv[0];
    return 0;
}

int64_t _iset_ret(struct vm* vm, int64_t argc, int64_t* argv) {
    (void)argv;
    ENSURE(0);
    vm->ip = frame_get_return_address(vm->fr);
    vm->fr = frame_leave_frame(vm->fr);

    return 0;
}

int64_t _iset_pop(struct vm* vm, int64_t argc, int64_t* argv) {
    (void)argv;
    ENSURE(1);
    return 0;
}

int64_t _iset_dup(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(1);
    stack_push(vm->st, argv[0]);
    stack_push(vm->st, argv[0]);
    return 0;
}

int64_t _iset_swap(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    stack_push(vm->st, argv[0]);
    stack_push(vm->st, argv[1]);
    return 0;
}

int64_t _iset_load(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(1);
    return frame_get_local(vm->fr, argv[0]);
}

int64_t _iset_store(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    frame_set_local(vm->fr, argv[1], argv[0]);
    return 0;
}

int64_t _write_to_host(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(3);
    uint8_t* hostptr = (uint8_t*)argv[2];
    int64_t size = argv[0];

    if ((uint64_t)size > sizeof(int64_t)) return 0;
    
    for (int64_t i = 0; i < size; i++) {
        //printf("HOST[%lx] = (%x|%d|%c)\n", (uint64_t)(&hostptr[i]), (uint8_t)(argv[1] >> (i * 8)), (uint8_t)(argv[1] >> (i * 8)), (uint8_t)(argv[1] >> (i * 8)));
        hostptr[i] = (uint8_t)(argv[1] >> (i * 8));
    }

    return 0;
}

int64_t _read_from_host(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(2);
    uint8_t* hostptr = (uint8_t*)argv[1];
    int64_t size = argv[0];
    if ((uint64_t)size > sizeof(int64_t)) return 0;

    int64_t ret = 0;
    for (int64_t i = 0; i < size; i++) {
        ret |= (int64_t)hostptr[i] << (i * 8);
    }

    return ret;
}

int64_t _iset_hostcall(struct vm* vm, int64_t argc, int64_t* argv) {
    ENSURE(7);

    int64_t syscall = argv[0]; //RAX
    int64_t arg1 = argv[1];    //RDI
    int64_t arg2 = argv[2];    //RSI
    int64_t arg3 = argv[3];    //RDX
    int64_t arg4 = argv[4];    //R10
    int64_t arg5 = argv[5];    //R8
    int64_t arg6 = argv[6];    //R9

    int64_t ret = 0;
    //printf("syscall: %lu [RDI: %lx, RSI: %lx, RDX: %lx, R10: %lx, R8: %lx, R9: %lx]\n", syscall, arg1, arg2, arg3, arg4, arg5, arg6);

    __asm__ volatile(
        "movq %1, %%rax\n\t"
        "movq %2, %%rdi\n\t"
        "movq %3, %%rsi\n\t"
        "movq %4, %%rdx\n\t"
        "movq %5, %%r10\n\t"
        "movq %6, %%r8\n\t"
        "movq %7, %%r9\n\t"
        "syscall\n\t"
        "movq %%rax, %0\n\t"
        : "=r"(ret)
        : "r"(syscall), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5), "r"(arg6)
        : "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9");
    
    //printf("Result: %ld (%lx)\n", ret, ret);
    return ret;
}

struct vm_command vm_iset[] = {
    {
        .name = "INVALID",
        .opcode = VM_ISET_INVALID,
        .arg_sources = {VM_ARG_INVALID},
        .isize = 1,
        .phony = 1,
        .host = 0,
        .exec = _iset_inv
    },
    {
        .name = "HALT",
        .opcode = VM_ISET_HALT,
        .arg_sources = {VM_ARG_INVALID},
        .isize = 1,
        .phony = 1,
        .host = 0,
        .exec = _iset_halt
    },
    {
        .name = "PUSH",
        .opcode = VM_ISET_PUSH,
        .arg_sources = {VM_ARG_IMMEDIATE},
        .isize = 2,
        .phony = 0,
        .exec = _iset_push
    },
    {
        .name = "ADD",
        .opcode = VM_ISET_ADD,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_add
    },
    {
        .name = "SUB",
        .opcode = VM_ISET_SUB,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_sub
    },
    {
        .name = "MUL",
        .opcode = VM_ISET_MUL,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_mul
    },
    {
        .name = "DIV",
        .opcode = VM_ISET_DIV,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_div
    },
    {
        .name = "MOD",
        .opcode = VM_ISET_MOD,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_mod
    },
    {
        .name = "AND",
        .opcode = VM_ISET_AND,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_and
    },
    {
        .name = "OR",
        .opcode = VM_ISET_OR,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_or
    },
    {
        .name = "XOR",
        .opcode = VM_ISET_XOR,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_xor
    },
    {
        .name = "NOT",
        .opcode = VM_ISET_NOT,
        .arg_sources = {VM_ARG_STACK, VM_ARG_INVALID},
        .isize = 2,
        .phony = 0,
        .exec = _iset_not
    },
    {
        .name = "SHL",
        .opcode = VM_ISET_SHL,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_shl
    },
    {
        .name = "SHR",
        .opcode = VM_ISET_SHR,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 0,
        .host = 0,
        .exec = _iset_shr
    },
    {
        .name = "JMP",
        .opcode = VM_ISET_JMP,
        .arg_sources = {VM_ARG_IMMEDIATE, VM_ARG_INVALID},
        .isize = 2,
        .phony = 1,
        .host = 0,
        .exec = _iset_jmp
    },
    {
        .name = "JZ",
        .opcode = VM_ISET_JZ,
        .arg_sources = {VM_ARG_STACK, VM_ARG_IMMEDIATE},
        .isize = 3,
        .phony = 1,
        .host = 0,
        .exec = _iset_jz
    },
    {
        .name = "CALL",
        .opcode = VM_ISET_CALL,
        .arg_sources = {VM_ARG_IMMEDIATE, VM_ARG_INVALID},
        .isize = 2,
        .phony = 1,
        .host = 0,
        .exec = _iset_call
    },
    {
        .name = "RET",
        .opcode = VM_ISET_RET,
        .arg_sources = {VM_ARG_INVALID, VM_ARG_INVALID},
        .isize = 1,
        .phony = 1,
        .host = 0,
        .exec = _iset_ret
    },
    {
        .name = "POP",
        .opcode = VM_ISET_POP,
        .arg_sources = {VM_ARG_STACK, VM_ARG_INVALID},
        .isize = 2,
        .phony = 1,
        .host = 0,
        .exec = _iset_pop
    },
    {
        .name = "DUP",
        .opcode = VM_ISET_DUP,
        .arg_sources = {VM_ARG_STACK, VM_ARG_INVALID},
        .isize = 2,
        .phony = 1,
        .host = 0,
        .exec = _iset_dup
    },
    {
        .name = "SWAP",
        .opcode = VM_ISET_SWAP,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 1,
        .host = 0,
        .exec = _iset_swap
    },
    {
        .name = "LOAD",
        .opcode = VM_ISET_LOAD,
        .arg_sources = {VM_ARG_IMMEDIATE, VM_ARG_INVALID},
        .isize = 2,
        .phony = 0,
        .host = 0,
        .exec = _iset_load
    },
    {
        .name = "STORE",
        .opcode = VM_ISET_STORE,
        .arg_sources = {VM_ARG_STACK, VM_ARG_IMMEDIATE},
        .isize = 3,
        .phony = 1,
        .host = 0,
        .exec = _iset_store
    },
    {
        .name = "HOSTE",
        .opcode = VM_ISET_HOSTCALL,
        .arg_sources = {VM_ARG_IMMEDIATE, VM_ARG_STACK, VM_ARG_STACK, VM_ARG_STACK, VM_ARG_STACK, VM_ARG_STACK, VM_ARG_STACK},
        .isize = 8,
        .phony = 0,
        .host = 1,
        .exec = _iset_hostcall
    },
    {
        //HOSTW <size> <value> <hostptr>
        .name = "HOSTW",
        .opcode = VM_ISET_HOSTWRITE,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK, VM_ARG_STACK},
        .isize = 4,
        .phony = 1,
        .host = 0,
        .exec = _write_to_host
    },
    {
        .name = "HOSTR",
        .opcode = VM_ISET_HOSTREAD,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 1,
        .host = 1,
        .exec = _read_from_host
    },
    {
        .name = "NOP",
        .opcode = VM_ISET_NOP,
        .arg_sources = {VM_ARG_INVALID},
        .isize = 1,
        .phony = 1,
        .host = 0,
        .exec = _iset_nop
    },
    {
        .name = "LOADS",
        .opcode = VM_ISET_LOADS,
        .arg_sources = {VM_ARG_STACK, VM_ARG_INVALID},
        .isize = 2,
        .phony = 0,
        .host = 0,
        .exec = _iset_load
    },
    {
        .name = "STORES",
        .opcode = VM_ISET_STORES,
        .arg_sources = {VM_ARG_STACK, VM_ARG_STACK},
        .isize = 3,
        .phony = 1,
        .host = 0,
        .exec = _iset_store
    }
};

struct vm_command* vm_get_command(uint8_t opcode)
{
    for (uint32_t i = 0; i < GET_ISET_SIZE(vm_iset); i++) {
        if (vm_iset[i].opcode == opcode) {
            return &vm_iset[i];
        }
    }
    return VM_ISET_INVALID;
}