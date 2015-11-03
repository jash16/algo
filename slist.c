#include "share.h"
#include "slist.h"

slist *slist_create() {
    slist *l = (slist *)calloc(sizeof(slist), 1);
    if (l == NULL) {
        return NULL;
    }
    l->head = NULL;
    l->tail = NULL;
    l->len = 0;
    l->dup = NULL;
    l->free = NULL;
    l->match = NULL;

    return l;
}

void slist_release(slist *list) {
    slist_node *sn;
    slist_node *next;
    sn = list->head;

    while (sn != NULL) {
        next = sn->next;
        if (list->free) {
            list->free(sn->value);
        }
        free(sn);
        sn = next;
    }
    free(list);
    return;
}

slist *slist_add_node_head(slist *list, void *value) {
    slist_node *sn;

    sn = (slist_node *)malloc(sizeof(slist_node));
    if (sn == NULL) {
        return NULL;
    }
    
    sn->value = value;
    if (list->head == NULL) {
        list->head = sn;
        list->tail = sn;

    } else {
        sn->next = list->head;
        list->head = sn;
    }
    list->len ++;
    return list;
} 

slist *slist_add_node_tail(slist *list, void *value) {
    slist_node *sn;
    sn = (slist_node *)calloc(sizeof(slist_node), 1);
    if (sn == NULL) {
        return NULL;
    }
    sn->value = value;
    
    if (list->head == NULL) {
        list->head = sn;
        list->tail = sn;
    } else {
        sn->next = NULL;
        list->tail->next = sn;
        list->tail = sn;
    }
    list->len ++;
    return list;
}

slist *slist_insert_node(slist *list, slist_node *old, void *value, int after) {
    slist_node *new;
    slist_node *tmp;

    new = (slist_node *)calloc(sizeof(slist_node), 1);
    if (new == NULL) {
        return NULL;
    }

    new->value = value;
    new->next = NULL;
    if (after == 1) {
        new->next = old->next;
        old->next = new;
    } else {
        tmp = list->head;
        if (tmp && tmp == old) { // head node
            list->head = new;
            new->next = old;
        }
        while (tmp && tmp->next != old) {
            tmp = tmp->next;
        }
        if (tmp) {
            tmp->next = new;
            new->next = old;
        } else {
            return NULL;
        }
    }
    list->len ++;
    return list;
}

void slist_del_node(slist *list, slist_node *sn) {
    slist_node *tmp;
    tmp = list->head;
    if (tmp && tmp == sn) {
        list->head = sn->next;
        if (list->head == NULL) {
            list->tail = list->head;
        }
        //return;
    }

    while (tmp && tmp->next != sn) {
        tmp = tmp->next;
    }

    if (tmp != NULL) {
        tmp->next = sn->next;    
    } else {
        return;
    }
    
    if (list->free) {
        list->free(sn->value);
    }

    free(sn);
    list->len --;
    return;
}

slist_iterator *slist_get_iterator(slist *list) {
    slist_iterator *si;

    si = (slist_iterator*)calloc(sizeof(slist_iterator), 1);
    if (si == NULL) {
        return NULL;
    }

    si->next = list->head;
    return si;
}

slist_node *slist_next(slist_iterator *si) {
    slist_node *current = NULL;
    current = si->next;

    if (current != NULL) {
        si->next = current->next;
    }
    return current;
}

void slist_release_iterator(slist_iterator *si) {
    free(si);
}

slist *slist_dup(slist *orig) {
    slist *list;
    slist_node *sn;
    slist_iterator *si;
    void *value = NULL;

    list = slist_create();
    if (list == NULL) {
        return NULL;
    }
    list->dup = orig->dup;
    list->free = orig->free;
    list->match = orig->match;

    si = slist_get_iterator(orig);
    if (si == NULL) {
        free(list);
        return NULL;
    }

    while ((sn = slist_next(si))) {
        if (list->dup) {
            value = list->dup(sn->value);
            if (value == NULL) {
                slist_release_iterator(si);
                slist_release(list);
                return NULL;
            }
        } else {
            value = sn->value;
        }
        if (slist_add_node_tail(list, sn) == NULL) {
            slist_release_iterator(si);
            slist_release(list);
            return NULL;
        }
    }

    slist_release_iterator(si);
    return list;
}

slist_node *slist_search(slist *list, void *key) {
    slist_node *node;
    slist_iterator *si;

    si = slist_get_iterator(list);
    if (si == NULL) {
        return NULL;
    }

    while ((node = slist_next(si))) {
        if (list->match) {
            if (list->match(node->value, key)) {
                slist_release_iterator(si);
                return node;
            }
        } else {
            if (node->value == key) {
                slist_release_iterator(si);
                return node;
            }
        }
    }
    
    slist_release_iterator(si);
    return NULL;
}

slist_node *slist_index(slist *list, long index) {
    slist_node *node;

    if (index < 0) {
        index = (-index) - 1;
    }

    if (index > list->len) {
        return list->tail;
    }
    
    node=list->head;
    while (index && node->next) {
        index --;
        node = node->next;
    }
    
    return node;
}

void slist_rewind(slist *list, slist_iterator *si) {
    si->next = list->head;
    return;
}

#ifdef SLIST_TEST
#include <stdio.h>
#include <assert.h>

int main() {
    slist *list = slist_create();

    assert(list != NULL);

    int arr[] = {1, 2, 3, 4, 5, 6};
    int i = 0;
    int n = sizeof(arr) / sizeof(arr[0]);
    for (i = 0; i < n; i++) {
        slist_add_node_head(list, &arr[i]);
    }
    
    slist_iterator *si = slist_get_iterator(list);
    assert(si != NULL);
    slist_node *sn = NULL;
    while ((sn = slist_next(si)) != NULL) {
        printf("%d\t", *(int *)sn->value);
    }
    printf("\n");
    slist_rewind(list, si);

    int a = 10;
    slist_add_node_tail(list, &a);

    while ((sn = slist_next(si)) != NULL) {
        printf("%d\t", *(int *)sn->value);
    }
    printf("\n");
    int c = 4;
    sn = slist_search(list, &arr[2]);
    slist_insert_node(list, sn, &c, 1);

    slist_rewind(list, si);
    while ((sn = slist_next(si)) != NULL) {
        printf("%d\t", *(int *)sn->value);
    }
    printf("\n"); 

    slist *list2 = slist_dup(list);

    slist_rewind(list, si);
    while ((sn = slist_next(si)) != NULL) {
        printf("%d\t", *(int *)sn->value);
    }
    printf("\n");

    slist_release(list);
    slist_release(list2);
    slist_release_iterator(si);
}

#endif /*SLIST_TEST*/
