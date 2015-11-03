#include "share.h"
#include "dlist.h"

dlist *dlist_create() {
    dlist *list = calloc(sizeof(dlist), 1);
    if (list == NULL) {
        return NULL;
    }
    
    list->head = NULL;
    list->tail = NULL;

    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;

    list->len = 0;

    return list;
}

void dlist_release(dlist *list) {
    dlist_node *current, *next;

    current = list->head;
    
    while (current) {
        next = current->next;
        if (list->free) {
            list->free(current);
        }
        free(current);
        current = next;
    }
    return;
}


dlist *dlist_add_node_head(dlist *list, void *value) {
    dlist_node *node;

    node = (dlist_node *)calloc(sizeof(dlist_node), 1);
    if (node == NULL) {
        return NULL;
    }

    node->value = value;
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
    } else {
        node->next = list->head;
        list->head->prev = node;
        node->prev = NULL;
        list->head = node;
    }

    list->len ++;
    return list;
}

dlist *dlist_add_node_tail(dlist *list, void *value) {
    dlist_node *node;
    
    node = (dlist_node *)calloc(sizeof(dlist_node), 1);
    if (node == NULL) {
        return NULL;
    }

    node->value = value;

    if (list->tail == NULL) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->len ++;
    return list;
}

dlist *dlist_insert_node(dlist *list, dlist_node *old, void *value, int after) {
    dlist_node *node;

    node = (dlist_node *)calloc(sizeof(dlist_node), 1);
    if (node == NULL) {
        return NULL;
    }
    
    node->value = value;
    if (after) {           //after old
        node->prev = old;
        node->next = old->next;
        if (old->next) {
            old->next->prev = node;
        } else {
            list->tail = node;
        }
        old->next = node;

    } else {              //before old
        node->next = old;
        node->prev = old->prev;
        if (old->prev) { //not head
            old->prev->next = node;
        } else {
            list->head = node;
        }
        old->prev = node;
    }
    list->len ++;
    return list;
}

void dlist_del_node(dlist *list, dlist_node *node) {
    dlist_node *before;
    dlist_node *after;
    
    before = node->prev;
    after = node->next;
    if (before) {
        before->next = after;
    } else {
        list->head = after;
    }
    if (after) {
        after->prev = before;
    } else {
        list->tail = before;
    }

    list->len --;
    return;
}

dlist_iterator *dlist_get_iterator(dlist *list, int direction) {
    dlist_iterator *di;

    di = (dlist_iterator *)calloc(sizeof(dlist_iterator), 1);
    if (di == NULL) {
        return NULL;
    }

    di->direction = direction;
    if (direction) { //from head to tail
        di->next = list->head;
    } else {
        di->next = list->tail;
    }

    return di;
}

void dlist_release_iterator(dlist_iterator *di) {
    free(di);
}

dlist_node *dlist_next(dlist_iterator *di) {
    dlist_node *current;
    
    current = di->next;
    if (current) {
        if (di->direction) {
            di->next = current->next;
        } else {
            di->next = current->prev;
        }
    }
    return current;
}

void dlist_rewind(dlist *list, dlist_iterator *di) {
    di->next = list->head;
    di->direction = 1;
}

void dlist_rewind_tail(dlist *list, dlist_iterator *di) {
    di->next = list->tail;
    di->direction = 0;
}

dlist *dlist_dup(dlist *orig) {
    void *ptr;
    dlist *new;
    dlist_iterator *di;
    dlist_node *node;

    new = dlist_create();
    if (new == NULL) {
        return NULL;
    }

    new->dup = orig->dup;
    new->free = orig->free;
    new->match = orig->match;

    di = dlist_get_iterator(orig, 1);
    if (di == NULL) {
        free(new);
        return NULL;
    }
    while ((node = dlist_next(di))) {
        if (new->dup) {
            ptr = new->dup(node->value);
            if (ptr == NULL) {
                dlist_release(new);
                dlist_release_iterator(di);
                return NULL;
            }
        } else {
            ptr = node->value;
        }
        
        if (dlist_add_node_tail(new, ptr) == NULL) {
            dlist_release(new);
            dlist_release_iterator(di);
            return NULL;
        }
    }

    dlist_release_iterator(di);
    return new;
}

dlist_node *dlist_search_key(dlist *list, void *key) {
    dlist_iterator *di;
    dlist_node *node;

    di = dlist_get_iterator(list, 1);
    if (di == NULL) {
        return NULL;
    }
    
    while ((node = dlist_next(di))) {
        if (list->match) {
            if (list->match(node->value, key)) {
                 dlist_release_iterator(di);
                 return node;
            }
        } else {
            if (key == node->value) {
                 dlist_release_iterator(di);
                 return node;
            }
        }
    }
    
    dlist_release_iterator(di);
    return NULL;
}

dlist_node *dlist_index(dlist *list, long index) {
    dlist_node *node;

    if (index < 0) {
        index = (-index) - 1;
        node = list->tail;
        while (index -- && node) {
            node = node->prev;
        }
    } else {
        node = list->head;
        while (index -- && node) {
            node = node->next;
        }
    }
    return node;
}

void dlist_rotate(dlist *list) {
    dlist_node *tail;

    if (dlist_length(list) <= 1) {
        return;
    }

    tail = list->tail;
    list->tail = tail->prev;
    list->tail->next = NULL;

    tail->next = list->head;
    tail->next->prev = tail;
    tail->prev = NULL;
    list->head = tail;
}

#ifdef DLIST_TEST

#include <stdio.h>
#include <assert.h>

int main() {
    dlist *list = dlist_create();
    assert(list != NULL);
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    int i = 0;
    int n = sizeof(arr) / sizeof(arr[0]);

    for (i = 0; i < n; i++) {
        dlist_add_node_head(list, &arr[i]);
    }

    dlist_iterator *di = dlist_get_iterator(list, 1);
    assert(di != NULL);
    dlist_node *dn = NULL;
    while ((dn = dlist_next(di))) {
        printf("%d\t", *(int *)dn->value);
    }
    printf("\n");

    dn = dlist_search_key(list, &arr[2]);
    assert(dn != NULL);
    int a = 100;
    dlist_insert_node(list, dn, &a, 0);

    dlist_rewind(list, di);
    while ((dn = dlist_next(di))) {
        printf("%d\t", *(int *)dn->value);
    }
    printf("\n");

    dlist *list2 = dlist_dup(list);
    assert(list2 != NULL);

    dlist_rewind_tail(list2, di);
    while ((dn = dlist_next(di))) {
        printf("%d\t", *(int *)dn->value);
    }
    printf("\n");

    dlist_release(list);
    dlist_release(list2);
    dlist_release_iterator(di);
}

#endif /*DLIST_TEST*/
