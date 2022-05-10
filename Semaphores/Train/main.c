#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../mySemHeader/mysem.h"
#define N 4
#define TRIP_TIME 3

//GLOBALS
volatile int passengers_riding = 0;
volatile int passengers_got_off = 0;
volatile int passengers_left;

//SEMAPHORES
volatile int start_train;
volatile int pass_semid;
volatile int empty_train;
volatile int finish_route;
volatile int term_train;

void passenger_thread (int *id) {
		
	if (passengers_left >= N) {
		mysem_down(pass_semid);
	
		printf("%d thread riding\n", *id);
		passengers_riding++;
		passengers_left--;
		
		if (passengers_riding == N) {
			mysem_up(start_train);
		}
		else{
			mysem_up(pass_semid);
		}
		
		mysem_down(finish_route);
		
		printf("KATEBHKA THREAD: %d\n", *id);
		passengers_got_off++;
		
		if(passengers_got_off == N){
			passengers_got_off = 0;
			mysem_up(empty_train);
		}
	}
}

void train_thread () {
	
	int i;
	
	while (1) {
		
		mysem_down(start_train);
		
		printf("\nTOY TOY TOY\n");
		sleep(TRIP_TIME);
		printf("TELOS DIADROMHS / KATEBEITE\n\n");
		
		for(i=0; i<N; i++){
			mysem_up(finish_route);
		}
		
		mysem_down(empty_train);
		
		if(passengers_left == 0){
			break;
		}
		else if (passengers_left < N) {
			printf("\nThe train is not full so it will not start another ride\n");
			break;
		}
		
		passengers_riding = 0;
		mysem_up(pass_semid);
		
	}
	mysem_up(term_train);
}

int main(int argc, char **argv) {
	
	int i;
	pthread_t train;
	pthread_t pass;
	int *id;
	
	if (argc<2) {
		printf("***Not enough arguments!***\n");
		return 0;
	}
	
	passengers_left = atoi(argv[1]);
	
	if (passengers_left < N) {
		printf("The train is not full so it will not start another ride\n");
		return 0;
	}
	
	id = calloc(atoi(argv[1]), sizeof(int));
	
	start_train = mysem_create(0);
	pass_semid = mysem_create(1);
	empty_train = mysem_create(0);
	finish_route = mysem_create(0);
	term_train = mysem_create(0);
	
	if(pthread_create(&train, NULL, (void *) train_thread, NULL)){
		printf("### ERROR ###\n");
		return(0);
	}
	
	for (i=0; i<atoi(argv[1]); i++) {
		id[i]=i;
		if(pthread_create(&pass, NULL, (void *) passenger_thread, (void *) &id[i])){
			printf("### ERROR ###\n");
			return (0);
		}
	}
	
	mysem_down(term_train);
	
	mysem_destroy(pass_semid);
	mysem_destroy(finish_route);
	mysem_destroy(empty_train);
	mysem_destroy(start_train);
	mysem_destroy(term_train);
	
	free(id);
	
	return 0;
}
