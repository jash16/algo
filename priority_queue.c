#include "priority_queue.h"
#include "share.h"
#include "heap.h"

priority_queue *pq_create(int flag, void *t) {
    priority_queue *pq = (priority_queue *)calloc(1, sizeof(priority_queue));
    if (pq == NULL) {
        return NULL;
    }

    pq->h = heap_create(0, flag, t);
    if (pq->h == NULL) {
        free(pq);
        return NULL;
    }
    return pq;
}

void pq_release(priority_queue *pq) {
    heap_release(pq->h);
    free(pq);
    return;
}

bool pq_insert(priority_queue *pq, void *val) {
    return heap_insert(pq->h, val);
}

void *pq_max(priority_queue *pq) {
    return heap_top(pq->h);
}

void *pq_extract_max(priority_queue *pq) {
    return heap_pop(pq->h);
}

void *pq_increase_key(priority_queue *pq, void *val1, void *val2) {
    
}

void *pq_min(priority_queue *pq) {
    return heap_top(pq->h);
}

void *pq_extract_min(priority_queue *pq) {
    return heap_pop(pq->h);
}

void *pq_decrease_key(priority_queue *pq, void *val1, void *val2) {

}


#ifdef TEST_PQ

#include <stdio.h>
#include <stdlib.h>

//gcc -o priority_queue priority_queue.c heap.c  -g -D TEST_PQ

typedef struct huffman {
    struct huffman *left, *right;
    float freq;
    char value;
} huffman;

char code[100] = {'\0'};

void get_huffmap_code(huffman *h) {
    static int idx = 0;
    if (h->left == NULL && h->right == NULL) {
        printf("%s\t%c\n", code, h->value);
    }
    if (h->left != NULL) {
        code[idx] = '0';
        idx ++;
        get_huffmap_code(h->left);
        code[idx] = '\0';
        idx --;
    }
    if (h->right != NULL) {
        code[idx] = '1';
        idx ++;
        get_huffmap_code(h->right);
        code[idx] = '\0';
        idx --;
    }
}

int huffman_cmp(void *ptr1, void *ptr2) {
    huffman *h1 = (huffman *)ptr1;
    huffman *h2 = (huffman *)ptr2;

    return h1->freq > h2->freq ? 1 : 0;
}

struct heap_type huffman_heap_type = {
    .free = NULL,
    .cmp = huffman_cmp,
    .dup = NULL,
    .match = NULL,
};

int main() {
    struct huffman h[] = {
        { 
            .left = NULL,
            .right = NULL,
            .freq = 0.45,
            .value = 'a',
        },
        {
            .left = NULL,
            .right = NULL,
            .freq = 0.13,
            .value = 'b',
        },
        {
            .left = NULL,
            .right = NULL,
            .freq = 0.12,
            .value = 'c',
        },
        {
            .left = NULL,
            .right = NULL,
            .freq = 0.16,
            .value = 'd',
        },
        {
            .left = NULL,
            .right = NULL,
            .freq = 0.09,
            .value = 'e',
        },
        {
            .left = NULL,
            .right = NULL,
            .freq = 0.05,
            .value = 'f',
        },
    };
    
    int i = 0;
    huffman *p, *q;
    priority_queue *pq = pq_create(MIN_HEAP, &huffman_heap_type);
    if (pq == NULL) {
        printf("pq_create failed\n");
        return;
    }

    for (i = 0; i < sizeof(h)/sizeof(h[0]); i ++) {
        pq_insert(pq, &h[i]);   
    }
    
    while ((p = (huffman *)pq_extract_min(pq)) != NULL) {
        q = (huffman *)pq_extract_min(pq);
        if (q == NULL) {
            break;
        }

        huffman *hp = malloc(sizeof(huffman));
        hp->left = p;
        hp->right = q;
        hp->freq = p->freq + q->freq;
        pq_insert(pq, hp);
    }
    
    if (p != NULL) {
        //printf("%f\n", p->freq);
    }
    get_huffmap_code(p);   
    
    pq_release(pq);
    
}

#endif /*TEST_PQ*/
