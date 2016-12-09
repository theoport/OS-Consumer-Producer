/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"


int job_index_C;
int job_index_P;
Job** queu;
Val values;
void *producer (void *id);
void *consumer (void *id);

int main (int argc, char **argv)
{
	job_index_C=-1;
	job_index_P=-1;
	//initialise values in Val structure and check for erros:
	if (argc!=5){
		cout<<"Incorrect number of command line arguments.";
		return -1;
	}
	values.queu_size=check_arg(argv[1]);
	if (values.queu_size<0){
		cerr<<"Invalid queu size."<<endl;
		return -1;
	}
	values.number_jobs=check_arg(argv[2]);
	if (values.number_jobs<0){
		cerr<<"Invalid number of jobs per producer."<<endl;
		return -1;
	}
	values.tot_prod=check_arg(argv[3]);
	if (values.tot_prod<0){
		cerr<<"Invalid number of producers."<<endl;
		return -1;
	}
	values.tot_cons=check_arg(argv[4]);
	if (values.tot_cons<0){
		cerr<<"Invalid number of consumers."<<endl;
		return -1;
	}
	//create semaphore set
  values.sem_id=sem_create(SEM_KEY,3);
	if (values.sem_id<0){
		cerr<<"Error creating set of semaphores."<<endl;
		return -1;
	}
	//initialise mutex sempahore
	if (sem_init(values.sem_id,0,1)<0){
		cerr<<"Error creating semaphore 0."<<endl;
		return -1;
	}
	//initialise semaphore to ensure buffer is not full
	if (sem_init(values.sem_id,1,values.queu_size)<0){
		cerr<<"Error creating semaphore 1."<<endl;
		return -1;
	}
	//initialise semaphore to ensure buffer is not empty
	if (sem_init(values.sem_id,2,0)<0){
		cerr<<"Error creating semaphore 2. "<<endl;
		return -1;
	}
	//create array of threads for consumers and producers
	pthread_t producerid[values.tot_prod];
	pthread_t consumerid[values.tot_cons];
	//create array of Job pointers and assign queu pointer to it, assign all pointers to NULL
	queu=new Job*[values.queu_size];
 	for (int i=0;i<values.queu_size;i++)
		queu[i]=NULL;	
	//create array containing numbers 1-9
	int param[10];
	for (int i=0;i<9;i++)
		param[i]=i+1;
	//initialise threads
	for (int i=0;i<values.tot_prod;i++)
		pthread_create(&producerid[i], NULL, producer,(void *) &param[i]);
	for (int m=0;m<values.tot_cons;m++)
		pthread_create(&consumerid[m], NULL, consumer,(void *) &param[m]);
	//wait for all threads to finish
	for (int i=0;i<values.tot_prod;i++)
		pthread_join(producerid[i], NULL);
	for (int m=0;m<values.tot_cons;m++)
		pthread_join(consumerid[m], NULL);
	//delete semaphores
	sem_close(values.sem_id);
	//delete queu array
	for (int i=0;i<values.queu_size;i++)
		delete queu[i];
 	delete[] queu;
	return 0;
}

void *producer(void *parameter) 
{
	int *param = (int *) parameter;
	for (int jobs=0;jobs<values.number_jobs;jobs++){
		// Wait if job queu is full, end thread if wait exceeds 20 seconds
		if (sem_timed_wait(values.sem_id,1,20)<0){
			cerr<<"Producer ("<<*param<<"): 've waited for over 20 seconds!"<<endl;
			pthread_exit(0);
		}
		//sleep for 1-5 seconds, which is equivalent to doing a job every 1-5 seconds.
		sleep(rand() % 5 +1);
		//wait for critical area to be free
		sem_wait(values.sem_id,0);
		//increment global producer queu pointer until an empty position is found. Do so circularly, i.e. end connects to the start.
		do{ 
			job_index_P=(job_index_P+1)%(values.number_jobs-1);
		}while (queu[job_index_P]!=NULL);
		//create new job
		queu[job_index_P]=new Job;
		queu[job_index_P]->id=job_index_P+1;
		queu[job_index_P]->duration=rand() % 10 + 1;
		cout<<"Producer ("<<*param<<"): ";
		cout<<"Job id "<<queu[job_index_P]->id<<" duration "<<queu[job_index_P]->duration<<endl;
		//exit critical area
		sem_signal(values.sem_id,0);
		sem_signal(values.sem_id,2);
	}
	cout<<"Producer ("<<*param<<"): No more jobs to generate."<<endl;
  pthread_exit(0);
}

void *consumer (void *id) 
{
	int *param = (int *) id;
	int job_index_temp;
	int sleep_duration;
	while(true){
		//wait if job queu is empty, end thread if wait exceeds 20 seconds	
		if (sem_timed_wait(values.sem_id,2,20)<0){
			cerr<<"Consumer ("<<*param<<"): No more jobs left."<<endl;
			pthread_exit(0);
		}
		//wait for critical area to be free
		sem_wait(values.sem_id,0);
		//increment global consumer queu pointer until job is found, do so circularly, i.e. end connects to the start of the buffer
		do{ 
			job_index_C=(job_index_C+1)%(values.number_jobs-1);
		}while (queu[job_index_C]==NULL);
		cout<<"Consumer ("<<*param<<"): Job id "<<queu[job_index_C]->id;
		cout<<" executing sleep duration "<<queu[job_index_C]->duration<<endl;
		sleep_duration=queu[job_index_C]->duration;
		//consume job
		delete queu[job_index_C];
		queu[job_index_C]=NULL;	
		job_index_temp=job_index_C;
		//exit critical area
		sem_signal(values.sem_id,0);
		sem_signal(values.sem_id,1);
		//sleep for sleep duration of consumed job
		sleep(sleep_duration);
		cout<<"Consumer ("<<*param<<"): Job id "<<(job_index_temp+1)<<" completed."<<endl;
	}
}
