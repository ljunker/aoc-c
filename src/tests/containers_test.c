#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "containers/vector.h"
#include "containers/queue.h"
#include "containers/stack.h"
#include "containers/set.h"
#include "containers/stringbuilder.h"
#include "containers/grid.h"
#include "containers/point.h"

static void test_vector(void) {
    vec* v = vec_new();
    assert(v != NULL);
    assert(v->len == 0);

    vec_push(v, (void*)(intptr_t)10);
    vec_push(v, (void*)(intptr_t)20);
    vec_push(v, (void*)(intptr_t)30);

    assert(v->len == 3);
    assert((intptr_t)vec_get(v, 0) == 10);
    assert((intptr_t)vec_get(v, 1) == 20);
    assert((intptr_t)vec_get(v, 2) == 30);
    assert(vec_get(v, 3) == NULL);

    vec_set(v, 5, (void*)(intptr_t)99);
    assert(v->len == 6);
    assert((intptr_t)vec_get(v, 5) == 99);

    vec_free(v);
}

static void test_queue(void) {
    queue* q = queue_new();
    assert(q != NULL);
    assert(queue_empty(q));

    queue_push(q, (void*)(intptr_t)1);
    queue_push(q, (void*)(intptr_t)2);
    queue_push(q, (void*)(intptr_t)3);
    assert(queue_length(q) == 3);
    assert(!queue_empty(q));

    assert((intptr_t)queue_pop(q) == 1);
    assert((intptr_t)queue_pop(q) == 2);
    assert((intptr_t)queue_pop(q) == 3);
    assert(queue_pop(q) == NULL);
    assert(queue_empty(q));

    queue_free(q);
}

static void test_stack(void) {
    stack* s = stack_new();
    assert(s != NULL);
    assert(stack_empty(s));

    stack_push(s, (void*)(intptr_t)1);
    stack_push(s, (void*)(intptr_t)2);
    stack_push(s, (void*)(intptr_t)3);

    assert(stack_size(s) == 3);
    assert(!stack_empty(s));

    assert((intptr_t)stack_pop(s) == 3);
    assert((intptr_t)stack_pop(s) == 2);
    assert((intptr_t)stack_pop(s) == 1);
    assert(stack_pop(s) == NULL);
    assert(stack_empty(s));

    stack_free(s);
}

static void test_set(void) {
    set* s = set_new();
    assert(s != NULL);
    assert(set_size(s) == 0);

    assert(set_add(s, "foo"));
    assert(!set_add(s, "foo"));

    assert(set_contains(s, "foo"));
    assert(!set_contains(s, "bar"));
    assert(set_size(s) == 1);

    assert(set_add(s, "bar"));
    assert(set_contains(s, "bar"));
    assert(set_size(s) == 2);

    set_free(s);
}

static void test_stringbuilder(void) {
    sb* b = sb_new();
    assert(b != NULL);

    sb_append(b, "Hello");
    sb_append_char(b, ' ');
    sb_append(b, "World");
    sb_append_char(b, '!');
    sb_append_char(b, ' ');
    sb_append_int(b, 123);

    assert(strcmp(sb_str(b), "Hello World! 123") == 0);

    sb_free(b);
}

static void test_grid(void) {
    const char* input =
        "ABC\n"
        "DEF\n";

    Grid* g = grid_from_string(input);
    assert(g != NULL);
    assert(g->width == 3);
    assert(g->height == 2);

    assert(grid_get(g, 0, 0) == 'A');
    assert(grid_get(g, 1, 0) == 'B');
    assert(grid_get(g, 2, 0) == 'C');
    assert(grid_get(g, 0, 1) == 'D');
    assert(grid_get(g, 1, 1) == 'E');
    assert(grid_get(g, 2, 1) == 'F');

    // out-of-bounds should be safe
    assert(grid_get(g, -1, 0) == 0);
    assert(grid_get(g, 3, 0) == 0);

    grid_set(g, 1, 1, 'X');
    assert(grid_get(g, 1, 1) == 'X');

    grid_free(g);
}

int main(void) {
    printf("Running container tests...\n");

    test_vector();
    test_queue();
    test_stack();
    test_set();
    test_stringbuilder();
    test_grid();

    printf("All container tests passed.\n");
    return 0;
}
