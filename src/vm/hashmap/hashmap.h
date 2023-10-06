#ifndef _HASHMAP_H
#define _HASHMAP_H
#include <stdint.h>

struct hashmap_item {
    void* data;
    uint64_t size;
    uint64_t key;
};

struct hashmap {
    struct hashmap_item * items;
    uint32_t size;
} __attribute__((packed));

struct hashmap * create_hashmap(uint32_t size);
void delete_hashmap(struct hashmap * hman);
struct hashmap * duplicate_hashmap(struct hashmap* hmap);
uint64_t debug_get_hash(struct hashmap_item * hman);
uint64_t insert(struct hashmap * hman, struct hashmap_item * item);
int retrieve(struct hashmap * hman, struct hashmap_item * item, uint64_t key);
void dump_hashmap(struct hashmap * hman);
#endif