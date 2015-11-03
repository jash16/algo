#include "share.h"
#include "map.h"

map *map_create(void *type) {
    rbtree_type *rt = (rbtree_type *)type;
    map *m = (map *)calloc(sizeof(map), 1);
    if (m == NULL) {
        return NULL;
    }
    m->_t = rbtree_create(rt);
    if (m->_t == NULL) {
        free(m);
        return NULL;
    }
    return m;
}

void map_release(map *m) {
    rbtree_release(m->_t);
    free(m);
    return;
}

bool map_insert(map *m, void *key, void *val) {
    return rbtree_insert(m->_t, key, val);
}

bool map_delete(map *m, void *key) {
    return rbtree_delete(m->_t, key);
}

bool map_erase(map *m, void *key) {
    return rbtree_delete(m->_t, key);
}

bool map_empty(map *m) {
    return map_size(m) == 0;
}

//map_find返回一个pair
void *map_find(map *m, const void *key) {
    rbnode *n = rbtree_find(m->_t, key);
    if (n == NULL) {
        return NULL;
    } else {
        return RBNODE_VAL(n);
    }
    return NULL;
}

int map_count(map *m, const void *key) {
    return rbtree_count(m->_t, key);
}

map_iterator *map_get_iterator(map *m) {
    map_iterator *mi;

    mi = (map_iterator *)calloc(1, sizeof(map_iterator));
    if (mi == NULL) {
        return NULL;
    }
    mi->iter = rbtree_get_iterator(m->_t);
    if (mi->iter == NULL) {
        free(mi);
        return NULL;
    }
    return mi;
}

//void *map_next(map_iterator *mi, pair *p) {
void *map_next(map_iterator *mi) {
    rbnode *node;

    node = rbtree_next((rbtree_iterator *)mi->iter);
    if (node == NULL) {
        return NULL;
    }
//    p->first = RBNODE_KEY(node);
//    p->second = RBNODE_VAL(node);
//    return p;
    return RBNODE_VAL(node);
//    return make_pair(RBNODE_KEY(node), RBNODE_VAL(node), NULL);
}

void map_release_iterator(map_iterator *mi) {
    free(mi->iter);
    free(mi);
}

#ifdef MAP_TEST
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "pair.h"

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

char *key_of_val(void *val) {
    pair *p = (pair *)val;
    return p->first;
}

char *val_of_val(void *val) {
    pair *p = (pair *)val;
    return p->second;
}

rbtree_type demo_type = { 
    .key_cmp = key_cmp,
    .val_cmp = val_cmp,
    .key_dup = NULL,
    .val_dup = NULL,
    .key_free = key_free,
    .val_free = val_free,
    .key_of_val = key_of_val,
    .val_of_val = val_of_val,
};

int rand_range(int a, int b) {
    return rand() % (b-a) + a;
}

int main() {
    map *m = map_create(&demo_type);
    assert(m != NULL);
    char *key = NULL;
    char *val = NULL;
    srand(time(NULL));
    int i = 0, j = 0;
    //key = malloc()
//    map_insert(m, "hellofjwoefjoewjfo", "world");
    for (i = 0; i < 100; i++) {
        key = malloc(sizeof(char) * 16);
        val = malloc(sizeof(char) * 16);
        for (j = 0; j < 15; j ++) {
            key[j] = 'a' + rand_range(0, 25);
        }   
        for (j = 0; j < 15; j++) {
            val[j] = 'a' + rand_range(0, 25);
        }   
        key[15] = '\0';
        val[15] = '\0';
        map_insert(m, key, val);
    }
    printf("map_size: %d\n", map_size(m));
    struct timeval start, end;
    gettimeofday(&start, NULL);
    void *n = map_find(m, "hellofjwoefjoewjfo");
    gettimeofday(&end, NULL);
    assert(n == NULL);
//    printf("%s\n", (char *)n);
    int waist = (end.tv_sec - start.tv_sec) * 100000 + (end.tv_usec - start.tv_usec);
    printf("waist time: %d, start:(sec: %d, usec: %d), end(sec: %d, usec: %d)\n", waist,
            start.tv_sec, start.tv_usec, end.tv_sec, end.tv_usec);

    map_iterator *mi = map_get_iterator(m);
    assert(mi != NULL);
    pair *data;
    while ((data = map_next(mi))) {
        printf("key: %s => val: %s\n", (char *)(((pair *)data)->first), (char *)(((pair*)data)->second));
    }
    map_release_iterator(mi);
    map_release(m);
}

#endif
