#ifndef __PAIR_H__
#define __PAIR_H__

#include "bool.h"

typedef struct pair_type {
    int (*first_cmp)(void *fir1, void *fir2);
    int (*second_cmp)(void *sec1, void *sec2);
    void (*first_free)(void *first);
    void (*second_free)(void *second);
} pair_type;

typedef struct pair {
    void *first;
    void *second;
    pair_type *pt;
} pair;

pair *make_pair(void *first, void *second, pair_type *pt);
void free_pair(pair *p);

void *first(pair *p);
void *second(pair *p);

int pair_cmp(pair *p1, pair *p2);

#endif /*__PAIR_H__*/
