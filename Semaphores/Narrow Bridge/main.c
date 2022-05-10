#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../mySemHeader/mysem.h"
#define N 3
#define BRIDGE_TIME 3

//Globals
volatile int blue_active = 0;
volatile int red_active = 0;
volatile int blue_waiting = 0;
volatile int red_waiting = 0;
//Semaphores
volatile int only_blue;
volatile int only_red;
volatile int bridge;
volatile int finish_main;

void enter_bridge (char color) {
	
	//Blue Car
	if (color == 'b') {
		mysem_down(bridge);
	
		if((red_active == 0) && (blue_active < N)){
			blue_active++;
			printf("%d \033[0;34mBlue\033[0m cars on the bridge\n", blue_active);
			mysem_up(only_blue);
		}
		else {
			blue_waiting++;
			//printf("%d \033[0;34mBlue\033[0m cars waiting\n", blue_waiting);
		}
		
		mysem_up(bridge);
		
		mysem_down(only_blue);
	}
	//Red Car
	else {
		mysem_down(bridge);
		
		if ((blue_active == 0) && (red_active < N)) {
			red_active++;
			printf("%d \033[0;31mRed\033[0m cars on the bridge\n", red_active);
			mysem_up(only_red);
		}
		else {
			red_waiting++;
			//printf("%d \033[0;31mRed\033[0m cars waiting\n", red_waiting);
		}
		
		mysem_up(bridge);
		
		mysem_down(only_red);
	}
}

void exit_bridge (char color) {
	
	int count;
	
	//Blue Car
	if (color == 'b') {
		mysem_down(bridge);
		blue_active--;
		printf("\033[0;34mBlue\033[0m car exited\n");
		
		if ((blue_active == 0) && (red_waiting > 0)){
			count = 0;
			while ((red_waiting > 0) && (count < N)) {
				red_active++;
				red_waiting--;
				mysem_up(only_red);
				count++;
				printf("%d \033[0;31mRed\033[0m cars on the bridge\n", red_active);
			}
		}
		else if ((red_waiting == 0) && (blue_waiting > 0)) {
			blue_active++;
			blue_waiting--;
			printf("%d \033[0;34mBlue\033[0m cars on the bridge\n", blue_active);
			mysem_up(only_blue);
		}
		mysem_up(bridge);
	}
	//Red Car
	else {
		mysem_down(bridge);
		red_active--;
		printf("\033[0;31mRed\033[0m car exited\n");
		
		if ((red_active == 0) && (blue_waiting > 0)) {
			count = 0;
			while ((blue_waiting > 0) && (count < N)) {
				blue_active++;
				blue_waiting--;
				mysem_up(only_blue);
				count++;
				printf("%d \033[0;34mBlue\033[0m cars on the bridge\n", blue_active);
			}
		}
		else if ((blue_waiting == 0) && (red_waiting > 0)) {
			red_active++;
			red_waiting--;
			printf("%d \033[0;31mRed\033[0m cars on the bridge\n", red_active);
			mysem_up(only_red);
		}
		mysem_up(bridge);
	}
	
	if((red_waiting == 0) && (red_active == 0) && (blue_waiting == 0) && (blue_active == 0)){
		mysem_up(finish_main);
	}
}

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
	
	only_blue = mysem_create(0);
	only_red = mysem_create(0);
	bridge = mysem_create(1);
	finish_main = mysem_create(0);
	
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
	
	mysem_down(finish_main);
	
	mysem_destroy(only_blue);
	mysem_destroy(only_red);
	mysem_destroy(bridge);
	mysem_destroy(finish_main);
	
	free(red_id);
	free(blue_id);
	return 0;
}

