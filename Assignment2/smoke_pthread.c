/**This assignment has been adopted directorly from the previous smoke_uthread
There has been discussions with signal not sending up correctly during debugging with Weiyi Zhang and Ye Yuan
Thanks to Weiyi Zhang's idea of using struct instead of typedef to get it finally running properly
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_ITERATIONS 1000

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

/**
 * You might find these declarations helpful.
 *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
 *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
 */
enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

int signal_count [5];  // # of times resource signalled
int smoke_count  [5];  // # of times smoker with resource smoked

//Changed from typedef to struct to solve the signal not receving by agent problem
struct agent_t{
    pthread_mutex_t mutex;
    pthread_cond_t  match;
    pthread_cond_t  paper;
    pthread_cond_t  tobacco;
    pthread_cond_t  smoke;
};

/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can re-write it if you like, but be sure that all it does
 * is choose 2 random reasources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */
void* agent (void* arg) {
    struct agent_t *a = (struct agent_t*)arg;
    static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
    static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};                                                

    pthread_mutex_lock (&a->mutex);
    int i = 0;
    for ( i = 0; i < NUM_ITERATIONS; i++) {
        int r = random() % 3;
        signal_count [matching_smoker [r]] ++;
        int c = choices [r];
        if (c & MATCH) {
            VERBOSE_PRINT ("match available\n");
            pthread_cond_signal (&a->match);
        }
        if (c & PAPER) {
            VERBOSE_PRINT ("paper available\n");
            pthread_cond_signal (&a->paper);
        }
        if (c & TOBACCO) {
            VERBOSE_PRINT ("tobacco available\n");
            pthread_cond_signal (&a->tobacco);
        }
		VERBOSE_PRINT("agent is waiting for smoker to smoke\n");
		pthread_cond_wait(&a->smoke, &a->mutex);
		//printf("A somker finishes smoking\n");
    }
    //printf("Factory Shut Down\n");
    //The agent initially is not terminated properly, various ways such as timedwait and if conditions has been added to force the thread to close
    pthread_mutex_unlock (&a->mutex);
    pthread_exit(0);
}

//
// TODO
pthread_cond_t tobaccoSmoker;
pthread_cond_t paperSmoker;
pthread_cond_t matchSmoker;

int indicator = 0;

/**All the printf is used to check which part signal start to go wrong
The unlock used to be inside the while loop, and was moved above and below the loop
to ensure it's running correctly
*/

void whichSmoker(int number){
    if(number == 3){
        pthread_cond_signal(&tobaccoSmoker);
        //printf("Tobacco smoker ready\n");
        indicator = 0;
    }else if(number == 5){
        pthread_cond_signal(&paperSmoker);
        //printf("Paper smoker ready\n");
        indicator = 0;
    }else if(number == 6){
        pthread_cond_signal(&matchSmoker);
        //printf("Match smoker ready\n");
        indicator = 0;
    }
}

void* tobacco(void* arg){
    struct agent_t *a = (struct agent_t*)arg;
    int i;
	pthread_mutex_lock (&a->mutex);
	while(1){
        pthread_cond_wait(&a->tobacco, &a->mutex);
        indicator += TOBACCO;
        whichSmoker(indicator);
    }	  
	pthread_mutex_unlock(&a->mutex);
    pthread_exit(0);
}

void* paper(void* arg){
    struct agent_t *a = (struct agent_t*)arg;
    int i;
	pthread_mutex_lock (&a->mutex);
	while(1){
        pthread_cond_wait(&a->paper, &a->mutex);
        indicator += PAPER;
        whichSmoker(indicator);
    }		
	pthread_mutex_unlock(&a->mutex);
    pthread_exit(0);
}

void* match(void* arg){
    struct agent_t *a = (struct agent_t*)arg;
    int i;
	pthread_mutex_lock (&a->mutex);
	while(1){
        pthread_cond_wait(&a->match, &a->mutex);
        indicator += MATCH;
        whichSmoker(indicator);
    }
	pthread_mutex_unlock (&a->mutex);
    pthread_exit(0);
}

void* smokerWithTobacco(void* arg){
    struct agent_t *a = (struct agent_t*)arg;
    int i;
	pthread_mutex_lock (&a->mutex);
	while(1){
        pthread_cond_wait(&tobaccoSmoker, &a->mutex);
        //printf("Tobacco smoker start smoking\n");
        smoke_count[TOBACCO]++;
	pthread_cond_broadcast(&a->smoke);
	//printf("Tobacco smoker %d\n", smoke_count[TOBACCO]);
		
    }
    pthread_mutex_unlock(&a->mutex);
    pthread_exit(0);
}

void* smokerWithPaper(void* arg){
    struct agent_t *a = (struct agent_t*)arg;
    int i;
	pthread_mutex_lock (&a->mutex);
	while(1){
        pthread_cond_wait(&paperSmoker, &a->mutex);
        smoke_count[PAPER]++;
	pthread_cond_broadcast(&a->smoke);
	//printf("Paper smoker %d\n", smoke_count[PAPER]);
    }
pthread_mutex_unlock(&a->mutex);
    pthread_exit(0);
}

void* smokerWithMatch(void* arg){
    struct agent_t *a = (struct agent_t*)arg;
    int i;
	pthread_mutex_lock (&a->mutex);
	while(1){
        pthread_cond_wait(&matchSmoker, &a->mutex);
        smoke_count[MATCH]++; 
	pthread_cond_broadcast(&a->smoke);
	//printf("Match smoker %d\n", smoke_count[MATCH]);
    }
    pthread_mutex_unlock(&a->mutex);
    pthread_exit(0);
}
// You will probably need to add some procedures and struct agent_t etc.
//

int main (int argc, char** argv) {
    pthread_t ag, t, p, m, sT, sM, sP;
    struct agent_t a;
    pthread_mutex_init(&a.mutex, NULL);
    pthread_cond_init (&a.match,NULL);
    pthread_cond_init (&a.paper,NULL);
    pthread_cond_init (&a.tobacco,NULL);
    pthread_cond_init (&a.smoke,NULL);
    
    // TODO
    pthread_cond_t tobaccoSmoker = PTHREAD_COND_INITIALIZER;
    pthread_cond_t matchSmoker = PTHREAD_COND_INITIALIZER;
    pthread_cond_t paperSmoker = PTHREAD_COND_INITIALIZER;


    pthread_create(&t, NULL, tobacco, (void*)&a);
    pthread_create(&m, NULL, match, (void*)&a);
    pthread_create(&p, NULL, paper, (void*)&a);
    pthread_create(&sT, NULL, smokerWithTobacco, (void*)&a);
    pthread_create(&sM, NULL, smokerWithMatch, (void*)&a);
    pthread_create(&sP, NULL, smokerWithPaper, (void*)&a);
    pthread_create (&ag, NULL, agent, (void*)&a);
    
    //Wait for agent finish, then procced to finish, no need to wait other thread to finish
    pthread_join(ag,NULL);

    assert (signal_count [MATCH]   == smoke_count [MATCH]);
    assert (signal_count [PAPER]   == smoke_count [PAPER]);
    assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
    assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);
    printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
            smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);
	exit(0);
}
