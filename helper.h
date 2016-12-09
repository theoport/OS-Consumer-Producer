/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
#include <cstddef>

using namespace std;

# define SEM_KEY 0x07599// Change this number as needed


struct Job{
	int id; 			//Job ID
	int duration; //Job duration
};

struct Val{					
	int queu_size;		//Size of queu
	int number_jobs;	//Number of jobs each producer can make
	int tot_prod;			//Number of producers
	int tot_cons;			//Number of consumers
	int sem_id;				//ID of semaphore array
};

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_attach (key_t);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);
int sem_timed_wait(int id, short unsigned int num, const int time);
