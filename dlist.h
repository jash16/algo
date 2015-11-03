#ifndef __DLIST_H__
#define __DLIST_H__
#include "bool.h"
typedef struct dlist_node {
    struct dlist_node *next;
    struct dlist_node *prev;
    void *value;
}dlist_node;

typedef struct dlist_iterator {
    struct dlist_node *next;
    int direction;
} dlist_iterator;

typedef struct dlist {
    dlist_node *head;
    dlist_node *tail;
    
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    bool (*match)(void *ptr, void *ptr2);
    unsigned long len;
}dlist;

#define dlist_length(l) ((l)->len)
#define dlist_first(l) ((l)->head)
#define dlist_last(l) ((l)->tail)
#define dlist_prev_node(n) ((n)->prev)
#define dlist_next_node(n) ((n)->next)
#define dlist_node_value(n) ((n)->value)

#define dlist_set_dup_method(l, m) ((l)->dup = (m))
#define dlist_set_free_method(l, m) ((l)->free = (m))
#define dlist_set_match_method(l, m) ((l)->match = (m))

#define dlist_node_free(l, n) do {\
    if ((l)->free) {\
        (l)->free((n)->value); \
    } \
    free(n); \
}while (0)

dlist *dlist_create();
void dlist_release(dlist *list);
dlist *dlist_add_node_head(dlist *list, void *value);
dlist *dlist_add_node_tail(dlist *list, void *value);
dlist *dlist_insert_node(dlist *list, dlist_node *old_node, void *value, int after);
void dlist_del_node(dlist *list, dlist_node *node);
dlist_iterator *dlist_get_iterator(dlist *list, int direction);
dlist_node *dlist_next(dlist_iterator *di);
void dlist_release_iterator(dlist_iterator *di);

dlist *dlist_dup(dlist *orgi);
dlist_node *dlist_search_key(dlist *list, void *key);
dlist_node *dlist_index(dlist *list, long index);
void dlist_rewind(dlist *list, dlist_iterator *di);
void dlist_rewind_tail(dlist *list, dlist_iterator *di);
void dlist_rotate(dlist *list);

#define DL_START_HEAD 0
#define DL_START_TAIL 1

#endif
