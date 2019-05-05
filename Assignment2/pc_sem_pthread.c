#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 200;
const int NUM_CONSUMERS  = 2;
const int NUM_PRODUCERS  = 2;

int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items
int items = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t producerEmpty;
sem_t consumerEmpty;

void* producer (void* v) {
    int i = 0;
    for (i=0; i<NUM_ITERATIONS; i++) {
        // TODO
        assert (items >= 0 && items <= MAX_ITEMS);
        sem_wait(&consumerEmpty);
        pthread_mutex_lock(&lock);
        items++;
        histogram[items] += 1;
        printf("Producer Produced %d\n", items);
        pthread_mutex_unlock(&lock);
        sem_post(&producerEmpty);
    }
    pthread_exit(NULL);
}

void* consumer (void* v) {
    int i = 0;
    for (i=0; i<NUM_ITERATIONS; i++) {
        // TODO
        assert (items >= 0 && items <= MAX_ITEMS);
        sem_wait(&producerEmpty);
        pthread_mutex_lock(&lock);
        items--;
        histogram[items] += 1;
        printf("Comnsumer Consumed %d\n", items);
        pthread_mutex_unlock(&lock);
        sem_post(&consumerEmpty);
    }
    pthread_exit(NULL);
}

int main (int argc, char** argv) {

    // TODO: Create Threads and Join
    pthread_t p1, p2, c1, c2;
    sem_init(&producerEmpty, 0, 0);
    sem_init(&consumerEmpty, 0, MAX_ITEMS);
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&c1, NULL, consumer, NULL);
    pthread_create(&c2, NULL, consumer, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
    int sum = 0;
    int i = 0;
    for (i = 0; i <= MAX_ITEMS; i++) {
        printf("  items=%d, %d times\n", i, histogram[i]);
        sum += histogram[i];
    }
}
