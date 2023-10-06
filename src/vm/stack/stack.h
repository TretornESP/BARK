#ifndef _STACK_H
#define _STACK_H
#include <stdint.h>

struct stack {
    int64_t *data;
    uint32_t top;
    uint32_t size;
    void* vmid;
    void (*overflow)(struct stack*);
    void (*underflow)(struct stack*);
};

struct stack* stack_create(uint32_t size, void* vmid, void (*overflow)(struct stack*), void (*underflow)(struct stack*));
struct stack* stack_duplicate(struct stack* s);
void stack_destroy(struct stack *s);
void stack_push(struct stack *s, int64_t data);
int64_t stack_pop(struct stack *s);
int64_t stack_peek(struct stack *s, uint32_t offset);
uint32_t stack_size(struct stack *s);
void stack_enter_frame(struct stack *s);
void stack_leave_frame(struct stack *s);
void* stack_getvmid(struct stack *s);
void stack_debug(struct stack *s);
#endif