#include "frame.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void hmap_insert_adapter(struct hashmap * hmap, int64_t value, int64_t key) {
    struct hashmap_item * item = malloc(sizeof(struct hashmap_item));
    item->key = key;
    item->data = malloc(sizeof(int64_t));
    memcpy(item->data, &value, sizeof(int64_t));
    item->size = sizeof(int64_t);

    insert(hmap, item);
    free(item);
}

int64_t hmap_retrieve_adapter(struct hashmap * hmap, int64_t key) {
    struct hashmap_item * item = calloc(1, sizeof(struct hashmap_item));
    retrieve(hmap, item, key);
    if (item->size != sizeof(int64_t)) return 0;
    int64_t value = 0;
    memcpy(&value, item->data, sizeof(int64_t));
    free(item);
    return value;
}

struct frame * frame_create(struct frame* father, uint32_t return_address, uint32_t num_locals) {
    struct frame * frame = malloc(sizeof(struct frame));
    frame->return_address = return_address;
    frame->locals = create_hashmap(num_locals);
    frame->max_local_vars = num_locals;
    frame->next = 0;
    frame->prev = father;
    return frame;
}

void frame_destroy(struct frame * frame) {
    if (!frame) return;
    if (frame->locals) delete_hashmap(frame->locals);
    free(frame);
}

struct frame * frame_duplicate(struct frame* frame) {
    if (!frame) return 0;
    struct frame * new_frame = frame_create(frame->prev, frame->return_address, frame->max_local_vars);
    new_frame->locals = duplicate_hashmap(frame->locals);
    return new_frame;
}

void frame_set_local(struct frame * frame, int64_t key, int64_t value) {
    if (!frame) return;
    if (!frame->locals) return;
    hmap_insert_adapter(frame->locals, value, key);
}

int64_t frame_get_local(struct frame * frame, int64_t key) {
    if (!frame) return 0;
    if (!frame->locals) return 0;
    return hmap_retrieve_adapter(frame->locals, key);
}

void frame_debug(struct frame * frame) {
    if (!frame) return;
    printf("Frame [prev: %p next: %p]: \n", (void*)frame->prev, (void*)frame->next);
    printf("\treturn_address: %d\n", frame->return_address);
    printf("\tlocals: \n");
    dump_hashmap(frame->locals);
    printf("\n");
}

struct frame* frame_enter_frame(struct frame * frame, uint32_t return_address) {
    if (!frame) return 0;
    if (frame->next) return 0;

    frame->next = frame_create(frame, return_address, frame->max_local_vars);
    return frame->next;
}

struct frame* frame_leave_frame(struct frame * frame) {
    if (!frame) return 0;
    if (!frame->next) return 0;

    struct frame * prev = frame->prev;
    frame_destroy(frame);
    prev->next = 0;
    return prev;
}

uint32_t frame_get_return_address(struct frame * frame) {
    if (!frame) return 0;
    return frame->return_address;
}