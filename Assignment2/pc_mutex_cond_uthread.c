#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 200;
const int NUM_CONSUMERS  = 2;
const int NUM_PRODUCERS  = 2;

int producer_wait_count;     // # of times producer had to wait
int consumer_wait_count;     // # of times consumer had to wait
int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items

int items = 0;
struct temp{
    int items;
    uthread_mutex_t lock;
    uthread_cond_t producerEmpty;
    uthread_cond_t consumerEmpty;
};

struct temp* temp_init(){
    struct temp* temp = malloc(sizeof(struct temp));
    temp->items = 0;
    temp->lock = uthread_mutex_create();
    temp->consumerEmpty = uthread_cond_create(temp->lock);
    temp->producerEmpty = uthread_cond_create(temp->lock);
    return temp;
}

void* producer (void* arg) {
    struct temp* temp = arg;
    for(int i = 0; i < NUM_ITERATIONS; i++){
        assert (temp->items >= 0 && temp->items <= MAX_ITEMS);
        uthread_mutex_lock(temp->lock);
        while(temp->items == MAX_ITEMS){
            producer_wait_count++;
            uthread_cond_wait(temp->producerEmpty);
        }
        temp->items++;
        histogram[temp->items] += 1;
        //printf("Produced: %d\n", temp->items);
        uthread_cond_signal(temp->consumerEmpty);
        uthread_mutex_unlock(temp->lock);
    }
    return NULL;
}

void* consumer (void* arg) {
    struct temp* temp = arg;
    for(int i = 0; i < NUM_ITERATIONS; i++){
        assert (temp->items >= 0 && temp->items <= MAX_ITEMS);
        uthread_mutex_lock(temp->lock);
        while(temp->items == 0){
            consumer_wait_count++;
            uthread_cond_wait(temp->consumerEmpty);
        }
        temp->items--;
        histogram[temp->items] += 1;
        //printf("Consumed: %d\n", temp->items);
        uthread_cond_signal(temp->producerEmpty);
        uthread_mutex_unlock(temp->lock);
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
    printf ("producer_wait_count=%d\nconsumer_wait_count=%d\n", producer_wait_count, consumer_wait_count);
    printf ("items value histogram:\n");
    int sum=0;
    for (int i = 0; i <= MAX_ITEMS; i++) {
        printf ("  items=%d, %d times\n", i, histogram [i]);
        sum += histogram [i];
    }
    assert (sum == sizeof (t) / sizeof (uthread_t) * NUM_ITERATIONS);
}
