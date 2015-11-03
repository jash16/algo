#ifndef __FIBHEAP_H__
#define __FIBHEAP_H__

typedef struct fibtype {
    void *(key_dup)(void *key);
    int (*key_cmp)(void *key1, void *key2);
    void (key_free)(void *key);
}fibtype;

typedef struct fibnode {
    void *key;
    int degree;
    struct fibnode *left;
    struct fibnode *right;
    struct fibnode *child;
    struct fibnode *parent;
    int marked;
}fibnode;

typedef struct fibheap {
    int max_degree;
    int keynum;
    struct fibnode *min;
    struct fibnode **cons;
    fibtype *type;
}fibheap;


#define FIBNODE_KEY_SET(t, n, key) do {\
    if (t->type->key_dup) {\
        n->key = t->type->key_dup(key); \
    } else {\
        n->key = key;\
    }\
}while(0)

#define FIBNODE_FREE_KEY(t, n) do { \
    if (t->type->key_free) {\
        t->type->key_free(n->key); \
    } \
} while(0) 

#define FIBNODE_FREE(t, n) do {\
    if (t->type->key_free) {\
        t->type->key_free(n->key); \
    }\
    free(n); \
}while(0)

fibnode *fibnode_create(fibheap *t, void *key);
fibheap *fibtree_create(fibtype *type);
void *fibtree_destroy(fibtree *t);

#endif /*__FIBHEAP_H__*/
