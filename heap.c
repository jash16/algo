#include "share.h"
#include "heap.h"

static void _min_heapfy_up(heap *h, int i);
static void _max_heapfy_up(heap *h, int i);
static void _min_heapfy_down(heap *h, int i);
static void _max_heapfy_down(heap *h, int i);

static void _min_heapsort(heap *h);
static void _max_heapsort(heap *h);

heap *heap_create(int size, int flag, heap_type *ht) {
    int hsize;

    heap *h = (heap *)calloc(sizeof(heap), 1);
    if (h == NULL) {
        return NULL;
    }
    hsize = size < HEAP_MIN_SIZE ? HEAP_MIN_SIZE : size;

    h->ele = (void **)calloc(sizeof(void *), hsize);
    if (h->ele == NULL) {
        free(h);
        return NULL;
    }

    h->flag = flag;
    h->size = hsize;
    h->used = 0;
    //这里该检查ht以及ht->cmp不能为NULL， 否则堆没法进行排序
    h->ht = ht;
    return h;
}

void heap_release(heap *h) {
    int idx = 0;
    heap_type *ht = h->ht;

    if (ht && ht->free) {
        while (idx < h->used) {
            ht->free(h->ele[idx]);
            idx ++;
        }
    }
    free(h->ele);
    free(h);

    return;
}

bool heap_insert(heap *h, void *value) {
    int idx = 0;
    int size = 0;
    void **ptr;

    if (h->used == h->size) {
        size = h->size * 2;
        ptr = realloc(h->ele, size * sizeof(void *));    
        if (ptr == NULL) {
            return false;
        }
        h->ele = ptr;
        h->size = size;
    }
    h->ele[h->used] = value;
    h->used ++;

    switch(h->flag) {
    case MIN_HEAP:
        _min_heapfy_up(h, h->used - 1);
        break;
    case MAX_HEAP:
        _max_heapfy_up(h, h->used - 1);
        break;
    default:
        return false;
    }
    return true;
}

static void _min_heapfy_up(heap *h, int idx) {
    int parent;
    heap_type *ht = h->ht;
    void *tmp;

    while (idx > 0) {
        parent = (idx - 1) / 2;
        if (ht && ht->cmp) {
            if (ht->cmp(h->ele[idx], h->ele[parent]) > 0) { // a > b
                break;
            } else {
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[parent];
                h->ele[parent] = tmp;

                idx = parent;
            }
        } else {
            if (h->ele[idx] > h->ele[parent]) {
                break;
            } else {
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[parent];
                h->ele[parent] = tmp;
                idx = parent;
            }
        }
    }
    return;
}

static void _min_heapfy_down(heap *h, int idx) {
    int lchild, min;
    void *tmp;
    int used = h->used;
    heap_type *ht = h->ht;

    while (idx < used) {
        lchild = idx*2 + 1;
        if (lchild >= used) {
            break;
        }
        min = lchild;
        if (lchild + 1 < used) {
            if (ht && ht->cmp) {
                if (ht->cmp(h->ele[lchild], h->ele[lchild + 1]) > 0) {
                    min = lchild + 1;
                }    
            } else {
                if (h->ele[lchild] > h->ele[lchild + 1]) {
                    min = lchild + 1;
                }
            }
        }
        
        if (ht && ht->cmp) {
            if (ht->cmp(h->ele[idx], h->ele[min]) > 0) { // a > b
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[min];
                h->ele[min] = tmp;

                idx = min;
            } else {
                break;
            }
        } else {
            if (h->ele[idx] > h->ele[min]) {
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[min];
                h->ele[min] = tmp;

                idx = min;
            } else {
                break;
            }
        }
    }
    return;
}

static void _max_heapfy_up(heap *h, int idx) {
    int parent;
    heap_type *ht = h->ht;
    void *tmp;

    while (idx > 0) {
        parent = (idx - 1) / 2;
        if (ht && ht->cmp) {
            if (ht->cmp(h->ele[idx], h->ele[parent]) > 0) { // a > b
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[parent];
                h->ele[parent] = tmp;
                idx = parent;
            } else {
                break;
            }
        } else {
            if (h->ele[idx] > h->ele[parent]) {
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[parent];
                h->ele[parent] = tmp;
                idx = parent;
            } else {
                break;
            }
        }
    }
    return;
}

void _max_heapfy_down(heap *h, int idx) {

    int lchild, max;
    void *tmp;
    int used = h->used;
    heap_type *ht = h->ht;

    while (idx < used) {
        lchild = idx * 2 + 1;
        if (lchild >= used) {
            break;
        }
        max = lchild;

        if (lchild + 1 < used) {
            if (ht && ht->cmp) {
                if (ht->cmp(h->ele[lchild], h->ele[lchild + 1]) < 0) {
                    max = lchild + 1;
                }    
            } else {
                if (h->ele[lchild] < h->ele[lchild + 1]) {
                    max = lchild + 1;
                }
            }
        }
        
        if (ht && ht->cmp) {
            if (ht->cmp(h->ele[idx], h->ele[max]) > 0) { // a > b
                break;
            } else {
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[max];
                h->ele[max] = tmp;
                idx = max;
            }
        } else {
            if (h->ele[idx] < h->ele[max]) {
                tmp = h->ele[idx];
                h->ele[idx] = h->ele[max];
                h->ele[max] = tmp;

                idx = max;
            } else {
                break;
            }
        }
    }
    return;
}

void *heap_pop(heap *h) {
    void *value;

    if (h->used == 0) {
        return NULL;
    }

    value = h->ele[0];
    h->ele[0] = h->ele[h->used - 1];
    h->used --;
    
    switch(h->flag) {
    case MIN_HEAP:
        _min_heapfy_down(h, 0);
        break;
    case MAX_HEAP:
        _max_heapfy_down(h, 0);
        break;
    }
    return value;
}

void *heap_top(heap *h) {
    void *val;
    
    if (h->used == 0) {
        return NULL;
    }
    return h->ele[0];
}

void *heap_find_and_increase(heap *h) {

}

void *heap_find_and_decrease(heap *h) {

}

//sort the heap
void heap_sort(heap *h) {
    int i = 0;
    void *ptr;
    int idx = h->used - 1;
    int used = h->used;
    heap_type *ht = h->ht;
    
    while (idx > 0) {
        ptr = h->ele[idx];
        h->ele[idx] = h->ele[0];
        h->ele[0] = ptr;
        
        idx --;
        h->used --;
        switch(h->flag) {
        case MIN_HEAP:
            _min_heapfy_down(h, 0);
            break;
        case MAX_HEAP:
            _max_heapfy_down(h, 0);
            break;
        }
    }

    h->used = used;
    return;
} 

//将已有元素的数组进行堆化操作
void heap_make(heap *h) {
    int idx = 0;
    int used = h->used;

    for (idx = used / 2; idx >= 0; idx --) {
        switch(h->flag) {
        case MIN_HEAP:
            _min_heapfy_down(h, idx);
            break;
        case MAX_HEAP:
            _max_heapfy_down(h, idx);
            break;
        }
    }

    return;
}
#ifdef HEAP_TEST
#include <stdio.h>
#include <assert.h>

int int_cmp(void *ptr, void *ptr2) {
    int i1 = *(int *)ptr;
    int i2 = *(int *)ptr2;

    return i1 > i2 ? 1 : (i1 == i2 ? 0 : -1);
}

int string_cmp(void *ptr, void *ptr2) {
    char *str1 = (char *)ptr;
    char *str2 = (char *)ptr2;
    return strcmp(str1, str2);
}

struct heap_type str_heap_type = {
    .free = NULL,
    .cmp = string_cmp,
    .dup = NULL,
    .match = NULL,
};

struct heap_type int_heap_type = {
    .free = NULL,
    .cmp  = int_cmp,
    .dup = NULL,
    .match = NULL,
};

int main() {
    heap *h = heap_create(8, MIN_HEAP, &int_heap_type);
    int arr[] = {5, 2, 7, 6, 4, 3, 8, 1, 10, 9};
    int i = 0;
    int n = sizeof(arr)/sizeof(arr[0]);

    for (i = 0; i < n; i++) {
        heap_insert(h, &arr[i]);
    }
 /*
    for (i = 0; i < n; i++) {
        void *ptr = heap_pop(h);
        printf("%d\n", *(int *)ptr);
    }
*/
 
    heap_sort(h); //after heap_sort, h->ele is a sorted array
    for (i = 0; i < n; i++) {
        void *ptr = h->ele[i];
//        void *ptr = heap_pop(h);
        printf("%d\t", *(int *)ptr);
    }
    printf("\n");

    heap_make(h);

    for (i = 0; i < n; i++) {
        void *ptr = heap_pop(h);
        printf("%d\t", *(int *)ptr);
    }
    printf("\n");
    
    heap *h2 = heap_create(8, MAX_HEAP, &str_heap_type);
    assert(h2 != NULL);
    
    char *strs[] = {"hello", "horld", "i", "fuck", "this", "world"};

    n = sizeof(strs) / sizeof(strs[0]);
    for (i = 0; i < n; i++) {
        heap_insert(h2, strs[i]);
    }
/*
    for (i = 0; i < n; i++) {
        void *ptr = heap_pop(h2);
        printf("%s\n", (char *)ptr);
    }
*/

    heap_sort(h2);
    for (i = 0; i < n; i++) {
        void *ptr = h2->ele[i];
        printf("%s\t", (char *)ptr);
    }
    printf("\n");
    heap_release(h);
    heap_release(h2);

}

#endif /*HEAP_TEST*/
