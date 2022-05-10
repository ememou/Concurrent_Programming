#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_monitor.h"
#define BRIDGE_TIME 3

void blue_car (int *id) {
	
	enter_bridge('b');
	
	sleep(BRIDGE_TIME);
	
	printf("\033[0;34mBlue\033[0m car [%d] is passing...\n", *id);
	
	exit_bridge('b');
}

void red_car (int *id) {
	
	enter_bridge('r');
	
	sleep(BRIDGE_TIME);
	
	printf("\033[0;31mRed\033[0m car [%d] is passing...\n", *id);
	
	exit_bridge('r');
}

int main(int argc, char **argv) {
	
	if (argc < 3) {
		printf("***Not enough arguments***\n");
		return 0;
	}
	
	int i, *red_id, *blue_id, blue, red;
	pthread_t blue_thread, red_thread;
	
	blue = atoi(argv[1]);
	red = atoi(argv[2]);
	
	red_id = calloc(red, sizeof(int));
	blue_id = calloc(blue, sizeof(int));
	
	printf("%d blue cars - %d red cars\n\n", blue, red);
	
	monitor_init();
	
	//Create blue_car threads
	for (i=0; i<blue; i++) {
		blue_id[i] = i;
		if (pthread_create(&blue_thread, NULL, (void *) blue_car, (void *) &blue_id[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	//Create red_car threads
	for (i=0; i<red; i++) {
		red_id[i] = i;
		if (pthread_create(&red_thread, NULL, (void *) red_car, (void *) &red_id[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	
	term_main();
	//sleep(10);
	
	free(red_id);
	free(blue_id);
	return 0;
}

