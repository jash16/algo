#include "share.h"
#include "rbtree.h"
#include "pair.h"

static void _rbtree_delete_balance(rbtree *t, rbnode *x);
static void _rbtree_insert_balance(rbtree *t, rbnode *x);
static void _rbtree_left_rotate(rbtree *t, rbnode *z);
static void _rbtree_right_rotate(rbtree *t, rbnode *z);
static void _rbtree_transplant(rbtree *t, rbnode *u, rbnode *v);

static void _insert_unique(rbtree *t, void *key, void *value);

static void _rbtree_release_recur(rbtree *t, rbnode *root);

static rbnode *_rbtree_minimum(rbtree *t, rbnode *x);
static rbnode *_rbtree_maximum(rbtree *t, rbnode *x);

rbtree *rbtree_create(rbtree_type *rt) {
    rbtree *t = (rbtree *)calloc(sizeof(rbtree), 1);
    if (t == NULL) {
        return NULL;
    }

    t->nil.val = NULL;
//    t->nil.key = NULL;
    t->nil.color = BLACK;
    t->nil.parent = NULL;
    t->nil.left = NULL;
    t->nil.right = NULL;

    t->root = &t->nil;
    t->rt = rt;

    return t;
}

static void _rbtree_release_recur(rbtree *t, rbnode *root) {
    rbtree_type *rt = t->rt;
    if (root == &t->nil) {
        return;
    }
    if (root->left != &t->nil) {
        _rbtree_release_recur(t, root->left);
    }
    if (root->right != &t->nil) {
        _rbtree_release_recur(t, root->right);
    }
    rbnode_free(root, rt);
    return;
}

void rbtree_release(rbtree *t) {
    rbnode *node = t->root;
    _rbtree_release_recur(t, node);   
    free(t); 
    return;
}

static rbnode *_rbtree_maximum(rbtree *t, rbnode *x) {
    rbnode *y = x;
    while (x != &t->nil) {
        y = x;
        x = x->right;
    }
    return y;
}

static rbnode *_rbtree_minimum(rbtree *t, rbnode *x) {
    rbnode *y = x;
    while (x != &t->nil) {
        y = x;
        x = x->left;
    }
    return y;
}

static void _rbtree_left_rotate(rbtree *t, rbnode *z) {
    rbnode *y;

    y = z->right;
    z->right = y->left;
    if (y->left != &t->nil) {
        y->left->parent = z;
    }
    y->parent = z->parent;

    if (z->parent == &t->nil) {
        t->root = y;
    } else if (z == z->parent->left) {
        z->parent->left = y;
    } else {
        z->parent->right = y;
    }

    y->left = z;
    z->parent = y;
    return;
}

static void _rbtree_right_rotate(rbtree *t, rbnode *x) {
    rbnode *y;

    y = x->left;

    x->left = y->right;
    if (y->right != &t->nil) {
        y->right->parent = x;
    }

    y->parent = x->parent;
    if (x->parent == &t->nil) {
        t->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
    return;
} 

rbnode *rbtree_root(rbtree *t) {
    if (t->root != &t->nil) {
        return t->root;
    }
    return NULL;
}

rbnode *rbtree_clone_node(rbtree *t, rbnode *node) {

    void *key, *val;
    rbnode *new;
    rbtree_type *rt ;
    rt = t->rt;

    new = (rbnode *)calloc(sizeof(rbnode), 1);
    if (new == NULL) {
        return NULL;
    }
    if (rt->key_dup) {
        key = rt->key_dup(rt->key_of_val(node->val));
    } else {
        key = rt->key_of_val(node->val);
    }
    if (rt->val_dup) {
        val = rt->val_dup(rt->val_of_val(node->val));
    } else {
        val = rt->val_of_val(node->val);
    }
    new->val = make_pair(key, val, NULL);
    new->color = node->color;
    new->left = &t->nil;
    new->right = &t->nil;
    new->parent = &t->nil;
    return new;
}

rbnode *rbtree_find(rbtree *t, const void *key) {
    rbnode *x;
    rbtree_type *rt;
    int ret;

    x = t->root;
    rt = t->rt;

    while (x != &t->nil) {
        ret = rt->key_cmp(rt->key_of_val(x->val), key);
        if (ret == 0) {
            return x;
        } else if (ret > 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return NULL;
}

bool rbtree_insert(rbtree *t, void *key, void *value) {
    rbnode *x = NULL;
    rbnode *y = NULL;
    rbnode *z = NULL;
    pair *p = NULL;

    int ret = 0;

    rbtree_type *rt = t->rt;
    x = t->root;
    y = x;
    while (x != &t->nil) {
        y = x;
        p = x->val;

        ret = rt->key_cmp(rt->key_of_val(p), key);

        if (ret == 0) { //等于
            if (rt->val_free) rt->val_free(rt->val_of_val(x->val));
            p->second = value;
            return;
        } else if (ret < 0) {
            x = x->right;
        } else {
            x = x->left;
        }
    }
    
    z = (rbnode *)calloc(sizeof(rbnode), 1);
    if (z == NULL) {
        return false;
    }
    z->parent = y;

    if (y == &t->nil) {
        t->root = z;
    } else {
        if (rt->key_cmp(rt->key_of_val(p), key) > 0) {
            y->left = z;
        } else {
            y->right = z;
        }
    }

//    z->key = key;
//    z->val = value;
    p = make_pair(key, value, NULL);
    z->val = p;
    z->left = &t->nil;
    z->right = &t->nil;
    z->color = RED;
    t->size ++;
    _rbtree_insert_balance(t, z);

    return true;
}

static void _rbtree_insert_balance(rbtree *t, rbnode *z) {
    rbnode *w;
    
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) { //左子树
            w = z->parent->parent->right;
            if (w->color == RED) { //叔叔为红色
                z->parent->color = BLACK;
                w->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else { //叔叔为黑色
                if (z == z->parent->right) {
                    z = z->parent;
                    _rbtree_left_rotate(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                _rbtree_right_rotate(t, z->parent->parent);
            }
        } else {  //右子树
            w = z->parent->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    _rbtree_right_rotate(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                _rbtree_left_rotate(t, z->parent->parent);
            }
        }    
    }

    t->root->color = BLACK;
    return;
}

static void _rbtree_transplant(rbtree *t, rbnode *u, rbnode *v) {
    if (u->parent == &t->nil) {
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
    return;
}

static void _rbtree_delete_balance(rbtree *t, rbnode *x) {
    rbnode *w;

    while (x != t->root && x->color == BLACK) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                _rbtree_left_rotate(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) { //left is RED
                    w->color = RED;
                    w->left->color = BLACK;
                    _rbtree_right_rotate(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                _rbtree_left_rotate(t, x->parent);
                x = t->root;
            }
        } else {
            w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                _rbtree_right_rotate(t, x->parent);
                w = x->parent->left;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    _rbtree_left_rotate(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                _rbtree_right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}

bool rbtree_delete(rbtree *t, void *key) {
    rbnode *z;
    rbnode *y;
    rbnode *x;
    rbtree_type *rt;
    
    int y_origin_color;

    z = rbtree_find(t, key);
    if (z == NULL) {
        return false;
    }
    
    y = z;
    y_origin_color = y->color;

    if (z->left == &t->nil) {
         x = z->right;
         _rbtree_transplant(t, z, z->right);
    } else if (z->right == &t->nil) {
         x = z->left;
         _rbtree_transplant(t, z, z->left);
    } else {
        y = _rbtree_minimum(t, z->right);
        y_origin_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x->parent = y;          
        } else {
            _rbtree_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        _rbtree_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_origin_color == BLACK) {
        _rbtree_delete_balance(t, x);
    }
    rt = t->rt;
    rbnode_free(z, rt);
    return true;
}

int rbtree_count(rbtree *t, const void *key) {
    rbnode *node = NULL;
    int count = 0;
    int ret = 0;

    rbtree_type *rt;

    rt = t->rt;
    node = t->root;

    while (node != &t->nil) {
        ret = rt->key_cmp(rt->key_of_val(node->val), key);
        if (ret > 0) {
            node = node->left;
        } else if (ret < 0) {
            node = node->right;
        } else {
            count ++;
            node = node->right;
        }
    }
    return count;
}

void rbtree_clear(rbtree *t) {
//    rbnode *node = t->root;
    _rbtree_release_recur(t, t->root);
    t->root = &t->nil;
    t->size = 0;
}

rbtree_iterator *rbtree_get_iterator(rbtree *t) {
    rbtree_iterator *ri;

    ri = (rbtree_iterator *)calloc(sizeof(rbtree_iterator), 1);
    if (ri == NULL) {
        return NULL;
    }
    ri->t = t;
    ri->node = _rbtree_minimum(t, t->root);
    ri->begin = 0;
    ri->end = 0;
    return ri;
}

rbnode *rbtree_next(rbtree_iterator *ri) {
    rbtree *t;
    rbnode *node, *parent;
    rbnode *ret;

    t = ri->t;
    node = ri->node;
    if (node == NULL || node == &t->nil) {
        return NULL;
    }    
    ret = node;

    if (node->right != &t->nil) {
        node = node->right;
        while (node->left != &t->nil) {
            node = node->left;
        }
        ri->node = node; //更改ri->node
        ri->end = 0;
        return ret;
    } else {
        parent = node->parent;

        //找到第一个大于该节点的节点
        while (parent != &t->nil && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }
        if (parent != &t->nil) {
            ri->node = parent; //更改ri->node
            ri->end = 0;
            return ret;
        }
        if (ri->end == 0) {
            ri->end = 1;
            return ret;
        }
        //当前节点已经没有后继了，返回NULL
        return NULL;
    }
    return NULL;
}

rbnode *rbtree_prev(rbtree_iterator *ri) {
    rbtree *t;
    rbnode *ret;
    rbnode *node, *parent;

    t = ri->t;
    node = ri->node;
    ret = node;

    if (node == NULL || node == &t->nil) {
        return NULL;
    }

    if (node->left != &t->nil) {
        node = node->left;
        while (node->right != &t->nil) {
            node = node->right;
        }
        ri->node = node;
        ri->begin = 0;
        return ret;
    
    } else { //找到第一个小于它的
        parent = node->parent;
        while (parent != &t->nil && node == parent->left) {
            node = parent;
            parent = parent->parent;
        }
        if (parent != &t->nil) {
            ri->node = parent;
            ri->begin = 0;
            return ret;
        }
        
        if (ri->begin == 0) {
            ri->begin = 1;
            return ret;
        }
        return NULL;
    }
    return NULL;
}

void rbtree_release_iterator(rbtree_iterator *ri) {
    free(ri);
    return;
}

#ifdef RBTREE_TEST
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void tree_pre_visit(rbtree *t, rbnode *root) {
    rbtree_type *rt = t->rt;
    if (root != &t->nil) {
        printf("key: %s, value: %s color: %s\n", 
                (char *)(rt->key_of_val(root->val)), (char *)(rt->val_of_val(root->val)), root->color == RED? "red" : "black");
        tree_pre_visit(t, root->left);
        tree_pre_visit(t, root->right);
    }
}

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

char *key_dup(const void *key) {
    char *k = (char *)key;
    return strdup(k);
}

char *val_dup(const void *val) {
    char *v = (char *)val;
    return strdup(v);
}

void key_free(void *key) {
    free(key);
    return;
}

void val_free(void *val) {
    free(val);
    return;
}

void *key_of_val(const void *val) {
    pair *p = (pair *)val;
    return p->first;
}

void *val_of_val(const void *val) {
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
    rbtree *t = rbtree_create(&demo_type);
    assert(t != NULL);

    rbtree_type *rt = t->rt;
    
    char *key = NULL;
    char *val = NULL;
    srand(time(NULL));
    int i = 0, j = 0;
    for (i = 0; i < 10000000; i++) {
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
        rbtree_insert(t, key, val);
    }
//    tree_pre_visit(t, t->root);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    rbnode *n = rbtree_find(t, "yyyydmfcrdpywik");
    gettimeofday(&end, NULL);
    int waist = (end.tv_sec - start.tv_sec) * 100000 + (end.tv_usec - start.tv_usec);
    printf("waist time: %d\n", waist);
    assert(n == NULL);

    int count = rbtree_count(t, "xello");
    printf("key[hello] count: %d\n", count);

    
    rbtree_iterator *ri = rbtree_get_iterator(t);
    
    printf("============next===============\n");
    while ((n = rbtree_next(ri)) != NULL) {
        printf("key: %s, value: %s\n", (char *)(rt->key_of_val(n->val)), (char *)(rt->val_of_val(n->val)));
    }

    printf("============prev==============\n");
    while ((n = rbtree_prev(ri)) != NULL) {
        printf("key: %s, value: %s\n", (char *)(rt->key_of_val(n->val)), (char *)(rt->val_of_val(n->val)));
    }
 
 
    char *key_find[1000];
    for (i = 0; i < 1000; i++) {
        key_find[i] = malloc(sizeof(char) * 16);
        for (j = 0; j < 15; j ++) {
            key[j] = 'a' + rand_range(0, 25);
        }
        key_find[i][15] = '\0';
    }

    gettimeofday(&start, NULL);
    for (i = 0; i < 1000; i++) {
        rbnode *n = rbtree_find(t, key_find[i]);
        if (n != NULL) {
            printf("find one: key: %s\n", key_find[i]);
        }
        free(key_find[i]);
    }
    gettimeofday(&end, NULL);

    waist = (end.tv_sec - start.tv_sec) * 100000 + (end.tv_usec - start.tv_usec);
    printf("waist time: %d\n", waist);
    rbtree_release_iterator(ri);
    rbtree_clear(t);
    rbtree_release(t);

}

#endif /*RBTREE_TEST*/
