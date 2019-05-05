#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "uthread.h"
#include "uthread_sem.h"

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 200;
const int NUM_CONSUMERS  = 2;
const int NUM_PRODUCERS  = 2;

int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items

int items = 0;

struct temp{
    int items;
    uthread_sem_t lock;
    uthread_sem_t producerEmpty;
    uthread_sem_t consumerEmpty;
};

struct temp* temp_init() {
    struct temp *temp = malloc(sizeof(struct temp));
    temp->items = 0;
    temp->lock = uthread_sem_create(1);
    temp->producerEmpty = uthread_sem_create(0);
    temp->consumerEmpty = uthread_sem_create(MAX_ITEMS);
    return temp;
}

void* producer (void* v) {
    struct temp* temp = v;
    for (int i=0; i<NUM_ITERATIONS; i++) {
        // TODO
        assert (items >= 0 && items <= MAX_ITEMS);
        struct temp* temp = v;
        uthread_sem_wait(temp->consumerEmpty);
        uthread_sem_wait(temp->lock);
        temp->items++;
        histogram[temp->items] += 1;
        uthread_sem_signal(temp->producerEmpty);
        uthread_sem_signal(temp->lock);
    }
    return NULL;
}

void* consumer (void* v) {
    struct temp* temp = v;
    for (int i=0; i<NUM_ITERATIONS; i++) {
        // TODO
        assert (items >= 0 && items <= MAX_ITEMS);
        struct temp* temp = v;
        uthread_sem_wait(temp->producerEmpty);
        uthread_sem_wait(temp->lock);
        temp->items--;
        histogram[temp->items] += 1;
        uthread_sem_signal(temp->consumerEmpty);
        uthread_sem_signal(temp->lock);
    }
    return NULL;
}

int main (int argc, char** argv) {
    uthread_t t[4];

    uthread_init (4);

    // TODO: Create Threads and Join
    struct temp* temp = temp_init();
    for(int i = 0; i < NUM_CONSUMERS; i++){
        t[i] = uthread_create(&consumer, temp);
    }
    for(int j = 2; j < NUM_CONSUMERS + NUM_PRODUCERS; j++) {
        t[j] = uthread_create(&producer, temp);
    }
    for(int k = 0; k <NUM_CONSUMERS + NUM_PRODUCERS; k++){
        uthread_join(t[k], 0);
    }
    printf ("items value histogram:\n");
    int sum=0;
    for (int i = 0; i <= MAX_ITEMS; i++) {
        printf ("  items=%d, %d times\n", i, histogram [i]);
        sum += histogram [i];
    }
    assert (sum == sizeof (t) / sizeof (uthread_t) * NUM_ITERATIONS);
}