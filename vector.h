#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "bool.h"

#define VECTOR_DEFAULT_SIZE 8

typedef struct vector_type {
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*cmp)(void *ptr1, void *ptr2);
}vector_type;

typedef struct vector {
    int used;
    int size;
    void **ele;
    vector_type *vt;
} vector;

typedef struct vector_iterator {
    struct vector *v;
    int idx;    
} vector_iterator;

#define vector_empty(v) ((v)->used == 0)
#define vector_size(v) ((v)->used)
#define vector_capacity(v) ((v)->size)

#define vector_front(v) ((v)->ele[0])
#define vector_back(v) ((v)->ele[v->used - 1])

//#define VECTOR_SET_DUP_METHOD(v, m) ((v)->dup = m)
//#define VECTOR_SET_FREE_METHOD(v, m) ((v)->free = m)
//#define VECTOR_SET_MATCH_METHOD(v, m) ((v)->match = m)

//#define VECTOR_FREE_ELE(v, e) do {\
    if ((v)->free) { \
        (v)->free(e); \
    } \
} while(0)

vector *vector_create(int size, vector_type *vt);

void vector_release(vector *v);

vector *vector_reinit(vector *v);

vector_iterator *vector_iterator_get(vector *v);
void *vector_next(vector_iterator *vi);
void vector_iterator_release(vector_iterator *vi);
void vector_iterator_rewind(vector_iterator *vi);

//peek the last value
void *vector_top(vector *v);
//push to the end
bool vector_push(vector *v, void *data); 
//pop the last value
void *vector_pop(vector *v);

//clear all the value
bool vector_clear(vector *v); //清除所有元素

//erase value from pos1 to pos2
void vector_erase(vector *v, int pos1, int pos2);  //清除从pos1到pos2中的元素

//insert n*data start pos
bool vector_insert(vector *v, int pos, int n, void *data);

//get value in the idx position
void *vector_get_idx(vector *v, int idx);

#endif  /*__VECTOR_H__*/
