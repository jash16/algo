#include "fibheap.h"
#include <stdlib.h>
#include <math.h>

#define LOG2(x) log(x)/log(2)

static void _fibnode_add(fibnode *n, fibnode *root);
static void _fibnode_remove(fibnode *n);
static void _fibheap_insert_node(fibheap *h, fibnode *n);
static void _fibnode_cat(fibnode *a, fibnode *b);
static void _fibheap_link(fibheap *h, fibnode *n, fibnode *r);
static void _fibheap_cons_make(fibheap *h);
static fibnode *_fibnode_remove_min(fibheap *h);
static void _fibheap_consolidate(fibheap *h);

static void _fibnode_add(fibnode *n, fibnode *root) {
    n->left = root->left;
    root->left->right = n;
    n->right = root;
    root->left = n;
    return;
}

static void _fibnode_remove(fibnode *n) {
    n->left->right = n->right;
    n->right->left = n->left;
}

static void _fibheap_insert_node(fibheap *h, fibnode *n) {
    if (h->min == NULL) {
        h->min = node;

    } else {
        _fibnode_add(node, h->min);
        if (h->type->key_cmp(node->key, h->min->key) < 0) {
            h->min = node;
        }
    }
    h->keynum ++;
}

static void _fibnode_cat(fibnode *a, fibnode *b) {
    fibnode *tmp;

    tmp = a->right;
    a->right = b->right;
    b->right->left = a;
    b->right = tmp;
    tmp->left = a;

    return;
}

static void _fibheap_link(fibheap *h, fibnode *n, fibnode *r) {
    _fibnode_remove(n);
    
    if (root->child == NULL) {
        r->child = n;
    } else {
        _fibnode_add(n, r->child);
    }

    n->parent = r;
    r->degree ++; //degree is the child number of r
    n->marked = 0;
}

static fibnode *fibnode_remove_min(fibheap *h) {
    fibnode *min;

    if (h == NULL || h->min == NULL) {
        return NULL;
    }

    min = h->min;
    if (min->right == min) {
        h->min = NULL;
    } else {
        _fibnode_remove(min);
        h->min = min->right;
    }
    min->left = min->right = min;

    return min;
}

static void _fibheap_cons_make(fibheap *h) {
    int old = h->max_degree;

    h->max_degree = LOG2(h->key_num) + 1;
    if (old > h->max_degree) {
        return;
    }

    h->cons = (fibnode **)realloc(h->cons, sizeof(fibnode *) * (h->max_degree + 1));
}

fibnode *fibnode_create(fibheap *t, void *key) {
    fibnode *n = calloc(1, sizeof(fibnode));
    if (n == NULL) {
        return NULL;
    }
    n->degree = 0;
    n->left = n;
    n->right = n;
    n->parent = NULL;
    n->child = NULL;
    n->marked = 0;
    FIBNODE_SET_KEY(t, n, key);
    return n;
}

fibheap *fibheap_create(fibtype *type) {
    fibheap *t = calloc(1, sizeof(fibheap));
    if (t == NULL) {
        return NULL;
    }

    t->min = NULL;
    t->cons = NULL;
    t->type = type;
    t->max_degree = 0;
    t->key_num = 0;
    return t;
}

fibheap *fibheap_union(fibheap *h1, fibheap *h2) {
    fibheap *tmp = NULL;

    if (h1 == NULL) {
        return h2;
    }
    if (h2 == NULL) {
        return h1;
    }

    if (h2->max_degree > h1->max_degree) {
        tmp = h1;
        h1 = h2;
        h2 = tmp;
    }

    if (h1->min == NULL) {
        h1->min = NULL;
        h1->key_num = h2->key_num;
        free(h2->cons);
        free(h2);
    
    } else if (h2->min == NULL) {
        free(h2->cons);
        free(h2);
    
    } else {
        _fibnode_cat(h1->min, h2->min);
        if (h1->type->key_cmp(h1->min->key, h2->min->key) > 0) {
            FIBNODE_SET_KEY(h1, h1->min, h2->min->key);
        }
        h1->key_num += h2->key_num;
        free(h2->cons);
        free(h2);
    }
    return h1;
}

static void _fibheap_consolidate(fibheap *h) {
    int i, d, D;
    fibnode *x, *y, *tmp;

    _fibheap_cons_make(h);
    D = h->max_degree + 1;

    for (i = 0; i < D; i ++) {
        h->cons[i] = NULL;
    }
    
    while (h->min != NULL) {
        x = _fibnode_remove_min(h);
        d = x->degree;
        while (h->cons[d] != NULL) {
            y = h->cons[d];
            if (h->type->key_cmp(x->key, y->key) > 0) {
                tmp = x;
                x = y;
                y = tmp;
            }
            _fibheap_link(h, y, x);
            //x's degree add 1
            h->cons[d] = NULL;
            d ++;
        }
        h->cons[d] = x;
    }
    h->min = NULL;
    for (i = 0; i < D; i ++) {
        if (h->min == NULL) {
            h->min = h->cons[i];
        } else {
            _fibnode_add(h->cons[i], h->min);
            if (h->type->key_cmp(h->cons[i]->key, h->min->key) < 0) {
                h->min = h->cons[i];
            }
        }
    }
}

fibnode *fibheap_extract_min(fibheap *h) {
    fibnode *child, *min;

    if (h == NULL || h->min == NULL) {
        return NULL;
    }
    
    min = h->min;
    while (min->child != NULL) {
        child = min->child;
        _fibnode_remove(child);       
        if (child->right == child) {
            min->child = NULL;
        } else {
            min->child = child->right;
        }
        _fibnode_add(h->min, child);
        child->parent = NULL;
    }

    _fibnode_remove(h->min);

    if (h->min->right == h->min) {
        h->min = NULL;
    } else {
        h->min = min->right;
        _fibheap_consolidate(h);
    }
    h->key_num --;
    return min;
}

