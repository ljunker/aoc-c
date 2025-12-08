#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
    const char* key;
    void* value;
} hm_entry;

struct hm {
    hm_entry* entries;
    size_t capacity;
    size_t length;
};

#define INITIAL_CAPACITY 16

hm* hm_create(void) {
    hm* map = malloc(sizeof(hm));
    if (map == NULL) {
        return NULL;
    }
    map->capacity = INITIAL_CAPACITY;
    map->length = 0;
    map->entries = calloc(map->capacity, sizeof(hm_entry));
    if (map->entries == NULL) {
        free(map);
        return NULL;
    }
    return map;
}

void hm_destroy(hm* map) {
    for (size_t i = 0; i < map->capacity; i++) {
        free((void*)map->entries[i].key);
    }
    free(map->entries);
    free(map);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void* hm_get(hm* map, const char* key) {
    const uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(map->capacity-1));

    while (map->entries[index].key != NULL) {
        if (strcmp(key, map->entries[index].key) == 0) {
            return map->entries[index].value;
        }
        index++;
        if (index >= map->capacity) {
            index = 0;
        }
    }
    return NULL;
}

static const char* hm_set_entry(hm_entry* entries, size_t capacity, const char* key, void* value, size_t* plength) {
    const uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity-1));

    while(entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            entries[index].value = value;
            return entries[index].key;
        }
        index++;
        if (index >= capacity) {
            index = 0;
        }
    }

    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    return key;
}

static bool hm_expand(hm* map) {
    const size_t new_capacity = map->capacity * 2;
    if (new_capacity < map->capacity) {
        return false;
    }
    hm_entry* new_entries = calloc(new_capacity, sizeof(hm_entry));
    if (new_entries == NULL) {
        return false;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        const hm_entry entry = map->entries[i];
        if (entry.key != NULL) {
            hm_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
        }
    }

    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;
    return true;
}

const char* hm_set(hm* map, const char* key, void* value) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    if (map->length >= map->capacity / 2) {
        if (!hm_expand(map)) {
            return NULL;
        }
    }
    return hm_set_entry(map->entries, map->capacity, key, value, &map->length);
}

size_t hm_length(hm* map) {
    return map->length;
}

hmiterator hm_iterator(hm* map) {
    hmiterator it;
    it._map = map;
    it._index = 0;
    return it;
}

bool hm_next(hmiterator* it) {
    const hm* map = it->_map;
    while (it->_index < map->capacity) {
        const size_t i = it->_index;
        it->_index++;
        if (map->entries[i].key != NULL) {
            const hm_entry entry = map->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}
