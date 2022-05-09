#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

clock_t begin_time;

typedef struct {
    int* vector;
    size_t used;
    size_t size;
} Vector;

void sig_callback(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("received signal %d to stop, aborting...\n", sig);
        clock_t clk = clock();
        double time = (double)(clk - begin_time) / CLOCKS_PER_SEC;
        printf("total cpu time: %f sec\n", time);
        exit(sig);
    }
}

void init_vector(Vector* vec, size_t init_size) {
    vec->vector = malloc(init_size * sizeof(int));
    vec->used = 0;
    vec->size = init_size;
}

void free_vector(Vector* vec) {
    free(vec->vector);
    vec->vector = NULL;
    vec->used = 0;
    vec->size = 0;
}

Vector ins_vector(Vector* vec, int element) {
    if (vec->used == vec->size) {
        Vector newvec;
        init_vector(&newvec, vec->size * 2);
        newvec.used = vec->used;
        newvec.vector = memcpy(newvec.vector, vec->vector, vec->size * sizeof(int));
        newvec.vector[newvec.used++] = element;
        free_vector(vec);
        return newvec;
    }
    vec->vector[vec->used++] = element;
    return *vec;
}

Vector collatz(const int *val) {
    Vector collatz_seq;
    init_vector(&collatz_seq, 4);
    if (val) {
        int coll = *val;
        ins_vector(&collatz_seq, coll);
        if (coll > 0) {
            while (coll != 1) {
                if (coll % 2 == 0) {
                    collatz_seq = ins_vector(&collatz_seq, coll /= 2);
                    continue;
                } else {
                    int i = 3 * coll + 1;
                    coll = i;
                    collatz_seq = ins_vector(&collatz_seq, i);
                    continue;
                }
            }
        } else {
            printf("integer must be larger than 0\n");
        }
    }

    return collatz_seq;
}

int main(int argc, char** argv) {
    begin_time = clock();
    int* collat = malloc(sizeof collat);
    if (argc == 1) {
        int mode;
        printf("single run or continuous execution?\n0: single\n1: continuous\n\n");
        scanf("%d", &mode);
        if (mode) {
            int i = 4;
            signal(SIGINT, (__sighandler_t) sig_callback);
            signal(SIGTERM, (__sighandler_t) sig_callback);
            while (1) {
                printf("\n\ncollatz sequence for %d\n", i);
                Vector coll = collatz(&i);
                for (int j = 0; j < coll.used; j++) {
                    printf("%d\n", *(coll.vector + j));
                    if (j == coll.used - 1) {
                        printf("iterations: %d\n", j + 1);
                        free_vector(&coll);
                    }
                }
                i++;
            }
        }
        printf("choose an integer: ");
        scanf("%d", collat);
    } else if (argc > 1) {
        *collat = (int)strtol(*(argv + 1), NULL, 10);
    }
    if (collat) {
        printf("\n\ncollatz sequence for %d\n", *collat);
        clock_t begin = clock();
        Vector coll = collatz(collat);
        clock_t end = clock();
        double time = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("cpu time for collatz calc: %f sec\n", time);
        for (int i = 0; i < coll.used; i++) {
            printf("%d\n", *(coll.vector + i));
            if (i == coll.used - 1) {
                printf("iterations: %d\n\n", i + 1);
                free_vector(&coll);
            }
        }
    }
    free(collat);
    return 0;
}