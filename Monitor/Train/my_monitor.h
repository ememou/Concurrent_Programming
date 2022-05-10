#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define N 4

int term,passengers_left, passengers_riding, passengers_got_off, wait;
pthread_cond_t finish_route, train, pass, empty_train, terminate_main;
pthread_mutex_t monitor;

void monitor_init(int val) {
	
	term=0;
	passengers_left = val;
	passengers_riding = 0;
	passengers_got_off = 0;
	wait = 0;
	pthread_mutex_init(&monitor, NULL);
	pthread_cond_init(&train, NULL);
	pthread_cond_init(&finish_route, NULL);
	pthread_cond_init(&pass, NULL);
	pthread_cond_init(&terminate_main, NULL);
}

void get_in(int *arg){
	
	pthread_mutex_lock(&monitor);
	
	if(passengers_riding==N){
		pthread_cond_wait(&pass, &monitor);
	}
	
	if (!term) {
		printf("%d thread riding\n", *arg);
		passengers_riding++;
		
		
		if(passengers_riding==N){
			wait = 1;
			pthread_cond_signal(&train);
		}
		
		pthread_cond_wait(&finish_route, &monitor);
		
		printf("KATEBHKA THREAD: %d\n", *arg);
		passengers_left--;
		passengers_got_off++;
		
		if(passengers_got_off == N){
			passengers_got_off = 0;
			pthread_cond_signal(&empty_train);
		}
	}
	
	pthread_mutex_unlock(&monitor);
}


void wait_train(){
	
	pthread_mutex_lock(&monitor);
	
	if(!wait){
		//printf("train\n");
		pthread_cond_wait(&train, &monitor);
	}
	wait = 0;
	pthread_mutex_unlock(&monitor);
}

int train_is_back(){
	
	int i;
	
	pthread_mutex_lock(&monitor);

	for(i=0; i<N; i++){
		pthread_cond_signal(&finish_route);
	}
	
	pthread_cond_wait(&empty_train, &monitor);
	
	passengers_riding = 0;
	if(passengers_left >= N){
		for(i=0; i<N; i++){
			pthread_cond_signal(&pass);
		}
	}
	
	pthread_mutex_unlock(&monitor);
	
	if(passengers_left == 0){
		return 1;
	}
	else if (passengers_left < N) {
		term=1;
		for(i=0; i<passengers_left; i++){
			pthread_cond_signal(&pass);
		}
		printf("\nThe train is not full so it will not start another ride\n");
		return 1;
	}
	else{
		return 0;
	}
}

void term_main(){
	pthread_mutex_lock(&monitor);
	
	//printf("wait\n");
	pthread_cond_wait(&terminate_main, &monitor);
	
	pthread_mutex_unlock(&monitor);
}

void signal_to_term_main(){
	pthread_mutex_lock(&monitor);
	
	//printf("signal\n");
	pthread_cond_signal(&terminate_main);
	
	pthread_mutex_unlock(&monitor);
}
