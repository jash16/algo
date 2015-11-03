#ifndef __SLIST_H__
#define __SLIST_H__

#include "bool.h"

typedef struct slist_node {
    struct slist_node *next;
    void *value;
}slist_node;

typedef struct slist_iterator {
    struct slist_node *next;
}slist_iterator;

typedef struct slist {
    struct slist_node *head;
    struct slist_node *tail;
    
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    bool (*match)(void *ptr1, void *ptr2);
    unsigned long len;
}slist;

#define slist_length(l) ((l)->len)
#define slist_first(l) ((l)->head)
#define slist_last(l) ((l)->tail)
#define slist_next_node(n) ((n)->next)
#define slist_node_value(n) ((n)->value)

#define slist_set_dup_method(l, m) ((l)->dup = (m))
#define slist_set_free_method(l, m) ((l)->free = (m))
#define slist_set_match_method(l, m) ((l)->match = (m))

#define slist_node_free(l, n) do {\
    if ((l)->free) {\
        (l)->free((n)->value); \
    } else {\
        free(n->value); \
    }  \
    free(n); \
}while (0)

slist *slist_create();
void slist_release(slist *list);
slist *slist_add_node_head(slist *list, void *sn);
slist *slist_add_node_tail(slist *list, void *sn);
slist *slist_insert_node(slist *list, slist_node *old_node, void *value, int after);
void slist_del_node(slist *list, slist_node *node);

slist_iterator *slist_get_iterator(slist *list);
slist_node *slist_next(slist_iterator *si);
void slist_release_iterator(slist_iterator *si);
slist *slist_dup(slist *orgi);
slist_node *slist_search(slist *list, void *key);
slist_node *slist_index(slist *list, long index);

void slist_rewind(slist *list, slist_iterator *si);
#endif /*__SLIST_H__*/
