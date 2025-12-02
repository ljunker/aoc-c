#include "numset.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    long long value;
    bool used;
} numset_entry;

struct numset {
    numset_entry* entries;
    size_t capacity;
    size_t length;
};

#define NUMSET_INITIAL_CAPACITY 16

static uint64_t hash_u64(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

static size_t next_power_of_two(size_t x) {
    if (x < 2) return 2;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
#if SIZE_MAX > 0xFFFFFFFFu
    x |= x >> 32;
#endif
    x++;
    return x;
}

static bool numset_expand(numset* s);

numset* numset_new(void) {
    numset* s = malloc(sizeof(numset));
    if (!s) {
        fprintf(stderr, "numset_new: out of memory\n");
        abort();
    }
    s->capacity = NUMSET_INITIAL_CAPACITY;
    s->length = 0;
    s->entries = calloc(s->capacity, sizeof(numset_entry));
    if (!s->entries) {
        fprintf(stderr, "numset_new: out of memory (entries)\n");
        free(s);
        abort();
    }
    return s;
}

void numset_free(numset* s) {
    if (!s) return;
    free(s->entries);
    free(s);
}

static bool numset_expand(numset* s) {
    size_t new_capacity = s->capacity * 2;
    new_capacity = next_power_of_two(new_capacity);
    numset_entry* new_entries = calloc(new_capacity, sizeof(numset_entry));
    if (!new_entries) {
        fprintf(stderr, "numset_expand: out of memory\n");
        return false;
    }

    for (size_t i = 0; i < s->capacity; i++) {
        if (s->entries[i].used) {
            long long v = s->entries[i].value;
            uint64_t h = hash_u64((uint64_t)v);
            size_t idx = (size_t)(h & (new_capacity - 1));

            while (new_entries[idx].used) {
                idx = (idx + 1) % new_capacity;
            }
            new_entries[idx].value = v;
            new_entries[idx].used = true;
        }
    }

    free(s->entries);
    s->entries = new_entries;
    s->capacity = new_capacity;
    return true;
}

bool numset_add(numset* s, long long value) {
    if (!s) return false;

    if (s->length * 2 >= s->capacity) {
        if (!numset_expand(s)) {
            return false;
        }
    }

    uint64_t h = hash_u64((uint64_t)value);
    size_t idx = (size_t)(h & (s->capacity - 1));

    while (s->entries[idx].used) {
        if (s->entries[idx].value == value) {
            return false;
        }
        idx = (idx + 1) % s->capacity;
    }

    s->entries[idx].value = value;
    s->entries[idx].used = true;
    s->length++;
    return true;
}

bool numset_contains(numset* s, long long value) {
    if (!s) return false;
    uint64_t h = hash_u64((uint64_t)value);
    size_t idx = (size_t)(h & (s->capacity - 1));

    while (s->entries[idx].used) {
        if (s->entries[idx].value == value) {
            return true;
        }
        idx = (idx + 1) % s->capacity;
    }
    return false;
}

size_t numset_size(numset* s) {
    if (!s) return 0;
    return s->length;
}

numset_iter numset_iterator(numset* s) {
    numset_iter it;
    it._set = s;
    it._index = 0;
    return it;
}

bool numset_next(numset_iter* it, long long* out_value) {
    if (!it || !it->_set || !out_value) return false;
    numset* s = it->_set;

    while (it->_index < s->capacity) {
        size_t i = it->_index++;
        if (s->entries[i].used) {
            *out_value = s->entries[i].value;
            return true;
        }
    }
    return false;
}
