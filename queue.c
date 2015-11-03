#include "share.h"
#include "queue.h"

queue *queue_create(int size, queue_type *qt) {
    queue *q = (queue *)calloc(sizeof(queue), 1);
    if (q == NULL) {
        return NULL;
    }

    q->head = 0;
    q->tail = 0;

    q->size = size;

    q->ele = (void **)calloc(sizeof(void *), size);
    if (q->ele == NULL) {
        free(q);
        return NULL;
    }
    
    return q;
}

void queue_release(queue *q) {
    void *ptr;
    queue_type *qt = q->qt;
    if (qt && qt->free) {
        while (!queue_empty(q)) {
            ptr = dequeue(q);
            qt->free(ptr);
        }
    }
    free(q->ele);
    free(q);
}

bool enqueue(queue *q, void *value) {
    if (queue_full(q)) {
        return false;
    }

    q->ele[q->tail] = value;
    q->tail = (q->tail + 1) % q->size;
    return true;
}

void *dequeue(queue *q) {
    void *ptr;

    if (queue_empty(q)) {
        return NULL;
    }

    ptr = q->ele[q->head];
    q->head = (q->head + 1) % q->size;

    return ptr;
}

bool queue_empty(queue *q) {
    if (q->head == q->tail) {
        return true;
    }
    return false;
}

bool queue_full(queue *q) {
    if ((q->tail + 1) % q->size == q->head) {
        return true;
    }
    return false;
}

int queue_size(queue *q) {
    int size = 0;
    
    size = q->tail - q->head;

    if (size < 0) {
        size += q->size;
    }

    return size;
}

/*
queue_iterator *queue_get_iterator(queue *q) {
    queue_iterator *qi;

    qi = (queue_iterator *)calloc(sizeof(queue_iterator), 1);
    if (qi == NULL) {
        return NULL;
    }

    qi->q = q;
    qi->idx = q->head;
    return qi;
}

void queue_release_iterator(queue_iterator *qi) {
    free(qi);
}

void *queue_next(queue_iterator *qi) {
    void *ptr;

    queue *q = qi->q;

    if (qi->idx == q->tail) {
        return NULL;
    }

    ptr = q->ele[qi->idx++];
    return ptr;
} */


#ifdef QUEUE_TEST
#include <stdio.h>
#include <assert.h>

int main() {
    queue *q = queue_create(8, NULL);
    assert(q != NULL);
    int a = 10;
    int b = 20;
    enqueue(q, &a);
    enqueue(q, &b);

    printf("size=%d\n", queue_size(q));
    assert(queue_full(q) != true);
    printf("%d\n", *(int *)dequeue(q));
    printf("%d\n", *(int *)dequeue(q));
    assert(queue_empty(q) != false);
 
    queue_release(q);
}
#endif
