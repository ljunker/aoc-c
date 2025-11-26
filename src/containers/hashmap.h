#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct hm hm;

hm* hm_create(void);

void hm_destroy(hm* map);

void* hm_get(hm* map, const char* key);

const char* hm_set(hm* map, const char* key, void* value);

size_t hm_length(hm* map);

typedef struct {
    const char* key;
    void* value;

    hm* _map;
    size_t _index;
} hmiterator;

hmiterator hm_iterator(hm* map);

#endif // HASHMAP_H