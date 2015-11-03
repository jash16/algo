#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

/*use heap to implements priority_queue*/

#include "bool.h"

#define MAX_PRIORITY_QUEUE 1
#define MIN_PRIORITY_QUEUE 0

/*typedef struct pq_type {
    int (*cmp)(void *val1, void *val2);
}pq_type;
*/
typedef struct priority_queue {
    void *h; //can you max_heap
//    void *type;
} priority_queue;

priority_queue *pq_create(int flag, void *type);
void pq_release(priority_queue *pq);

bool pq_insert(priority_queue *pq, void *val);
void *pq_max(priority_queue *pq);
void *pq_extract_max(priority_queue *pq);
void *pq_increase_key(priority_queue *pq, void *val1, void *val2);

void *pq_min(priority_queue *pq);
void *pq_extract_min(priority_queue *pq);
void *pq_decrease_key(priority_queue *pq, void *val1, void *val2);
#endif /*__PRIORITY_QUEUE_H__*/
