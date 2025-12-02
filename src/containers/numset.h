#ifndef NUMSET_H
#define NUMSET_H

#include <stdbool.h>
#include <stddef.h>

typedef struct numset numset;

numset* numset_new(void);

void numset_free(numset* s);

bool numset_add(numset* s, long long value);

bool numset_contains(numset* s, long long value);

size_t numset_size(numset* s);

typedef struct {
    numset* _set;
    size_t _index;
} numset_iter;

numset_iter numset_iterator(numset* s);
bool numset_next(numset_iter* it, long long* out_value);

#endif
