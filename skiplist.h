#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include "bool.h"
#define MAX_LEVEL 16

typedef struct skiplist_type {
    void (*key_free)(void *key);
    int (*key_cmp)(void *key1, void *key2);
    void (*val_free)(void *val);
    int (*val_cmp)(void *val1, void *val2);
}skiplist_type;

typedef struct skipnode {
    void *key;
    void *val;
    struct skipnode *forward[0];
} skipnode;

typedef struct skiplist {
    int level;
    int length;
    struct skiplist_type *st;
    struct skipnode *header;
} skiplist;

#endif /*__SKIPLIST_H__*/
