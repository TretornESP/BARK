#include "hashmap.h"
#include "md5.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hashmap * create_hashmap(uint32_t size) {

    struct hashmap * hmap = (struct hashmap*)malloc(sizeof(struct hashmap));
    hmap->size = size;
    hmap->items = calloc(sizeof(struct hashmap_item), size);

    return hmap;
}

void delete_hashmap(struct hashmap * hmap) {
    if (!hmap) return;

    for (uint64_t i = 0; i < hmap->size; i++) {
        struct hashmap_item * item = &hmap->items[i];
        if (item && item->data) free(item->data);
    }

    free(hmap->items);
    free(hmap);
}

struct hashmap * duplicate_hashmap(struct hashmap* hmap) {
    if (!hmap) return 0;

    struct hashmap * new_hmap = create_hashmap(hmap->size);
    for (uint64_t i = 0; i < hmap->size; i++) {
        struct hashmap_item * item = &hmap->items[i];
        if (item->key) {
            insert(new_hmap, item);
        }
    }

    return new_hmap;
}

uint64_t calculate_hash(void * address, uint64_t size) {

    uint8_t * hash = (uint8_t*)malloc(16);
    MD5_Digest(hash, address, size);

    uint64_t hash_value = 0;
    for (int i = 0; i < 16; i++) {
        hash_value += hash[i];
    }

    free(hash);
    return hash_value;
}

uint64_t debug_get_hash(struct hashmap_item * item) {
    return calculate_hash(item->data, item->size);
}

uint64_t insert(struct hashmap * hmap, struct hashmap_item * new_item) {
    if (!hmap) return 0;
    if (!new_item) return 0;
    if (new_item->key == 0 || new_item->data == 0 || new_item->size == 0) {
        return 0;
    }
  
    uint64_t hash = calculate_hash(new_item->data, new_item->size);

    uint64_t index = hash % hmap->size;
    uint64_t original_index = index;
    
    struct hashmap_item * item = 0x0;
    uint8_t found = 0;
    for (uint64_t i = 0; i < hmap->size; i++) {
        item = &hmap->items[i];
        if (item && item->key && (item->key == new_item->key)) {
            found = 1;
            break;
        }
    }
    if (!found) {
        item = &hmap->items[index];

        while (item->key) {
            index++;
            if (index >= hmap->size) {
                index = 0;
            }
            if (index == original_index) {
                printf("HASHMAP: No free space in hashmap\n");
                return 0;
            }
            item = &hmap->items[index];
        }
    }
    //printf("Inserting item with key %ld at: %p\n", new_item->key, (void*)item);

    // Insert item, free old data if it exists
    item->key = new_item->key;
    if (item->data) free(item->data);
    item->data = malloc(new_item->size);
    memcpy(item->data, new_item->data, new_item->size);
    item->size = new_item->size;

    return hash;
}

int retrieve(struct hashmap * hmap, struct hashmap_item * response_item, uint64_t key) {
    uint64_t index = key % hmap->size;
    uint64_t original_index = index;
    
    struct hashmap_item * item = &hmap->items[index];
    while (item->key != key) {
        index++;
        if (index >= hmap->size) {
            index = 0;
        }
        if (index == original_index) {
            printf("HASHMAP: No item with key %lx\n", key);
            return 0;
        }
        item = &hmap->items[index];
    }

    response_item->key = item->key;
    response_item->data = item->data;
    response_item->size = item->size;

    return 1;
}

void dump_hashmap(struct hashmap * hman) {
    for (uint64_t i = 0; i < hman->size; i++) {
        struct hashmap_item * item = &hman->items[i];
        if (item->key) {
            printf("\t\t%lx: ", item->key);
            for (uint64_t j = 0; j < item->size; j++) {
                printf("%02x ", ((uint8_t*)item->data)[j]);
            }
            printf("\b\n");
        }

    }
}