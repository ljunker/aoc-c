#include "containers/stack.h"

#include <stdlib.h>
#include <stdio.h>

struct stack {
    void** data;
    size_t len;
    size_t cap;
};

#define STACK_INITIAL_CAP 16

static void stack_ensure_capacity(stack* s, size_t min_cap) {
    if (!s) return;
    if (s->cap >= min_cap) return;

    size_t new_cap = (s->cap > 0) ? s->cap : STACK_INITIAL_CAP;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }

    void** new_data = realloc(s->data, new_cap * sizeof(void*));
    if (!new_data) {
        fprintf(stderr, "stack_ensure_capacity: out of memory\n");
        abort();
    }

    s->data = new_data;
    s->cap = new_cap;
}

stack* stack_new(void) {
    stack* s = malloc(sizeof(stack));
    if (!s) {
        fprintf(stderr, "stack_new: out of memory\n");
        abort();
    }

    s->len = 0;
    s->cap = 0;
    s->data = NULL;

    stack_ensure_capacity(s, STACK_INITIAL_CAP);
    return s;
}

void stack_free(stack* s) {
    if (!s) return;
    free(s->data);
    free(s);
}

void stack_push(stack* s, void* item) {
    if (!s) return;
    if (s->len >= s->cap) {
        stack_ensure_capacity(s, s->len + 1);
    }
    s->data[s->len++] = item;
}

void* stack_pop(stack* s) {
    if (!s) return NULL;
    if (s->len == 0) return NULL;
    s->len--;
    return s->data[s->len];
}

bool stack_empty(stack* s) {
    if (!s) return true;
    return s->len == 0;
}

size_t stack_size(stack* s) {
    if (!s) return 0;
    return s->len;
}
