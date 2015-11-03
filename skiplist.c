#include "skiplist.h"
#include "share.h"

int random_level() {
    int k = 1;
    while (rand() % 2) {
        k ++;
    }

    return k < MAX_LEVEL ? k : MAX_LEVEL;
}

skipnode *skipnode_create(int level, void *key, void *val) {
    int i = 0;
    skipnode *sn;

    sn = (skipnode *)calloc(sizeof(skipnode) + sizeof(void *) * level, 1);
    if (sn == NULL) {
        return NULL;
    }
    
    for (i = 0; i < level; i++) {
        sn->forward[i] = NULL;
    }
    sn->key = key;
    sn->val = val;
    return sn;
}

skiplist *skiplist_create(skiplist_type *st) {
    int i = 0;
    skiplist *sl;

    sl = (skiplist *)calloc(sizeof(skiplist), 1);
    if (sl == NULL) {
        return NULL;
    }
    sl->level = 0;
    sl->length = 0;
    sl->st = st;
    sl->header = skipnode_create(MAX_LEVEL, NULL, NULL);
    if (sl->header == NULL) {
        free(sl);
        return NULL;
    }
    return sl;
}

void skiplist_release(skiplist *sl) {
    skipnode *p, *q;

    skiplist_type *st;

    st = sl->st;
    p = sl->header;

    while (p) {
        q = p->forward[0];
        if (st->key_free) {
            st->key_free(p->key);
        }   
        if (st->val_free) {
            st->val_free(p->val);
        }
        free(p);
        p = q;
    }
    free(sl);
    return;
}

void skiplist_visit(skiplist *sl) {

    skipnode *p, *q;

    p = sl->header;

    while (p) {
        q = p->forward[0];
        if (q == NULL) {
            break;
        }
        printf("key: %d, val: %s\n", *(int *)q->key, (char *)q->val);
        p = q;
    }
}

skipnode *skiplist_search(skiplist *sl, void *key) {
    int i, ret;
    int level = sl->level;
    skiplist_type *st;
    skipnode *p, *q = NULL;
    
    p = sl->header;
    st = sl->st;
    for (i = level - 1; i >= 0; i --) {
        while ((q = p->forward[i])) {
            ret = st->key_cmp(q->key, key);
            if (ret == 0) {
                return q;
            } else if (ret < 0) {
                p = q;
            } else { //forward[i]->key > key, down
                break;
            }
        }
    }
    return NULL;
}

bool skiplist_insert(skiplist *sl, void *key, void *val) {
    int level, i, ret;
    int new_level;
    skipnode *update[MAX_LEVEL];
    skipnode *p, *q = NULL;
    skipnode *new_node = NULL;
    skiplist_type *st = sl->st;

    level = sl->level;
    p = sl->header;

    for (i = level-1; i >= 0; i--) {
        while ((q = p->forward[i])) {
            ret = st->key_cmp(q->key, key);
            if (ret <= 0) { //允许元素相同，后来的插入在后面
                p = q;
            } else {
                break;
            }
        }
        update[i] = p;
    }
    
    new_level = random_level();
    if (new_level > sl->level) {
        for (i = sl->level; i < new_level; i++) {
            update[i] = sl->header;
        }
        sl->level = new_level;
    }
    
    new_node = skipnode_create(new_level, key, val);
    if (new_node == NULL) {
        return false;
    }
    
    for (i = 0; i < new_level; i ++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }
    sl->length ++;
    return true;
}

bool skiplist_delete(skiplist *sl, void *key) {
    skipnode *update[MAX_LEVEL];
    skipnode *p, *q = NULL;
    int level, i, ret;
    skiplist_type *st;

    level = sl->level;
    p = sl->header;
    st = sl->st;

    for (i = level - 1; i >= 0; i--) {
        while ((q = p->forward[i])) {
            ret = st->key_cmp(q->key, key);
            if (ret < 0) {
                p = q;
            } else {
                break;
            }
        }
        update[i] = p;
    }
    if (!q) {
        return false;
    }
    
    ret = st->key_cmp(q->key, key);
    if (ret != 0) {
        return false;
    }

    for (i = 0; i < sl->level; i++) {
        if (update[i]->forward[i] == q) {
            update[i]->forward[i] = q->forward[i];
        }
    }
    for (i = sl->level - 1; i >= 0; i--) {
        if (sl->header->forward[i] == NULL) {
            sl->level --;
        }
    }
    sl->length --;
    return true;
}

#ifdef SKIPLIST_TEST
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

int key_cmp(void *key1, void *key2) {
    int ikey1 = *(int *)key1;
    int ikey2 = *(int *)key2;

    return ikey1 > ikey2 ? 1 : (ikey1 == ikey2 ? 0 : -1); 
}

void val_free(void *val) {
    free(val);
    return;
}

void key_free(void *key) {
    free(key);
    return;
}

skiplist_type demo_type = {
    .key_free = key_free,
    .val_free = val_free,
    .key_cmp = key_cmp,
    .val_cmp = NULL,
};


int rand_range(int a, int b) {
    return rand() % (b - a) + a;
}

int main() {
    skiplist *sl = skiplist_create(&demo_type);
    assert(sl != NULL);

    int i = 0, j = 0;
    int *key = NULL;
    char *val = NULL;
    srand(time(NULL));
    for (i = 0; i < 50000; i++) {
        key = (int *)malloc(sizeof(int));
        *key = rand_range(0, 10000000);
        val = malloc(sizeof(char) * 16);
        for (j = 0; j < 2; j ++) {
            val[j] = 'a' + rand_range(0, 25);
        }
        val[15] = '\0';
        skiplist_insert(sl, key, val);
    }
    printf("%d\n", sl->length);
    int a = 100000;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    skipnode *sn = skiplist_search(sl, &a);
    gettimeofday(&end, NULL); 
    int waist = (end.tv_sec - start.tv_sec) * 100000 + (end.tv_usec - start.tv_usec);
    printf("waist time: %d\n", waist);
 
    skiplist_visit(sl);
    skiplist_release(sl);
    return 0;
}

#endif /*SKIPLIST_TEST*/
