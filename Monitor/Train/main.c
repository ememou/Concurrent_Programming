#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_monitor.h"
#define N 4
#define TRIP_TIME 3

void passenger_thread (int *id) {
		
	get_in(id);
}

void train_thread () {
	
	while (1) {
		
		wait_train();
		
		printf("\nTOY TOY TOY\n");
		sleep(TRIP_TIME);
		printf("TELOS DIADROMHS / KATEBEITE\n\n");
		
		if(train_is_back()) break;
	}
	signal_to_term_main();
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
	
	if (atoi(argv[1]) < N) {
		printf("The train is not full so it will not start another ride\n");
		return 0;
	}
	
	id = calloc(atoi(argv[1]), sizeof(int));
	
	monitor_init(atoi(argv[1]));
	
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
	
	
	term_main();
		
	free(id);
	
	return 0;
}
