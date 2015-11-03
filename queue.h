#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "bool.h"

/*
typedef struct queue_iterator {
    int idx;
    queue *q;
} queue_iterator;
*/

typedef struct queue_type {
    void (*free)(void *ptr);
    void *(*dup)(void *ptr);
} queue_type;

typedef struct queue {
    int head;
    int tail;
    int size;
    void **ele;
    queue_type *qt;
} queue;

queue *queue_create(int size, queue_type *qt);
void queue_release(queue *q);

/*
queue_iterator *queue_get_iterator(queue *q);
void queue_release_iterator(queue_iterator *qi);
void *queue_next(queue_iterator *qi);
*/
//#define queue_size(q) ((q)->tail)

#define queue_front(q) ((q)->head)

int queue_size(queue *q);

bool enqueue(queue *q, void *data);
void *dequeue(queue *q);
bool queue_empty(queue *q);
bool queue_full(queue *q);

#endif /*__QUEUE_H__*/
