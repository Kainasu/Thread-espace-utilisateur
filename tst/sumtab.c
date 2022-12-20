#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "thread.h"

//int * tab;

struct args{
    int * tab;
    int a;
    int b;
};

void init_args(struct args * arg, int a, int b, int * tab){
    arg->a = a;
    arg->b = b;
    arg->tab = tab;
}

void* sumtab(void * arg){
    thread_t th1, th2;
    int m, err;
    int * res;
    
    struct args* args = (struct args *) arg;
    int * tab = args->tab;    
    m = (args->a + args->b)/2;

    if (m != args->a){
        int * res2;
        struct args tab1, tab2;
        init_args(&tab1, args->a, m, tab);
        init_args(&tab2, m+1, args->b, tab);

        err = thread_create(&th1, sumtab, (void*)&tab1);
        assert(!err);
        err = thread_create(&th2, sumtab, (void*)&tab2);
        assert(!err);
        
        err = thread_join(th2, (void*)&res);
        assert(!err);
        err = thread_join(th1, (void*)&res2);
        assert(!err);
        *res = *res + *res2;
        free(res2);
        return (void*) res;
    }
    res = malloc(sizeof(int));
    if (args->a == args->b){
        *res = tab[args->a];
    } else {
        *res = tab[args->a] + tab[args->b];
    }
    return (void*) res;
}

int main(int argc, char * argv[]){
    struct timeval tv1, tv2;
    unsigned long us;
    int n, err, *res;
    thread_t th1, th2;

    if (argc > 1)
        n = atoi(argv[1]);
    else
        n = 8;

    int * tab = malloc(n*sizeof(int));
    for (int i= 0; i < n; i++)
        tab[i] = 2;        
    
    struct args tab1;
    tab1.a = 0;
    tab1.b = n-1;
    tab1.tab = tab;
    
    gettimeofday(&tv1, NULL);
    err = thread_create(&th1, sumtab, (void*)&tab1);
    assert(!err);

    err = thread_join(th1, (void*)&res);
    assert(!err);    
    gettimeofday(&tv2, NULL);

    us = (tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
    printf("Résultat : %d\n", *res);
    printf("Programme exécuté en %lu us\n", us);
    free(res);
    free(tab);
    return 0;
    
}
