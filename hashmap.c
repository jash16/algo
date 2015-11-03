#include "hashmap.h"
#include "share.h"

static void hash_table_init(hash_table *h);
static void hash_table_clear(hash_table *h);
static void hashmap_rehash_step(hashmap *h);
static int hashmap_expand_if_needed(hashmap *h);
static int hashmap_next_power(int n);

hashmap *hashmap_create(hash_type *ht) {
    hashmap *h = calloc(1, sizeof(hashmap));
    if (h == NULL) {
        return NULL;
    }
    h->rehashidx = -1;
    h->ht = ht;
    h->fingerprint = 0;
    h->iterators = 0;
    hash_table_init(&h->table[0]);
    hash_table_init(&h->table[1]);

    return h;
}

void hashmap_release(hashmap *h) {
    hash_table_clear(&h->table[0]);
    hash_table_clear(&h->table[1]);
    free(h);
    return;
}

hash_entry * hashmap_add_raw(hashmap *h, void *key) {
    int index;
    hash_table *ht;
    hash_entry *he;
    
    if(hashmap_rehashing(h)) { //正在rehash
        hashmap_rehash_step(h);
    }

    if ((index = hashmap_key_index(h, key)) == -1) {
        return NULL;
    }

    ht = h->rehashidx > -1 ? &h->table[1] : &h->table[0];
    he = calloc(1, sizeof(hash_entry));
    HASHMAP_ENTRY_SET_KEY(h, he, key);
    //HAHSMAP_ENTRY_SET_VAL(h, he, val);
    he->next = ht->entry[index];
    ht->entry[index] = he;

    ht->used ++;
    return he;
}

int hashmap_add(hashmap *h, void *key, void *val) {
    hash_entry *he;

    he = hashmap_add_raw(h, key);
    if (he == NULL) {
        return -1;
    }

    HASHMAP_ENTRY_SET_VAL(h, he, val);
    return 0;
}

int hashmap_expand(hashmap *h, unsigned long size) {
    hash_table table;
    
    if (hashmap_is_rehashing(h) || h->table[0].used > size) {
        return -1;
    }

    int real_size = hashmap_next_power(size);
    table.used = 0;
    table.size = real_size;
    table.sizemask = real_size - 1;
    table.entry = calloc(real_size, sizeof(hash_entry *));

    /*not in rehash, means the table[0] must be used or in initialize*/
    if (h->table[0].entry == NULL) {
        h->table[0] = table; //initialized
        return 0;
    }

    h->table[1] = table;
    h->rehashidx = 0; //start rehashing
    return 0;
}

int hashmap_resize(hashmap *h) {
    int minimal;
    
    if (hashmap_is_rehashing(h)) {
        return -1;
    }
    minimal = h->table[0].size;
    if (minimal < HASHMAP_DEFAULT_SIZE) {
        minimal = HASHMAP_DEFAULT_SIZE;
    }
    return hashmap_expand(h, minimal);
}

int hashmap_delete_nofree(hashmap *h, const void *key) {
    return hashmap_delete_generic(h, key, 1);
}

int hashmap_delete(hashmap *h, const void *key) {
    return hashmap_delete_generic(h, key, 0);
}

int hashmap_delete_generic(hashmap *h, const void *key, int nofree) {
    int index, idx, table;
    hash_entry *he, *heprev = NULL;

    if (h->table[0].size == 0) {
        return -1;
    }
    if (hashmap_is_rehashing(h)) {
        hashmap_rehash_step(h);
    }

    index = hashmap_key_index(h, key);
    for (table = 0; table < 1; table ++) {
        idx = h->table[table].sizemask & index;
        he = h->table[table].entry[idx];
        heprev = he;
        while (he) {
            if (HASHMAP_CMP_KEYS(h, key, he->key)) {
                if (heprev == NULL) {
                    h->table[table].entry[idx] = he->next;
                } else {
                    heprev->next = he->next;
                }
                if (!nofree) {
                    HASHMAP_ENTRY_FREE_KEY(h, he);
                    HASHMAP_ENTRY_FREE_VAL(h, he);
                }
                free(he);
                h->tablep[table].used --;
                return 0;
            }
            heprev = he;
            he = he->next;
        }
        if (!hashmap_is_rehashing(h)) {
            break;
        }
    }

    return -1;
}

void *hashmap_fetch_value(hashmap *h, const void *key) {
    hash_entry *he;

    he = hashmap_find(h, key);
    return he ? HASHMAP_ENTRY_VAL(he) : NULL;
}

hash_entry *hashmap_find(hashmap *h, const void *key) {
    int index, idx, table;
    hash_entry *he;
    hash_table *ht;

    if (h->table[0].size == 0) {
        return NULL;
    }
    
    if (hashmap_is_rehashing(h)) {
        hashmap_rehash_step(h);
    }

    index = HASHMAP_HASH_KEY(h, key);
    for (table = 0; table <= 1; table ++) {
        ht = &h->table[table];
        idx = index & ht->sizemask;
        he = ht->entry[idx];
        while (he) {
            if (HASHMAP_CMP_KEYS(h, key, he->key)) {
                return he;
            }
            he = he->next;
        }
        if (!hashmap_is_rehashing(h)) {
            break;
        }
    }

    return NULL;
}

void hashmap_empty(hashmap *h) {
    hash_table_clear(&h->table[0]);
    hash_table_clear(&h->table[1]);
    h->iterator = -1;
    h->fingerprint = -1;
    return;
}

int hash_rehash(hashmap *h, int n) {

    int table;
    int idx, index;
    hash_entry *he, *henext;
    hash_table *ht;

    if (!hashmap_is_rehashing(h)) {
        return -1;
    }
    while (n --) {
        if (h->table[0].used == 0) {
            free(h->table[0].entry);
            h->table[0] = h->table[1];
            hash_table_init(&h->table[1]);
            h->rehashidx = -1;
            return 0;
        }
        
        while ((he = h->table[0].entry[h->rehashidx]) == NULL) {
            h->rehashidx ++;
        }
       
        he = h->table[0].entry[h->rehashidx];

        while (he) {
            henext = he->next;
            index = HASHMAP_HASH_KEY(h, he->key);
            idx = index & h->table[1].sizemask;
            he->next = h->table[1].entry[idx];
            h->table[1].entry[idx] = he;
            he = henext;
            h->table[0].used --;
            h->table[1].used ++;
        }
        h->table[0].entry[h->rehashidx] = NULL;
        h->rehashidx ++;
    }
    return 1;
}

long long get_time_ms() {
    struct timeval tvl;
    gettimeofday(&tvl, NULL);
    return (((long long)tvl.tv_sec)*1000 + tvl.tv_usec / 1000);
}


int hashmap_rehash_millisecond(hashmap *h, int ms) {
    long long start = get_time_us();
    int rehashes = 0;

    while ((hashmap_rehash(h, 100))) {
        rehashes += 100;
        if (get_time_us() - start > ms) {
            break;
        }
    }
    return rehashes;
}

hash_entry *hashmap_get_random_key(hashmap *h) {
    int idx;
    int he_len, len;
    hash_entry *he, *henext;
    hash_table *ht;

    if (HASHMAP_SIZE(h) == 0) {
        return NULL;
    }
    if (hashmap_is_rehashing(h)) {
        hashmap_rehash_step(h);
    }

    if (hashmap_is_rehashing(h)) {
        do {
            index = random() % (h->table[0].size + h->table[1].size);
            he = index >= h->table[0].size ? h->table[1].entry[index - 
                         h->table[0].size] : &h->table[0].entry[index];
        } while(he == NULL)
    } else {
        do {
            index = random() % h->table[0].size;
            he = h->table[0].entry[index];
        } while(he == NULL)
    }
    
    henext = he;
    while (henext) {
        he_len ++;
        henext = henext->next;
    }
    len = random() % he_len;

    while (he_len --) {
        he = he->next;
    }
    return he;
}

hashmap_iterator *hashmap_get_iterator(hashmap *h) {
    hashmap_iterator *hi;

    hi = calloc(1, sizeof(hashmap_iterator));
    if (hi == NULL) {
       return NULL;
    }

    hi->h = h;
    hi->table = 0;
    hi->safe = 0;
    hi->index = -1;
    hi->entry = NULL;
    hi->next_entry = NULL;
}

hash_entry *hashmap_next(hashmap_iterator *hi) {
    hash_entry *he;
    hash_table *ht;

    while (1) {
        if (hi->entry == NULL) {
            ht = &hi->h->table[hi->table];
            hi->index ++;
            if (hi->index >= ht->size) {
                if (hashmap_is_rehashing(d) && hi->table == 0) {
                    hi->table ++;
                    hi->index = 0;
                    ht = &hi->h->table[1];
                } else {
                    break;
                }
            }
            hi->entry = ht->entry[hi->index];
        } else {
            hi->entry = hi->entry_next;
        }
        if (hi->entry) {
            hi->entry_next = hi->entry->next;
            return hi->entry;
        }
    }
    return NULL;
}

void hashmap_release_iterator(hashmap_iterator *hi) {
   free(hi);
}

/*all the private functions*/

static void hash_table_init(hash_table *h) {
    h->used = 0;
    h->size = 0;
    h->sizemask = 0;
    h->entry = NULL;
}

static void hash_table_clear(hashmap *h, hash_table *ht) { 
    hash_entry *he, *hn;
    int index = -1;

    for (index = 0; index < ht->size && ht->used > 0; index ++) {
        if ((he = ht->entry[index]) == NULL) {
            continue;
        } 
        while (he) {
            hn = he->next;
            HASHMAP_ENTRY_FREE_KEY(h, he);
            HASHMAP_ENTRY_FREE_VAL(h, he);
            free(he);
            he = hn;
            ht->used --;
        }
    }
    free(ht->entry);
    hash_table_init(ht->entry);
    return;
}

static int hashmap_key_index(hashmap *h, void *key) {
    int index, idx, table;
    hash_entry *he;
    hash_table *ht;

    if (hashmap_expand_if_needed(h) < 0) {
        return -1;
    }
    index = HASHMAP_HASH_KEY(h, key);

    for (table = 0; table <= 1; table ++) {
        idx = index & h->table[table].sizemask;
        he = h->table[table].entry[idx];
        while (he) {
            if (HASHMAP_CMP_KEYS(h, he->key, key)) {
                return -1;
            }
            he = he->next;
        }
        //decide table[0] or table[1]
        if (!hashmap_is_rehashing(h)) {
            break;
        }
    }
    return idx;
}

static int hashmap_expand_if_needed(hashmap *h) {
    /*now is rehashing*/
    if (hashmap_is_rehashing(h)) {
        return 0;
    }
    if (h->table[0].used >= h->table[0].size) {
        return hashmap_expand(h, h->table[0].size * 2);
    }
    return 0;
}

static int hashmap_next_power(hashmap *h, int n) {
    int i = 1;
    while (i < n) {
        i = i * 2;
    }
    return i;
}

static int hashmap_rehash_step(hashmap *h) {
    if (d->iterators == 0) {
        hashmap_rehash(h, 1);
    } 
}

#ifdef HASHMAP_TEST

#include <stdio.h>
#include <stdlib.h>

int main() {

}

#endif /*HASHMAP_TEST*/
