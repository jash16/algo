#ifndef __STACK_H__
#define __STACK_H__

#include "bool.h"

#define STACK_DEFAULT_SIZE 8

#define stack_full(s) ((s)->top == (s)->size)
#define stack_empty(s) ((s)->top == 0)
#define stack_size(s) ((s)->top)

typedef struct stack_type {
    void (*free)(void *ptr);
} stack_type;

/*
typedef struct stack_iterator {
    int idx;
    stack *s;
} stack_iterator;
*/
typedef struct stack {
    int top;
    int size;
    void **ele;
    stack_type *st;
} stack;

stack *stack_create(int size, stack_type *st);
void stack_release(stack *s);

bool stack_push(stack *s, void *data);
void *stack_pop(stack *s);
void *stack_top(stack *s);

//bool stack_full(stack *s);
//bool stack_empty(stack *s);

//stack_iterator *stack_get_iterator(stack *s);
//void *stack_next(stack_iterator *si);
//void stack_release_iterator(stack_iterator *si);

#endif /*__STACK_H__*/
