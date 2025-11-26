// stack.h
#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdbool.h>

typedef struct stack stack;

stack* stack_new(void);
void stack_free(stack* s);

void stack_push(stack* s, void* item);
void* stack_pop(stack* s);

bool stack_empty(stack* s);
size_t stack_size(stack* s);

#endif
