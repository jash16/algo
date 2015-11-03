#ifndef __HEAP_H__
#define __HEAP_H__

#include "bool.h"

#define MIN_HEAP 0
#define MAX_HEAP 1

#define HEAP_MIN_SIZE 8

typedef struct heap_type {
    void (*free)(void *ptr);
    int (*cmp)(void *ptr, void *ptr2);// >0 ptr > ptr2; <0 ptr < ptr2, =0 ptr == ptr2
    void *(*dup)(void *ptr);
    bool (*match)(void *ptr1, void *ptr2); // true ptr == ptr2
} heap_type;

typedef struct heap {
    int flag;
    int used;
    int size;
    void **ele;
    heap_type *ht;
} heap;

//#define HEAP_SET_FREE_METHOD(h, m) ((h)->free = m)
//#define HEAP_SET_CMP_METHOD(h, m) ((h)->cmp = m)

heap *heap_create(int size, int flag, heap_type *ht);
void heap_release(heap *h);

//void *heap_search(heap *h, void *value);

bool heap_insert(heap *h, void *value);
void *heap_pop(heap *h);  //大堆则取最大值，小堆则取最小值

void *heap_top(heap *h);

void heap_sort(heap *h);
void heap_make(heap *h);

#endif /*__HEAP_H__*/
