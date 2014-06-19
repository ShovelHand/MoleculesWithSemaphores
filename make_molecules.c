/*
 * CSC 360, Summer 2014
 * 
 * Assignment #1
 *
 * Interstellar-space problem: skeleton.
 */

/*this file is the work of Alexander Carmichael, V00174746*/
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>


/* Random # below threshold indicates H; otherwise C. */
#define ATOM_THRESHOLD 0.55
#define DEFAULT_NUM_ATOMS 50
#define TRUE   1
#define FALSE  0


/* Global / shared variables */
int  cNum = 0, hNum = 0;
int  cCount=0, hCount=0;  //number of c and h threads waiting on queue
int  molNum=0;  
int count = 0; // number of atoms that have gone live.  Want to see 50
long numAtoms;
/*semaphores delcared here*/
sem_t mutex;	/*restrict access to critical section*/
sem_t h_wait;	/*wait on hydrogen*/	
sem_t c_wait;	/*wait on carbon*/
//sem_t print_lock;/*dont let a bunch of things print to console a once*/
sem_t over_lock; //don't let program terminate before all atoms live

/* Whatever needs doing for your global data structures, do them here. */
/*struct for initializing semaphores*/
void init()
{
	/*initialize all semaphores*/
	sem_init(&mutex, 0, 1);
	sem_init(&h_wait, 0, 0);
	sem_init(&c_wait, 0, 0);
	//sem_init(&print_lock, 0, 1);
	sem_init(&over_lock, 0, 0);
}


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
	int *pi = (int *)malloc(sizeof(int));
	assert( pi != NULL);
	*pi = i;
	return pi;
}


void makeRadical(int atom, int type)
{
    /* Change this message into something meaningful in the sense
     * that either a C or an H will arrive to find enough atoms
     * present for a new radical, and that newly arrived atom will
     * triggers the actions that resume blocked atoms.
     */
	//do some sem_waits to sync threads? (no)
	//sem_wait(&mutex);
	if(cCount >= 2 && hCount >=1){
		molNum+=1;
	//	sem_wait(&print_lock);
		if(type == 0){
			
			fprintf(stdout, "ethynyl radical number %d was made by actions of h%d\n",molNum, atom);
			}
		if(type == 1){ fprintf(stdout, "ethynyl radical number %d was made by actions of c%d\n",molNum, atom);
					
		}
	cCount -=2;
	hCount -=1;
	//sem_post(&print_lock);
	
	}
	//sem_post(&mutex);
}




void *hReady( void *arg )
{
	int id = *((int *)arg);
	sem_wait(&mutex);  
	sem_post(&h_wait);
	//sem_wait(&print_lock);
	printf("h%d is alive\n", id);
	//sem_post(&print_lock);
	hCount+=1;
    /* Other things happen past this point... */
	if(cCount >=2){
		sem_post(&c_wait);//let two c's and an h go ahead
		sem_post(&c_wait);//once all conditions are met
		sem_post(&h_wait);
	}
	count+=1;   
	//sem_post(&mutex);	
	
	sem_wait(&h_wait);
	makeRadical(id, 0);
	if(count == numAtoms) sem_post(&over_lock); //have all atoms gone live?
	sem_post(&mutex);
	return;
}


void *cReady( void *arg )
{
	int id = *((int *)arg);
	sem_wait(&mutex);
	sem_post(&c_wait);
	//sem_wait(&print_lock);
	printf("c%d is alive\n", id);
	//sem_post(&print_lock);
    /* Other things happen past this point... */
	cCount+=1;
	if(cCount >= 2&& hCount >=1){ //enough threads ready?
		sem_post(&c_wait);
		sem_post(&c_wait);  
		sem_post(&h_wait);
	}
	count+=1;
	//sem_post(&mutex);	//other threads can go	
	sem_wait(&c_wait);	//but hold up 
	makeRadical(id, 1);
	if(count == numAtoms) sem_post(&over_lock); //was that the last one?
	sem_post(&mutex);
	return;
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

	init();
	srand(seed);
	atom = (pthread_t **)malloc(numAtoms * sizeof(pthread_t *));
	assert (atom != NULL);
	for (i = 0; i < numAtoms; i++) {
		atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
		if ( (double)rand()/(double)RAND_MAX < ATOM_THRESHOLD ) {
			hNum++;
			status = pthread_create (
					atom[i], NULL, hReady,
					(void *)dupInt(hNum)
				);
		} else {
			cNum++;
			status = pthread_create (
					atom[i], NULL, cReady,
					(void *)dupInt(cNum)
				);
		}
		if (status != 0) {
			fprintf(stderr, "Error creating atom thread\n");
			exit(1);
		}
	}
	
	sem_wait(&over_lock);  //gotta see 50 atoms go live before done.
	if(cCount >= 2 && hCount >= 1){
		fprintf(stdout, "There is enough unused material to make at least
one more molecule.  Starvation has occurred");
	}
	//release any atoms on wait queues
	while(cCount >0){
		sem_post($c_wait);
		cCount-=1;
	}
	while(hCount >0){
		sem_post(&h_wait);
		hCount -=1;
	}
	
	fprintf(stdout, "\n%d radicals created from:\n",molNum);
	fprintf(stdout, "hydrogens: %d,  carbons: %d\n",hNum,cNum);
	//fprintf(stdout, "%d molecules got\n",molNum);
	//sem_post(&print_lock);
	
	
}
