// set.h
#ifndef SET_H
#define SET_H

#include <stdbool.h>
#include "hashmap.h"

typedef struct {
    hm* map;
} set;

set* set_new(void);
void set_free(set* s);

bool set_add(set* s, const char* key);
bool set_contains(set* s, const char* key);

size_t set_size(set* s);

#endif
