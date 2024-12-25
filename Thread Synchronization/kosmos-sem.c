/*
 * kosmos-sem.c (semaphores)
 *
 * UVic CSC 360, Summer 2023
 *
 * Here is some code from which to start.
 *
 * PLEASE FOLLOW THE INSTRUCTIONS REGARDING WHERE YOU ARE PERMITTED
 * TO ADD OR CHANGE THIS CODE. Read from line 136 onwards for
 * this information.
 */

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logging.h"


/* Random # below threshold indicates H; otherwise C. */
#define ATOM_THRESHOLD 0.55
#define DEFAULT_NUM_ATOMS 40

#define MAX_ATOM_NAME_LEN 10
#define MAX_KOSMOS_SECONDS 5

/* Global / shared variables */
int  cNum = 0, hNum = 0;
long numAtoms;


/* Function prototypes */
void kosmos_init(void);
void *c_ready(void *);
void *h_ready(void *);
void make_radical(int, int, int, char *);
void wait_to_terminate(int);


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
	int *pi = (int *)malloc(sizeof(int));
	assert( pi != NULL);
	*pi = i;
	return pi;
}




int main(int argc, char *argv[])
{
	long seed;
	numAtoms = DEFAULT_NUM_ATOMS;
	pthread_t **atom;
	int i;
	int status;

	if ( argc < 2 ) {
		fprintf(stderr, "usage: %s <seed> [<num atoms>]\n", argv[0]);
		exit(1);
	}

	if ( argc >= 2) {
		seed = atoi(argv[1]);
	}

	if (argc == 3) {
		numAtoms = atoi(argv[2]);
		if (numAtoms < 0) {
			fprintf(stderr, "%ld is not a valid number of atoms\n",
				numAtoms);
			exit(1);
		}
	}

    kosmos_log_init();
	kosmos_init();

	srand(seed);
	atom = (pthread_t **)malloc(numAtoms * sizeof(pthread_t *));
	assert (atom != NULL);
	for (i = 0; i < numAtoms; i++) {
		atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
		if ( (double)rand()/(double)RAND_MAX < ATOM_THRESHOLD ) {
			hNum++;
			status = pthread_create (
					atom[i], NULL, h_ready,
					(void *)dupInt(hNum)
				);
		} else {
			cNum++;
			status = pthread_create (
					atom[i], NULL, c_ready,
					(void *)dupInt(cNum)
				);
		}
		if (status != 0) {
			fprintf(stderr, "Error creating atom thread\n");
			exit(1);
		}
	}

    /* Determining the maximum number of ethynyl radicals is fairly
     * straightforward -- it will be the minimum of the number of
     * hNum and cNum/2.
     */

    int max_radicals = (hNum < cNum/2 ? hNum : (int)(cNum/2));
#ifdef VERBOSE
    printf("Maximum # of radicals expected: %d\n", max_radicals);
#endif

    wait_to_terminate(max_radicals);
}

/*
* Now the tricky bit begins....  All the atoms are allowed
* to go their own way, but how does the Kosmos ethynyl-radical
* problem terminate? There is a non-zero probability that
* some atoms will not become part of a radical; that is,
* many atoms may be blocked on some semaphore of our own
* devising. How do we ensure the program ends when
* (a) all possible radicals have been created and (b) all
* remaining atoms are blocked (i.e., not on the ready queue)?
*/



/*
 * ^^^^^^^
 * DO NOT MODIFY CODE ABOVE THIS POINT.
 *
 *************************************
 *************************************
 *
 * ALL STUDENT WORK MUST APPEAR BELOW.
 * vvvvvvvv
 */


/* 
 * DECLARE / DEFINE NEEDED VARIABLES IMMEDIATELY BELOW.
 */

int radicals;
int num_free_c;
int num_free_h;

int combining_c1;
int combining_c2;
int combining_h;
char combiner[MAX_ATOM_NAME_LEN];

sem_t mutex;
sem_t wait_c;
sem_t wait_h;
sem_t staging_area;

int c_to_update = 1;
/*
 * FUNCTIONS YOU MAY/MUST MODIFY.
 */

//This function just initalises sempahpores for later use
void kosmos_init() { 
    sem_init(&mutex,0 ,1);       //We can enter one thread at a time
    sem_init(&wait_c,0 ,0);
    sem_init(&wait_h,0 ,0);
    sem_init(&staging_area,0 ,1);//The first time a hydrogen is created we dont deadlock
    
    num_free_c = 0;
    num_free_h = 0;
    radicals = 1;
}

//This function regulates which "Hydrogens" get used in the reaction
void *h_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "h%03d", id);

#ifdef VERBOSE
	printf("%s now exists\n", name);
#endif
    
    sem_wait(&mutex);
    num_free_h++;                                //Updating the number of free hydrogens
    sem_post(&mutex);                            //Unlocked so that a carbon can be created if needed
    
    sem_wait(&staging_area);                     //Making sure the previous hydrogen has been used in reaction before updating hydrogen id 
    combining_h = id; 
    sem_post(&wait_h);                           //Signaling a new hydrogen was created so C_ready can continue code
    
    if(num_free_h >=1 && num_free_c >=2){        //Checking to see if we can create the radical and if we can we do so
        num_free_h -= 1;
        num_free_c -=2;
        make_radical(combining_c1, combining_c2, combining_h, name);
        sem_post(&staging_area);                 //Allow hydrogen ID to get updated after reaction
        sem_post(&mutex);                        //Allow next thread to enter code section
    }else if(num_free_h >=1 && num_free_c ==1){  //If in this case we wait for one more carbon 
        sem_post(&mutex);
        sem_wait(&wait_c);
    }else{                                       //Otherweise we need to wait for 2 carbons
        sem_post(&mutex);
        sem_wait(&wait_c);
        sem_wait(&wait_c);
    }
    //The last few conditions signal mutex so other threads can enter their code and not deadlock while we wait for needed atoms 
	return NULL;
}

//This function regulates which "Carbons" get used in the reaction
void *c_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "c%03d", id);

#ifdef VERBOSE
	printf("%s now exists\n", name);
#endif
    
    sem_wait(&mutex);
    num_free_c++;                              //Locking critcal section and updating number of carbons
    if(c_to_update %2 == 1){                   //Alternating between which carbon id I update
        combining_c1 = id;
        c_to_update++;
    }else{
        combining_c2 = id;
        c_to_update++;
    }
    sem_post(&wait_c);                         //Signaling a new carbon has been made so h_ready code can carry on if waiting
    
    if(num_free_h >=1 && num_free_c >=2){      //Checking if we have enough atoms to create a radical
        num_free_h -= 1;
        num_free_c -=2;
        make_radical(combining_c1, combining_c2, combining_h, name);
        sem_post(&staging_area);               //After radical creation allow hydrogen id to update
        sem_post(&mutex);
    }
    else if(num_free_h >=1 && num_free_c ==1){ //Case where we wait for a carbon
        sem_post(&mutex);
        sem_wait(&wait_c);
    }else if(num_free_h <1 && num_free_c >=2){ //Case where we wait for a hydrogenn
        sem_post(&mutex);
        sem_wait(&wait_h);
    }else{                                     //Case where we wait for a hyrdogen and a carbon
        sem_post(&mutex);
        sem_wait(&wait_h); 
        sem_wait(&wait_c);
    }
    //The last few conditions signal mutex so other threads can enter their code and not deadlock while we wait for needed atoms
	return NULL;
}


/* 
 * Note: The function below need not be used, as the code for making radicals
 * could be located within h_ready and c_ready. However, it is perfectly
 * possible that you have a solution which depends on such a function
 * having a purpose as intended by the function's name.
 */
void make_radical(int c1, int c2, int h, char *maker)
{
#ifdef VERBOSE
    fprintf(stdout, "A ethynyl radical was made: c%03d  c%03d  h%03d\n",
        c1, c2, h);
#endif
    kosmos_log_add_entry(radicals, c1, c2, h, maker);
    radicals++;
}


void wait_to_terminate(int expected_num_radicals) {
    /* A rather lazy way of doing it, for now. */
    sleep(MAX_KOSMOS_SECONDS);
    kosmos_log_dump();
    sem_destroy(&mutex);
    sem_destroy(&wait_c);
    sem_destroy(&wait_h);
    sem_destroy(&staging_area);
    exit(0);
}
