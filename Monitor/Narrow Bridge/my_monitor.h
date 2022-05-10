#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define N 3


int blue_active, red_active, blue_waiting, red_waiting;
pthread_cond_t only_blue, only_red, terminate_main;
pthread_mutex_t monitor;

void monitor_init() {
	
	blue_active = 0;
	red_active = 0;
	blue_waiting = 0;
	red_waiting = 0;
	
	pthread_mutex_init(&monitor, NULL);
	pthread_cond_init(&only_blue, NULL);
	pthread_cond_init(&only_red, NULL);
	pthread_cond_init(&terminate_main, NULL);
}

void enter_bridge (char color) {
	
	pthread_mutex_lock(&monitor);
	//Blue Car
	if (color == 'b') {
		if((red_active == 0) && (blue_active < N)){
			blue_active++;
			printf("%d \033[0;34mBlue\033[0m cars on the bridge\n", blue_active);
		}
		else {
			blue_waiting++;
			//printf("%d \033[0;34mBlue\033[0m cars waiting\n", blue_waiting);
			
			pthread_cond_wait(&only_blue,&monitor);
		}
	}
	//Red Car
	else {
		if ((blue_active == 0) && (red_active < N)) {
			red_active++;
			printf("%d \033[0;31mRed\033[0m cars on the bridge\n", red_active);
		}
		else {
			red_waiting++;
			//printf("%d \033[0;31mRed\033[0m cars waiting\n", red_waiting);
			
			pthread_cond_wait(&only_red,&monitor);
		}
	}
	pthread_mutex_unlock(&monitor);
}

void exit_bridge (char color) {
	
	int count;
	
	pthread_mutex_lock(&monitor);
	
	//Blue Car
	if (color == 'b') {
		blue_active--;
		printf("\033[0;34mBlue\033[0m car exited\n");
		
		if ((blue_active == 0) && (red_waiting > 0)){
			count = 0;
			while ((red_waiting > 0) && (count < N)) {
				red_active++;
				red_waiting--;
				
				pthread_cond_signal(&only_red);
				
				count++;
				printf("%d \033[0;31mRed\033[0m cars on the bridge\n", red_active);
			}
		}
		else if ((red_waiting == 0) && (blue_waiting > 0)) {
			blue_active++;
			blue_waiting--;
			printf("%d \033[0;34mBlue\033[0m cars on the bridge\n", blue_active);
			
			pthread_cond_signal(&only_blue);
		}
	}
	
	//Red Car
	else {
		red_active--;
		printf("\033[0;31mRed\033[0m car exited\n");
		
		if ((red_active == 0) && (blue_waiting > 0)) {
			count = 0;
			while ((blue_waiting > 0) && (count < N)) {
				blue_active++;
				blue_waiting--;
				
				pthread_cond_signal(&only_blue);
				
				count++;
				printf("%d \033[0;34mBlue\033[0m cars on the bridge\n", blue_active);
			}
		}
		else if ((blue_waiting == 0) && (red_waiting > 0)) {
			red_active++;
			red_waiting--;
			printf("%d \033[0;31mRed\033[0m cars on the bridge\n", red_active);
			
			pthread_cond_signal(&only_red);
		}
	}
	
	if((red_waiting == 0) && (red_active == 0) && (blue_waiting == 0) && (blue_active == 0)){
		
		pthread_cond_signal(&terminate_main);
	}
	
	pthread_mutex_unlock(&monitor);
}

void term_main(){
	pthread_mutex_lock(&monitor);
	
	pthread_cond_wait(&terminate_main, &monitor);
	
	pthread_mutex_unlock(&monitor);
}
