#include "containers/set.h"

#include <stdlib.h>
#include <stdio.h>

set* set_new(void) {
    set* s = malloc(sizeof(set));
    if (!s) {
        fprintf(stderr, "set_new: out of memory\n");
        abort();
    }

    s->map = hm_create();
    if (!s->map) {
        fprintf(stderr, "set_new: failed to create hashmap\n");
        free(s);
        abort();
    }

    return s;
}

void set_free(set* s) {
    if (!s) return;
    hm_destroy(s->map);
    free(s);
}

bool set_add(set* s, const char* key) {
    if (!s || !key) return false;

    const void* existing = hm_get(s->map, key);
    if (existing != NULL) {
        return false;
    }

    const char* stored_key = hm_set(s->map, key, (void*)1);
    if (!stored_key) {
        fprintf(stderr, "set_add: hm_set failed\n");
        abort();
    }

    return true;
}

bool set_contains(set* s, const char* key) {
    if (!s || !key) return false;
    return hm_get(s->map, key) != NULL;
}

size_t set_size(set* s) {
    if (!s) return 0;
    return hm_length(s->map);
}
