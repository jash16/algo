#ifndef __RBTREE_H__
#define __RBTREE_H__

//#include "share.h"
#include "bool.h"

#define RED 0
#define BLACK 1

typedef struct rbtree_iterator {
    struct rbnode *node;
    struct rbtree *t;
    int begin;
    int end;
}rbtree_iterator;

typedef struct rbtree_type {
    int (*key_cmp)(const void *key1, const void *key2);
    int (*val_cmp)(const void *val1, const void *val2);
    void *(*key_dup)(const void *key);
    void *(*val_dup)(const void *val);
    void (*key_free)(void *key);
    void (*val_free)(void *val);
    void *(*key_of_val)(const void *val);
    void *(*val_of_val)(const void *val);
} rbtree_type;

typedef struct rbnode {
//    void *key;
    void *val;
    int color;
    struct rbnode *parent;
    struct rbnode *left;
    struct rbnode *right;
} rbnode;

typedef struct rbtree {
    rbnode *root;
    rbnode nil;   
    rbtree_type *rt;
    int size;
}rbtree;

rbtree *rbtree_create(rbtree_type *rt);
void rbtree_release(rbtree *t);
void rbtree_clear(rbtree *t);

bool rbtree_insert(rbtree *t, void *key, void *data);

/*how does multi key*/
bool rbtree_delete(rbtree *t, void *key);

int rbtree_count(rbtree *t, const void *key);

rbnode *rbtree_find(rbtree *t, const void *key);

rbnode *rbtree_root(rbtree *t);

rbnode *rbtree_clone_node(rbtree *rt, rbnode *node);

rbtree_iterator *rbtree_get_iterator(rbtree *t);
rbnode *rbtree_next(rbtree_iterator *ri); //后继
rbnode *rbtree_prev(rbtree_iterator *ri); //前驱
void rbtree_release_iterator(rbtree_iterator *ri);

#define rbnode_free(n, rt) do {\
    if (rt->key_free) {\
        rt->key_free(rt->key_of_val(n->val)); \
    }\
    if (rt->val_free) { \
        rt->val_free(rt->val_of_val(n->val));\
    }\
    free(n->val); \
    free(n); \
} while(0)

#define RBTREE_SIZE(t) ((t)->size)
#define RBNODE_KEY(n) ((n)->key)
#define RBNODE_VAL(n) ((n)->val)
#define RBNODE_PARENT(n) ((n)->parent)
#define RBNODE_LCHILD(n) ((n)->left)
#define RBNODE_RCHILD(n) ((n)->right)

#define RBNODE_COLOR(n) ((n)->color)
#define RBNODE_BLACK(n) ((n)->color = BLACK)
#define RBNODE_RED(n) ((n)->color = RED)

#endif  /*__RBTREE_H__*/
