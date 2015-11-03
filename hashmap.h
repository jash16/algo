#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#define HASHMAP_DEFAULT_SIZE 8
#define HASHMAP_EXPAND_FACTOR 1.5

typedef struct hash_type {
    int (*hash_func)(void *key);
    void (*key_free) (void *key);
    void (*val_free)(void *val);
    void *(*key_dup)(const void *key);
    void *(*val_dup)(const void *val);
    int (*key_cmp)(const void *key1, const void *key2);
    int (*val_cmp)(const void *val1, const void *val2);
} hash_type;

typedef struct hash_entry {
    void *key;
    void *val;
    struct hash_entry *next;
} hash_entry;

typedef struct hashmap_iterator {
    hashmap *h;
    int table;
    int safe;
    long index;
    hash_entry *entry, *next_entry;
}hashmap_iterator;

typedef struct hash_table {
    int used;
    int size;
    int sizemask;
    hash_entry **entry;
}hash_table;

typedef struct hashmap {
    int rehashidx;
    hash_type *ht;
    int iterators;
    int fingerprint;
    hash_table table[2];
} hashmap;

#define HASHMAP_SIZE(h) ((h)->table[0].used + (h)->table[1].used)
#define HASHMAP_SLOTS(h) ((h)->table[0].size + (h)->table[1].size)

#define HASHMAP_HASH_KEY(h, key) h->ht->hash_func(key)

#define HASHMAP_ENTRY_VAL(e) ((e)->val)
#define HASHMAP_ENTRY_KEY(e) ((e)->key)

#define HASHMAP_ENTRY_FREE_KEY(h, e) do { \
    if (h->ht->key_free) { \
        h->ht->key_free(e->key); \
    } \
} while (0)

#define HASHMAP_ENTRY_FREE_VAL(h, e) do { \
    if (h->ht->val_free) { \
        h->ht->val_free(e->val); \
    }\
} while(0)

#define HASHMAP_ENTRY_SET_KEY(h, e, __key__) do { \
    if (h->ht->key_dup) { \
        e->key = h->ht->key_dup(__key__); \
    } else { \
        e->key = __key__; \
    } \
} while(0)

#define HASHMAP_ENTRY_SET_VAL(h, e, __val__) do { \
    if (h->ht->val_dup) { \
        e->val = h->ht->val_dup(__val__); \
    } else { \
        e->val = __val__; \
    } \
} while(0)


#define HASHMAP_CMP_KEYS(h, key1, key2) \
    (((h)->ht->key_cmp) ? (h)->ht->key_cmp(key1, key2) : \
        (key1 == key2))
     
hashmap *hashmap_create(hash_type *ht);
void hashmap_release(hashmap *h);
int hashmap_add(hashmap *h, void *key, void *val);
int hashmap_delete(hashmap *h, const void *key);
int hashmap_delete_nofree(hashmap *h, const void *key);
int hashmap_expand(hashmap *h, unsigned long size);

void *hashmap_fetch_value(hashmap *h, const void *key);

int hashmap_resize(hashmap *h);

int hashmap_size(hashmap *h);

int hashmap_rehash(hashmap *h, int n);
int hashmap_rehash_millisecond(hashmap *h, int ms);

void hashmap_empty(hashmap *h);

hash_entry *hashmap_find(hashmap *h, void *key);

hash_entry *hashmap_get_random_key(hashmap *h);

hashmap_iterator *hashmap_get_iterator(hashmap *h);
void hashmap_release_iterator(hashmap_iterator *hi);
hash_entry *hashmap_next(hashmap_iterator *hi);

#endif /*__HASHMAP_H__*/
