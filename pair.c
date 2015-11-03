#include "share.h"

#include "pair.h"
//#include <stdlib.h>

pair *make_pair(void *first, void *second, pair_type *pt) {
    pair *p;

    p = (pair *)calloc(sizeof(pair), 1);
    if (p == NULL) {
        return NULL;
    }

    p->first = first;
    p->second = second;
    p->pt = pt;
    return p;
}

void free_pair(pair *p) {
    pair_type *pt;

    pt = p->pt;
    if (pt) {
        if (pt->first_free) {
            pt->first_free(p->first);
        }
        if (pt->second_free) {
            pt->second_free(p->second);
        }
    }

    free(p);
    return;
}

void *first(pair *p) {
    return p->first;
}

void *second(pair *p) {
    return p->second;
}

int pair_cmp(pair *p1, pair *p2) {
    pair_type *pt = p1->pt;
    int ret1 = 0, ret2 = 0;

    if (pt) {
        if (pt->first_cmp) {
            ret1 = pt->first_cmp(first(p1), first(p2));
        } else {
            ret1 = first(p1) > first(p2) ? 1 : (first(p1) == first(p2) ? 0 : -1);
        }

        if (pt->second_cmp) {
            ret2 = pt->second_cmp(second(p1), second(p2));
        } else {
            ret2 = second(p1) > second(p2) ? 1 : (second(p2) == second(p2) ? 0 : -1);
        }
    }

    return ret1 > 0 ? 1 : (ret1 < 0 ? -1 : (ret2 > 0 ? 1 : (ret2 < 0 ? -1 : 0)));
}

#ifdef PAIR_TEST
#include <stdio.h>
#include <string.h>

//compile
//gcc -Wall -g -o pair_test pair.c -D PAIR_TEST
int int_cmp(void *p1, void *p2) {
    int *i1 = (int *)p1;
    int *i2 = (int *)p2;
    return *i1 > *i2 ? 1 : ((*i1 == *i2) ? 0 : -1);
}

int string_cmp(void *p1, void *p2) {
    char *s1 = (char *)p1;
    char *s2 = (char *)p2;

     return strcmp(s1, s2);   
}
struct pair_type int_pair = {
    .first_cmp = int_cmp,
    .second_cmp = int_cmp,
    .first_free = NULL,
    .second_free = NULL,
};

struct pair_type int_string_pair = {
    .first_cmp = int_cmp,
    .second_cmp = string_cmp,
    .first_free = NULL,
    .second_free = NULL,
};

int main() {
    int a = 1;
    int b = 2;
    pair *p1 = make_pair((void *)&a, (void *)&b, &int_pair);
    printf("p1: first = %d, second = %d\n", *(int *)first(p1), *(int *)second(p1));
    
    int c = 1;
    int d = 3;

    pair *p2 = make_pair((void *)&c, (void *)&d, &int_pair);
    pair *p3 = make_pair((void *)&b, (void *)&d, &int_pair);
    int ret = pair_cmp(p1, p2);
    printf("p2: first = %d, second = %d\n", *(int *)first(p2), *(int *)second(p2));
    printf("p1 %c p2\n", ret > 0 ? '>' : (ret = 0 ? '=': '<'));
    ret = pair_cmp(p2, p3);
    printf("p2 %c p3\n", ret > 0 ? '>' : (ret = 0 ? '=': '<'));
    free_pair(p1);
    free_pair(p2);
    free_pair(p3);

    char hello[] = {"hello"};
    char world[] = {"hello1"};
    pair *isp1 = make_pair((void *)&a, (void *)hello, &int_string_pair);
    pair *isp2 = make_pair((void *)&a, (void *)world, &int_string_pair);
    pair *isp3 = make_pair((void *)&b, (void *)hello, &int_string_pair);

    printf("p1: first = %d, second = %s\n", *(int *)first(isp1), (char *)second(isp1));
    ret = pair_cmp(isp1, isp2);
    printf("isp1 %c isp2\n", ret > 0 ? '>' : (ret = 0 ? '=': '<'));
    ret = pair_cmp(isp1, isp3);
    printf("isp1 %c isp3\n", ret > 0 ? '>' : (ret = 0 ? '=': '<'));
    free_pair(isp1);
    free_pair(isp2);
    free_pair(isp3);
    return 0;
}

#endif
