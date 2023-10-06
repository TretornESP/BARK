#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct stack* stack_create(uint32_t size, void* vmid, void (*overflow)(struct stack*), void (*underflow)(struct stack*))
{
    struct stack *s = malloc(sizeof(struct stack));
    s->data = calloc(sizeof(int64_t), size);
    s->top = 0;
    s->size = size;
    s->vmid = vmid;
    s->overflow = overflow;
    s->underflow = underflow;
    return s;
}

struct stack* stack_duplicate(struct stack* s) 
{
    struct stack* new = stack_create(s->size, s->vmid, s->overflow, s->underflow);
    new->top = s->top;
    for (uint32_t i = 0; i < s->top; i++) {
        new->data[i] = s->data[i];
    }
    return new;
}

void stack_destroy(struct stack *s)
{
    if (!s) return;
    if (s->data) free(s->data);
    free(s);
}

void stack_push(struct stack *s, int64_t data)
{
    if (!s) return;
    if (s->top >= s->size) {
        s->overflow(s);
        return;
    }

    s->data[s->top++] = data;
}

int64_t stack_pop(struct stack *s)
{
    if (!s) return 0;
    if (s->top == 0) {s->underflow(s); return 0;}
    
    return s->data[--s->top];
}

int64_t stack_peek(struct stack *s, uint32_t offset)
{
    if (!s) return 0;
    if (s->top == 0) {s->underflow(s); return 0;}
    if (offset >= s->top) return 0;

    return s->data[s->top - offset - 1];
}

uint32_t stack_size(struct stack *s)
{
    if (!s) return 0;
    return s->top;
}

void* stack_getvmid(struct stack *s)
{
    if (!s) return 0;
    return s->vmid;
}

void stack_debug(struct stack *s) {
    if (!s) return;
    printf("Stack debug:\n");
    printf("\tVMID: %p, UNDERFLOW: %p, OVERFLOW: %p\n", s->vmid, (void*)(int64_t)s->underflow, (void*)(int64_t)s->overflow);
    printf("\tTOP: %d, SIZE: %d\n", s->top, s->size);
    printf("\tDATA: \n");
    for (uint32_t i = 0; i < s->top; i++) {
        printf("\t\t%d: %ld\n", i, s->data[i]);
    }
    printf("\n");
}