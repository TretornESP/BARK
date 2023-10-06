#ifndef _VM_H
#define _VM_H
#include "vm_structs.h"
#include <stdint.h>

struct vm* vm_create(int64_t* program, uint32_t memsize, uint32_t stacksize, uint32_t maxvars, void (*crash)(struct vm*, char *));
struct vm* vm_duplicate(struct vm* vm);
void vm_destroy(struct vm *vm);
void vm_step(struct vm *vm);
void vm_run(struct vm* vm, void (*breakpoint)(struct vm*));
uint8_t vm_running(struct vm *vm);
int64_t vm_result(struct vm *vm);
void vm_debug(struct vm *vm, uint8_t crashed);
#endif