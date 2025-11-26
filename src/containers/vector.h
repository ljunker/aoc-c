// vector.h
#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct {
    size_t len;
    size_t cap;
    void **data;
} vec;

vec* vec_new(void);
void vec_free(vec* v);

void vec_push(vec* v, void* item);
void* vec_get(vec* v, size_t index);

void vec_set(vec* v, size_t index, void* item);
void vec_reserve(vec* v, size_t new_cap);

#endif
