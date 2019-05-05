/** This assignemnt's algrothim is inspired by Yuanjie Zhao's smoke problem solution
https://github.com/YuanjieZhao/Cigarette-Smokers-Problem/blob/master/smoke.c
In the process of debugging, I have discussed with Ye Yuan and Weiyi Zhang
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

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

struct Agent {
    uthread_mutex_t mutex;
    uthread_cond_t  match;
    uthread_cond_t  paper;
    uthread_cond_t  tobacco;
    uthread_cond_t  smoke;
};

struct Agent* createAgent() {
    struct Agent* agent = malloc (sizeof (struct Agent));
    agent->mutex   = uthread_mutex_create();
    agent->paper   = uthread_cond_create (agent->mutex);
    agent->match   = uthread_cond_create (agent->mutex);
    agent->tobacco = uthread_cond_create (agent->mutex);
    agent->smoke   = uthread_cond_create (agent->mutex);
    return agent;
}

//
// TODO
uthread_cond_t tobaccoSmoker;
uthread_cond_t paperSmoker;
uthread_cond_t matchSmoker;
int indicator = 0;

void whichSmoker(int number){
    //the sum of resource enum will indicate which smoker can smoke
    if(number == 3){
        uthread_cond_signal(tobaccoSmoker);
        //printf("Tobacco smoker ready\n");
        indicator = 0;
    }else if(number == 5){
        uthread_cond_signal(paperSmoker);
        //printf("Paper smoker ready\n");
        indicator = 0;
    }else if(number == 6){
        uthread_cond_signal(matchSmoker);
        //printf("Match smoker ready\n");
        indicator = 0;
    }
    //indicator = 0;
    /**the indicator cannot set to zero here, as it will reset indicator everytime resources
    is produced
    */
}

//the thread for tobacco, paper and match will check if there is enough materail for smoke
void* tobacco(void* arg){
    struct Agent* temp = arg;
    uthread_mutex_lock(temp->mutex);
    while(1){
        uthread_cond_wait(temp->tobacco);
        indicator += TOBACCO;
        whichSmoker(indicator);
    }
    uthread_mutex_unlock(temp->mutex);
}

void* paper(void* arg){
    struct Agent* temp = arg;
    uthread_mutex_lock(temp->mutex);
    while(1){
        uthread_cond_wait(temp->paper);
        indicator += PAPER;
        whichSmoker(indicator);
    }
    uthread_mutex_unlock(temp->mutex);
}

void* match(void* arg){
    struct Agent* temp = arg;
    uthread_mutex_lock(temp->mutex);
    while(1){
        uthread_cond_wait(temp->match);
        indicator += MATCH;
        whichSmoker(indicator);
    }
    uthread_mutex_unlock(temp->mutex);
}

//Once smoker received signal, they start smoking, signal the agent to continue produce
void* smokerWithTobacco(void* arg){
    struct Agent* temp = arg;
    uthread_mutex_lock(temp->mutex);
    while(1){
        uthread_cond_wait(tobaccoSmoker);
        uthread_cond_signal(temp->smoke);
        //printf("Tobacco smoker start smoking\n");
        smoke_count[TOBACCO]++;
    }
    uthread_mutex_unlock(temp->mutex);
}

void* smokerWithPaper(void* arg){
    struct Agent* temp = arg;
    uthread_mutex_lock(temp->mutex);
    while(1){
        uthread_cond_wait(paperSmoker);
        uthread_cond_signal(temp->smoke);
        //printf("Paper smoker start smoking\n");
        smoke_count[PAPER]++;
    }
    uthread_mutex_unlock(temp->mutex);
}

void* smokerWithMatch(void* arg){
    struct Agent* temp = arg;
    uthread_mutex_lock(temp->mutex);
    while(1){
        uthread_cond_wait(matchSmoker);
        uthread_cond_signal(temp->smoke);
        //printf("Match smoker start smoking\n");
        smoke_count[MATCH]++;
    }
    uthread_mutex_unlock(temp->mutex);
}

// You will probably need to add some procedures and struct etc.
//
/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can re-write it if you like, but be sure that all it does
 * is choose 2 random reasources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */
void* agent (void* av) {
    struct Agent* a = av;
    static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
    static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};

    uthread_mutex_lock (a->mutex);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int r = random() % 3;
        signal_count [matching_smoker [r]] ++;
        int c = choices [r];
        if (c & MATCH) {
            VERBOSE_PRINT ("match available\n");
            uthread_cond_signal (a->match);
        }
        if (c & PAPER) {
            VERBOSE_PRINT ("paper available\n");
            uthread_cond_signal (a->paper);
        }
        if (c & TOBACCO) {
            VERBOSE_PRINT ("tobacco available\n");
            uthread_cond_signal (a->tobacco);
        }
        VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
        uthread_cond_wait (a->smoke);
    }
    uthread_mutex_unlock (a->mutex);
    return NULL;
}

int main (int argc, char** argv) {
    uthread_init (7);
    struct Agent*  a = createAgent();
    // TODO
    tobaccoSmoker = uthread_cond_create(a->mutex);
    matchSmoker = uthread_cond_create(a->mutex);
    paperSmoker = uthread_cond_create(a->mutex);

    uthread_create(tobacco, a);
    uthread_create(match, a);
    uthread_create(paper, a);
    uthread_create(smokerWithTobacco, a);
    uthread_create(smokerWithMatch, a);
    uthread_create(smokerWithPaper, a);

    uthread_join (uthread_create (agent, a), 0);
    assert (signal_count [MATCH]   == smoke_count [MATCH]);
    assert (signal_count [PAPER]   == smoke_count [PAPER]);
    assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
    assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);
    printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
            smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);
}
