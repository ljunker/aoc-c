#include "containers/vector.h"

#include <stdlib.h>
#include <stdio.h>

#define VEC_INITIAL_CAP 16

static void vec_ensure_capacity(vec* v, size_t min_cap) {
    if (!v) return;

    if (v->cap >= min_cap) {
        return;
    }

    size_t new_cap = (v->cap > 0) ? v->cap : VEC_INITIAL_CAP;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }

    void** new_data = realloc(v->data, new_cap * sizeof(void*));
    if (!new_data) {
        fprintf(stderr, "vec_ensure_capacity: out of memory\n");
        abort();
    }

    v->data = new_data;
    v->cap = new_cap;
}

vec* vec_new(void) {
    vec* v = malloc(sizeof(vec));
    if (!v) {
        fprintf(stderr, "vec_new: out of memory\n");
        abort();
    }

    v->len = 0;
    v->cap = 0;
    v->data = NULL;

    vec_ensure_capacity(v, VEC_INITIAL_CAP);
    return v;
}

void vec_free(vec* v) {
    if (!v) return;
    free(v->data);
    free(v);
}

void vec_reserve(vec* v, size_t new_cap) {
    if (!v) return;
    vec_ensure_capacity(v, new_cap);
}

void vec_push(vec* v, void* item) {
    if (!v) return;

    if (v->len >= v->cap) {
        vec_ensure_capacity(v, v->len + 1);
    }

    v->data[v->len++] = item;
}

void* vec_get(vec* v, size_t index) {
    if (!v) return NULL;
    if (index >= v->len) return NULL;
    return v->data[index];
}

void vec_set(vec* v, size_t index, void* item) {
    if (!v) return;

    if (index >= v->cap) {
        vec_ensure_capacity(v, index + 1);
    }

    if (index >= v->len) {
        for (size_t i = v->len; i < index; i++) {
            v->data[i] = NULL;
        }
        v->len = index + 1;
    }

    v->data[index] = item;
}
