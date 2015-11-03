#include "set.h"
#include "share.h"
#include "pair.h"

set *set_create(void *type) {
    set *s;

    s = (set *)calloc(sizeof(set), 1);
    if (s == NULL) {
        return NULL;
    }
    s->_t = rbtree_create((rbtree_type *)type);
    if (s->_t == NULL) {
        free(s);
        return NULL;
    }
    return s;
}

void set_release(set *s) {
    rbtree_release(s->_t);
    free(s);
}

bool set_add(set *s, void *value) {
    return rbtree_insert(s->_t, value, value);
}

bool set_delete(set *s, void *value) {
    return rbtree_delete(s->_t, value);
}

void set_clear(set *s) {
    return rbtree_clear(s->_t);
}

void *set_find(set *s, const void *value) {
    pair *p = NULL;
    rbnode *n = rbtree_find(s->_t, value);
    if (n != NULL) {
        p = RBNODE_VAL(n);
        return p->second;
    } else {
        return NULL;
    }
    return NULL;
}

bool set_erase(set *s, void *value) {
    return rbtree_delete(s->_t, value);
}

set_iterator *set_get_iterator(set *s) {
    set_iterator *si = calloc(1, sizeof(set_iterator));
    if (si == NULL) {
        return NULL;
    }
    si->iter = rbtree_get_iterator(s->_t);
    if (si->iter == NULL) {
        free(si);
        return NULL;
    }
    return si;
}

void *set_next(set_iterator *si) {
    rbnode *node;
    node = rbtree_next((rbtree_iterator *)si->iter);
    if (node == NULL) {
        return NULL;
    }
    return ((pair *)(RBNODE_VAL(node)))->first;
}

void set_release_iterator(set_iterator *si) {
    rbtree_release_iterator(si->iter);
    free(si);
    return;
}

#ifdef SET_TEST
#include <stdio.h>
#include <assert.h>

int key_cmp(const void *key1, const void *key2) {
    char *k1 = (char *)key1;
    char *k2 = (char *)key2;
    return strcmp(k1, k2);
}

int val_cmp(const void *val1, const void *val2) {
    char *v1 = (char *)val1;
    char *v2 = (char *)val2;
    return strcmp(v1, v2);
}

void key_free(void *key) {
    free(key);
    return;
}

void val_free(void *val) {
    free(val);
    return;
}

void *key_of_val(void *val) {
    pair *p = (pair *)val;
    return p->second;
}

void *val_of_val(void *val) {
    pair *p = (pair *)val;
    return p->first;
}

rbtree_type demo_type = { 
    .key_cmp = key_cmp,
    .val_cmp = val_cmp,
    .key_dup = NULL,
    .val_dup = NULL,
    .key_free = NULL,
    .val_free = val_free,
    .key_of_val = key_of_val,
    .val_of_val = val_of_val,
};

int rand_range(int a, int b) {
    return rand() % (b-a) + a;
}

int main() {
    set *m = set_create(&demo_type);
    assert(m != NULL);
    char *key = NULL;
    char *val = NULL;
    srand(time(NULL));
    int i = 0, j = 0;
    //key = malloc()
//    set_add(m, "hellofjwoefjoewjfo");
    set_add(m, "zzzzzzzzzzzzzzzzzz");
    for (i = 0; i < 10000000; i++) {
        key = malloc(sizeof(char) * 16);
        for (j = 0; j < 15; j ++) {
            key[j] = 'a' + rand_range(0, 25);
        }   
        key[15] = '\0';
        set_add(m, key);
    }
    printf("set_size: %d\n", set_size(m));
    struct timeval start, end;
    gettimeofday(&start, NULL);
    void *n = set_find(m, "zzzzzzzzzzzzzzzzzz");
    gettimeofday(&end, NULL);
//    assert(n != NULL);
//    printf("%s\n", (char *)n);
    int waist = (end.tv_sec - start.tv_sec) * 100000 + (end.tv_usec - start.tv_usec);
    printf("waist time: %d, start:(sec: %d, usec: %d), end(sec: %d, usec: %d)\n", waist,
            start.tv_sec, start.tv_usec, end.tv_sec, end.tv_usec);
    char *data;
    set_iterator *si = set_get_iterator(m);
    while ((data = set_next(si))) {
//        printf("%s\n", data);   
    }
    set_release_iterator(si);
//    set_release(m);
    
}

#endif
