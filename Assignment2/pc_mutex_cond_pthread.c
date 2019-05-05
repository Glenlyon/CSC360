#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 200;
const int NUM_CONSUMERS  = 2;
const int NUM_PRODUCERS  = 2;

int producer_wait_count;     // # of times producer had to wait
int consumer_wait_count;     // # of times consumer had to wait
int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items

int items = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producerEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumerEmpty = PTHREAD_COND_INITIALIZER;


void* producer (void* arg) {
    int i = 0;
    for(i = 0; i < NUM_ITERATIONS; i++){
        pthread_mutex_lock(&lock);
        while(items == MAX_ITEMS){
            pthread_cond_wait(&producerEmpty, &lock);
        }
		assert (items < MAX_ITEMS);
        items++;
        histogram[items] += 1;
        printf("Produced: %d\n", items);
        pthread_cond_signal(&consumerEmpty);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(0);
}

void* consumer (void* arg) {
    int i = 0;
    for(i = 0; i < NUM_ITERATIONS; i++){
        pthread_mutex_lock(&lock);
        while(items == 0){
            pthread_cond_wait(&consumerEmpty, &lock);
        }		
		assert (items > 0);
        items--;
        histogram[items] += 1;
        printf("Consumed: %d\n", items); 
        pthread_cond_signal(&producerEmpty);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(0);
}

int main (int argc, char** argv) {
    pthread_t p1, p2, c1, c2;
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&c1, NULL, consumer, NULL);
    pthread_create(&c2, NULL, consumer, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
	printf("Done");
    int sum=0;
    int i = 0;
    for (i = 0; i <= MAX_ITEMS; i++) {
        printf ("  items=%d, %d times\n", i, histogram [i]);
        sum += histogram [i];
    }
}
