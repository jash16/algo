#include "share.h"
#include "vector.h"

vector *vector_create(int size, vector_type *vt) {
    int i = 0;
    int vsize = 0;
    vector *v = NULL;
    
    vsize = size < VECTOR_DEFAULT_SIZE ? VECTOR_DEFAULT_SIZE : size;

    v = (vector *)malloc(sizeof(vector));
    if (v == NULL) {
        return NULL;
    }
    v->size = vsize;
    v->used = 0;
    v->vt = vt;
    v->ele = (void **)calloc(sizeof(void *) * vsize, 1);
    if (v->ele == NULL) {
        free(v);
        return NULL;
    }
    
    return v;
}

void vector_release(vector *v) {
    int i = 0;
    vector_type *vt;
    vt = v->vt;
    if (vt && vt->free) {
        for (i = 0; i < v->used; i++) {
            vt->free(v->ele[i]);
        }
    }

    free(v->ele);
    free(v);

    return;
}

bool vector_clear(vector *v) {
    int i = 0;
    void **ptr = NULL;
    vector_type *vt= v->vt;
    
    if (vt && vt->free) {
        for (i = 0; i < v->used; i++) {
            vt->free(v->ele[i]);
        }   
    }

    if (v->size > VECTOR_DEFAULT_SIZE) {
        ptr = (void **)realloc(v->ele, VECTOR_DEFAULT_SIZE * sizeof(void *));
        if (ptr == NULL) {
            return false;
        }
        v->ele = ptr;
        v->size = VECTOR_DEFAULT_SIZE;
    }

    for (i = 0; i < v->size; i++) {
        v->ele[i] = NULL;
    }
    v->used = 0;
    return true;
}

void vector_erase(vector *v, int pos1, int pos2) {
    int size = 0;
    int used = 0;
    int i = 0, j = 0;

    used = v->used;
    size = v->size;

    if (pos1 > pos2) {
        int tmp = pos1;
        pos1 = pos2;
        pos2 = tmp;
    }
    if (pos1 > used) {
        return;
    }

    if (pos2 > used) {
        pos2 = used;
    }

    j = pos1;
    for (i = pos2+1; i < used; i ++) {
        v->ele[j] = v->ele[i];
        j ++;
    }
    /*
    for (i = pos1, j = pos2; i < pos2 && j < used; i ++, j ++) {
        v->ele[i] = v->ele[j];   
    }
    */
    v->used -= pos2 - pos1 + 1;
    return;
}

bool vector_insert(vector *v, int pos, int n, void *data) {
    int i = 0;
    int size = 0;
    int used = 0;
    void **ptr = NULL;

    used = v->used;
    size = v->size;

    //不允许出现空洞
    if (used < pos) {
        return false;
    }
    if (used + n >= size) {
        while (size < (used + n)) {
            size = size * 2;
        }
        ptr = (void **)realloc(v->ele, size * sizeof(void *));
        if (ptr == NULL) {
            return false;
        }
        v->ele = ptr;
        v->size = size;
    }

    for (i = pos; i < used; i++) {
        v->ele[i+n] = v->ele[i];
    }
    for (i = pos; i < pos + n; i++) {
        v->ele[i] = data;
    }
    v->used += n;
    return true;
}

vector *vector_reinit(vector *v) {
    int i = 0;
    void **ptr;

    v->used = 0;

    if (v->size > VECTOR_DEFAULT_SIZE) {
        ptr = (void **)realloc(v->ele, VECTOR_DEFAULT_SIZE * sizeof(void *));
        if (ptr == NULL) {
            return v;
        }
        v->ele = ptr;
        v->size = VECTOR_DEFAULT_SIZE;
    }
    
    for (i = 0; i < v->size; i++) {
        v->ele[i] = NULL;
    }
    return v;
}

vector_iterator *vector_iterator_get(vector *v) {
    vector_iterator *vi;

    if (v == NULL) {
        return NULL;
    }
    
    vi = (vector_iterator *)malloc(sizeof(vector_iterator));
    if (vi == NULL) {
        return vi;
    }
    vi->v = v;
    vi->idx = 0;
}

void vector_iterator_release(vector_iterator *vi) {
    free(vi);
    return;
}

void vector_iterator_rewind(vector_iterator *vi) {
    vi->idx = 0;
    return;
}

void *vector_next(vector_iterator *vi) {

    void *ptr = NULL;
    vector *v = vi->v;

    if (vi == NULL || vi->idx >= v->used) {
        return NULL;
    }
    
    ptr = v->ele[vi->idx];
    vi->idx ++;
    return ptr;
}

bool vector_push(vector *v, void *data) {
    void **ptr;
    int size;
    int used;

    size = v->size;
    used = v->used;
    if (used >= size) {
        size = size * 2;
        ptr = (void **)realloc(v->ele, size * sizeof(void *));
        if (ptr == NULL) {
            return false;
        }
        v->ele = ptr;
        v->size = size;
    }
    v->ele[used] = data;
    v->used ++;
    return true;
}

void *vector_pop(vector *v) {
    int size = v->size;
    int used = v->used;
    void *ptr;

    if (used == 0) {
        return NULL;
    }
    ptr = v->ele[used-1];
    v->used --;
    return ptr;
}

void *vector_top(vector *v) {
    int size = v->size;
    int used = v->used;
    void *ptr;

    if (used == 0) {
        return NULL;
    }

    ptr = v->ele[used - 1];
    return ptr;
}

void *vector_get_idx(vector *v, int idx) {
    int size = v->size;
    int used = v->used;
    void *ptr;

    if (used == 0 && idx >= used) {
        return NULL;
    }
    
    ptr = v->ele[idx];
    return ptr;
}

#ifdef VECTOR_TEST

#include <stdio.h>
#include <assert.h>
int int_cmp(void *ptr1, void *ptr2) {
    int i1 = *(int *)ptr1;
    int i2 = *(int *)ptr2;

    return i1 > i2 ? 1 : ((i1 == i2) ? 0 : -1);
}

struct vector_type int_vtype = {
    .dup = NULL,
    .free = NULL,
    .cmp = int_cmp,
};

int main() {
    vector *v = vector_create(8, &int_vtype);
    int a = 10;
    int b = 20;
    int arr[5] = {1, 2, 3, 4, 5};
    void *ptr;

    vector_iterator *vi = vector_iterator_get(v);

    vector_push(v, &a);
    vector_push(v, &b);
 
    
    while ((ptr = vector_next(vi))) {
        printf("%d\t", *(int *)ptr);
    }
    printf("\n");
    printf("size=%d, cap=%d\n", vector_size(v), vector_capacity(v));
    printf("front: %d, back: %d\n", *(int *)vector_front(v), *(int*)vector_back(v));

    vector_insert(v, 2, 5, &arr[2]);
    printf("size=%d, cap=%d\n", vector_size(v), vector_capacity(v));
    
    vector_iterator_rewind(vi);
    while ((ptr = vector_next(vi))) {
        printf("%d\t", *(int *)ptr);
    }
    printf("\n");

    vector_insert(v, 2, 5, &arr[4]);
    printf("size=%d, cap=%d\n", vector_size(v), vector_capacity(v));

    vector_iterator_rewind(vi);
    while ((ptr = vector_next(vi))) {
        printf("%d\t", *(int *)ptr);
    }   
    printf("\n");

    vector_erase(v, 1, 2);
    printf("size=%d, cap=%d\n", vector_size(v), vector_capacity(v));

    //vector_iterator *vi = vector_iterator_get(v);
    vector_iterator_rewind(vi);
    while ((ptr = vector_next(vi))) {
        printf("%d\t", *(int *)ptr);
    }
    printf("\n");

    printf("v[%d] = %d\n", *(int *)vector_get_idx(v, 5));

    printf("v[top] = %d\n", *(int *)vector_pop(v));
    printf("size=%d, cap=%d\n", vector_size(v), vector_capacity(v));
    vector_iterator_release(vi);
    vector_release(v);
    return 0;
}

#endif /*VECTOR_TEST*/
