#ifndef _VM_STRUCTS_H
#define _VM_STRUCTS_H

#include "stack/stack.h"
#include "frame.h"
#include <stdint.h>
#include <stdlib.h>

#define ENSURE_VM() ((vm == 0) ? (exit(1)) : ((void)0))

#define VM_CMD_MAXNAME     0x8
#define VM_CMD_MAXARGS     0x8

struct vm {
   int64_t *memory;
   uint32_t memsize;
   uint32_t ip;
   uint8_t running;
   struct stack *st;
   struct frame *fr;
   void (*crash)(struct vm*, char *);
};

struct vm_command {
   char name[VM_CMD_MAXNAME];
   uint8_t opcode;
   uint8_t arg_sources[VM_CMD_MAXARGS];
   uint8_t isize;
   uint8_t phony;
   uint8_t host;
   int64_t (*exec)(struct vm*, int64_t, int64_t*);
};

#endif