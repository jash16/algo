#ifndef __MAP_H__
#define __MAP_H__

#include "rbtree.h"

/*
typedef struct map_type {
    unsigned long (*hash)(const void *key, int len);
    void (*free_key)(void *key);
    void (*free_val)(void *val)
    bool (*cmp_key)(const void *key1, const void *key2);
    bool (*cmp_val)(const void *val1, const void *val2);
} map_type;
*/

#define map_size(m) ((m)->_t->size)

#define map_type(m, rt) ((m)->_t->rt = rt)

typedef struct map_iterator {
    void *iter;
} map_iterator;

typedef struct map {
    rbtree *_t;
} map;

map *map_create(void *type);
void map_release(map *m);

void map_clear(map *m);

int map_count(map *m, const void *key);
void map_emtpy(map *m);
bool map_erase(map *m, void *key);


bool map_insert(map *m, void *key, void *value);
bool map_delete(map *m, void *key);

void *map_find(map *m, const void *key);

map_iterator *map_get_iterator(map *m);
void *map_next(map_iterator *mi);
void map_release_iterator(map_iterator *mi);

#endif /*__MAP_H__*/
