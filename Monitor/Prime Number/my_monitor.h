#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct worker_arg{
	int number;
	int thread;
	int available;
}worker_arg_t;

int threads_number, threads_avl, finished, wait, term;
pthread_cond_t none, *start, terminate_main;
pthread_mutex_t monitor;

void monitor_init(int thread_num, int jobs){
	
	int i;
	
	pthread_mutex_init(&monitor, NULL);
	pthread_cond_init(&none, NULL);
	pthread_cond_init(&terminate_main, NULL);
	finished = jobs;
	wait = 0;
	term = 0;
	start = (pthread_cond_t*)calloc(thread_num, sizeof(pthread_cond_t));
	
	for (i=0; i<thread_num; i++) {
		pthread_cond_init(&start[i], NULL);
	}
	
	threads_number = thread_num;
	threads_avl = thread_num;
}

void give_job(worker_arg_t *arg, int number, int jobs){
	
	int i;
	pthread_mutex_lock(&monitor);
	
	if(!threads_avl){
		pthread_cond_wait(&none, &monitor);
	}
	
	for(i=0; i<threads_number; i++){
		if(!jobs) break;
		if(arg[i].available){
			arg[i].number = number;
			threads_avl--;
			arg[i].available = 0;
			//printf("Give to Thread %d job num %d\n", arg[i].thread, arg[i].number);
			pthread_cond_signal(&start[i]);
			break;
		}
	}
	
	pthread_mutex_unlock(&monitor);
}

int get_job(worker_arg_t *arg){
	if(!finished) 
		return 1;
	
	pthread_mutex_lock(&monitor);
	
	if (finished) {
		if (arg->available){
			//printf("Thread %d waits to get job\n", arg->thread);
			pthread_cond_wait(&start[arg->thread], &monitor);
			//printf("Thread %d got job %d\n", arg->thread, arg->number);
		}
	}	
	pthread_mutex_unlock(&monitor);	
	
	if(!finished) 
		return 1;
	
	return 0;
}

void finished_job(worker_arg_t *arg){
	
	int i;
	
	pthread_mutex_lock(&monitor);
	
	arg->available = 1;
	threads_avl++;
	pthread_cond_signal(&none);
	//printf("Thread %d finished job num %d.\n", arg->thread, arg->number);
	
	finished--;
	if(!finished){
		//printf("finish %d\n", finished);
		for(i=0; i<threads_number; i++)
			pthread_cond_signal(&start[i]);
	}
	
	pthread_mutex_unlock(&monitor);
}

void term_main(){
	
	pthread_mutex_lock(&monitor);
	if(!wait){
		//printf("wait\n");
		pthread_cond_wait(&terminate_main, &monitor);
	}
	pthread_mutex_unlock(&monitor);
}


void signal_to_term_main(){
	
	pthread_mutex_lock(&monitor);
	term++;

	if(term == threads_number){
		wait = 1;
		//printf("signal Term %d\n", term);
		pthread_cond_signal(&terminate_main);
	}
	pthread_mutex_unlock(&monitor);
}
