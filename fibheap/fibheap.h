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

typedef struct fibtree {
    int max_degree;
    int keynum;
    struct fibnode *min;
    struct fibnode **cons;
    fibtype *type;
}fibtree;


fibnode *fibnode_create(void *key);
fibtree *fibtree_create(fibtype *t);

#endif /*__FIBHEAP_H__*/
