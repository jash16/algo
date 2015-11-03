#include "share.h"
#include "stack.h"

stack *stack_create(int size, stack_type *st) {
    stack *s = NULL;
    int ssize;

    ssize = size < STACK_DEFAULT_SIZE ? STACK_DEFAULT_SIZE : size;
 
    s = (stack *)calloc(sizeof(stack), 1);
    if (s == NULL) {
        return NULL;
    }

    s->ele = (void **)calloc(sizeof(void *), ssize);
    if (s->ele == NULL) {
        free(s);
        return NULL;
    }

    s->size = ssize;
    s->top = 0;
    s->st = st;
    return s;
}

void stack_release(stack *s) {
    int i = 0;
    int used = s->top;
    if (s->st && s->st->free) {
        for (i = 0; i < used; i++) {
            s->st->free(s->ele[i]);
        }
    }
    if (s) {
        free(s->ele);
        free(s);
    }
    return;
}

/*
bool stack_full(stack *s) {
    if (s->top == s->size) {
        return true;
    }
    return false;
}

bool stack_empty(stack *s) {
    if (s->top == 0) {
        return true;
    }
    return false;
}
*/
bool stack_push(stack *s, void *data) {
    int size;
    void **ptr;

    if (stack_full(s)) {
        size = s->size * 2;
        ptr = (void **)realloc(s->ele, size * sizeof(void *));
        if (ptr == NULL) {
            return false;
        }
        s->ele = ptr;
        s->size = size;
    }
    s->ele[s->top++] = data;
    return true;
}

void *stack_pop(stack *s) {
    if (stack_empty(s)) {
        return NULL;
    }

    return s->ele[--s->top];
}

void *stack_top(stack *s) {
    if (stack_empty(s)) {
        return NULL;
    }
    
    return s->ele[s->top - 1];
}

#ifdef STACK_TEST
#include <stdio.h>
#include <assert.h>

int main() {
    stack *s = stack_create(8, NULL);
    assert(s != NULL);
    int a = 10;
    int b = 20;
    stack_push(s, &a);
    stack_push(s, &b);
    
    printf("top: %d\n", *(int *)stack_pop(s));
    printf("top: %d\n", *(int *)stack_pop(s));
    stack_release(s);
}
#endif
