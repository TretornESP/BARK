#include "vm.h"
#include "stack/stack.h"
#include "frame.h"
#include "iset.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//Callbacks

void _vm_stack_overflow(struct stack *s)
{
    struct vm *vm = stack_getvmid(s);
    if (!vm) return;
    vm->running = 0;
    vm->crash(vm, "Stack overflow");
}

void _vm_stack_underflow(struct stack *s)
{
    struct vm *vm = stack_getvmid(s);
    if (!vm) return;
    vm->running = 0;
    vm->crash(vm, "Stack underflow");
}

//Internal functions

int64_t _vm_nexti(struct vm *vm)
{
    ENSURE_VM();
    if (vm->ip >= vm->memsize) return 0;
    return vm->memory[vm->ip++];
}

struct vm* _vm_sandbox(struct vm* current) {
    struct vm *vm = malloc(sizeof(struct vm));
    vm->memory = calloc(sizeof(int64_t), current->memsize);
    memcpy(vm->memory, current->memory, sizeof(int64_t) * current->memsize);
    vm->memsize = current->memsize;
    vm->ip = current->ip;
    vm->running = current->running;
    vm->st = stack_duplicate(current->st);
    vm->fr = frame_duplicate(current->fr);
    vm->crash = current->crash;
    return vm;
}

void _vm_destroy(struct vm *vm)
{
    if (!vm) return;
    if (vm->memory) free(vm->memory);
    if (vm->st) stack_destroy(vm->st);
    if (vm->fr) frame_destroy(vm->fr);
    free(vm);
}

void _vm_decodei(struct vm* vm, int64_t inst)
{
    struct vm_command* vi = vm_get_command(inst);
    int64_t argc = vi->isize-1;

    int64_t * argv = calloc(sizeof(int64_t), argc);
    for (int64_t i = 0; i < argc; i++) {
        if (vi->arg_sources[i] == VM_ARG_STACK) {
            argv[i] = stack_pop(vm->st);
        } else if (vi->arg_sources[i] == VM_ARG_IMMEDIATE) {
            argv[i] = _vm_nexti(vm);
        } else {
            vm->crash(vm, "Invalid argument source");
        }
        //printf("ARGV[%lu] = %lx\n", i, argv[i]);
    }
    //printf("DECODE ENDED\n");
    int64_t result = vi->exec(vm, argc, argv);
    if (!vi->phony) stack_push(vm->st, result);
    free(argv);
}

void _vm_step(struct vm* vm)
{
    if (!vm) return;
    if (!vm->running) return;

    if (vm->ip >= vm->memsize) vm->crash(vm, "Exec out of bounds");
    int64_t inst = _vm_nexti(vm);
    _vm_decodei(vm, inst);
}

void _vm_printi(struct vm* vm, int64_t ip)
{
    struct vm_command* vi = vm_get_command(vm->memory[ip]);
    int64_t argc = vi->isize-1;
    uint32_t current_ip = ip+1;

    printf("(");
    for (uint32_t i = ip; i < current_ip; i++) printf("%016lx ", vm->memory[i]);
    printf("\b)");

    int64_t * argv = calloc(sizeof(int64_t), argc);
    for (int64_t i = 0; i < argc; i++) {
        if (vi->arg_sources[i] == VM_ARG_STACK) {
            argv[i] = stack_peek(vm->st, i);
        } else if (vi->arg_sources[i] == VM_ARG_IMMEDIATE) {
            argv[i] = vm->memory[current_ip++];
        } else {
            vm->crash(vm, "Invalid argument source on printi");
        }
    }

    int64_t result = 0;
    if (!vi->host) {
        struct vm* sandbox = _vm_sandbox(vm);
        result = vi->exec(sandbox, argc, argv);
        vm_destroy(sandbox);
    }
        
    printf(" %s (", vi->name);
    for (int64_t i = 0; i < argc; i++) {
        printf("%ld", argv[i]);
        if (i < argc-1) printf(", ");
    }
    if (!vi->host) {
        printf(") = %ld ", result);
    } else {
        printf(") = [CANNOT SANDBOX HOST COMMANDS] ");
    }
    if (!vi->phony) printf("(pushed to stack)");
    printf("\n");
    free(argv);    
}

//Core functions

struct vm* vm_create(int64_t* program, uint32_t memsize, uint32_t stacksize, uint32_t maxvars, void (*crash)(struct vm*, char* str))
{
    struct vm *vm = malloc(sizeof(struct vm));
    vm->memory = calloc(sizeof(int64_t), memsize);
    memcpy(vm->memory, program, sizeof(int64_t) * memsize);
    vm->memsize = memsize;
    vm->ip = 0;
    vm->running = 0;
    vm->st = stack_create(stacksize, (void*)vm, _vm_stack_overflow, _vm_stack_underflow);
    vm->fr = frame_create(0x0, 0x0, maxvars);
    vm->crash = crash;

    return vm;
}

void vm_destroy(struct vm *vm)
{
    _vm_destroy(vm);
}

void vm_run(struct vm* vm, void (*breakpoint)(struct vm*)) {
    if (!vm) return;
    vm->running = 1;
    while (vm->running) {
        if (breakpoint) breakpoint(vm);
        _vm_step(vm);
    }
}

int64_t vm_result(struct vm *vm) {
    ENSURE_VM();
    vm->running = 0;
    return stack_pop(vm->st);
}

uint8_t vm_running(struct vm *vm) {
    ENSURE_VM();
    return vm->running;
}

void vm_debug(struct vm *vm, uint8_t crashed) {
    if (!vm) return;
    printf("VM [IP: %d | Running %d]\n", vm->ip, vm->running);
    stack_debug(vm->st);
    frame_debug(vm->fr);
    if (!crashed) _vm_printi(vm, vm->ip);
    else
    {
        printf("Dumping memory:\n");
        for (uint32_t i = 0; i < vm->memsize; i++) {
            printf("%016lx ", vm->memory[i]);
            if (i % 8 == 7) printf("\n");
        }
    }
}

struct vm* vm_duplicate(struct vm* vm)
{
    return _vm_sandbox(vm);
}