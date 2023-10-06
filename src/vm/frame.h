#ifndef _FRAME_H
#define _FRAME_H
#include <stdint.h>

#include "hashmap/hashmap.h"

struct frame {
    uint32_t return_address;
    struct hashmap * locals;
    uint32_t max_local_vars;
    struct frame * next;
    struct frame * prev;
};

struct frame * frame_create(struct frame* father, uint32_t return_address, uint32_t num_locals);
void frame_destroy(struct frame * frame);

struct frame * frame_duplicate(struct frame* frame);
void frame_set_local(struct frame * frame, int64_t key, int64_t value);
int64_t frame_get_local(struct frame * frame, int64_t key);
uint32_t frame_get_return_address(struct frame * frame);
void frame_debug(struct frame * frame);

struct frame* frame_enter_frame(struct frame * frame, uint32_t return_address);
struct frame* frame_leave_frame(struct frame * frame);
#endif