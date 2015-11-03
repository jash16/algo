#ifndef __SET_H__
#define __SET_H__

#include "rbtree.h"

typedef struct set_iterator {
    void *iter;   
} set_iterator;

typedef struct set {
    rbtree *_t;
} set;

//type used for the low level, like rbtree implements
set *set_create(void *type); 

bool set_add(set *s, void *value);
void set_clear(set *s);
bool set_erase(set *s, void *value);
void *set_find(set *s, const void *value);

set_iterator *set_get_iterator(set *s);
void *set_next(set_iterator *si);
void set_release_iterator(set_iterator *si);

#define set_size(s)  ((s)->_t->size)

#endif /*__SET_H__*/

