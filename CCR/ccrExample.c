#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "CCR.h"
#define N 3
#define BRIDGE_TIME 3

int blue_active, blue_waiting, red_active, red_waiting, red_passed, blue_passed;

CCR_DECLARE(bridge);

void blue_car (int *id) {
	
	//ENTER_BRIDGE
	CCR_EXEC(bridge,1,
		
		blue_waiting++;
	);
	
	CCR_EXEC(bridge, (red_active == 0) && !blue_passed,
		
		if(blue_active < N-1){
			blue_active++;
			blue_waiting--;
		}
		else{
			blue_active++;
			blue_waiting--;
			blue_passed = 1;
		}
	);
		
	sleep(BRIDGE_TIME);
	printf("\033[0;34mBlue\033[0m car [%d] is passing...\n", *id);
		
	//EXIT_BRIDGE
	CCR_EXEC(bridge,1,
		
		blue_active--;
		printf("\033[0;34mBlue\033[0m car exited\n");
		//printf("\033[0;34mBlue\033[0m car exited!  W [%d], A [%d] & \033[0;31mRed\033[0m W [%d], A [%d] \n", blue_waiting, blue_active, red_waiting, red_active);
		
		if((blue_active == 0) && (red_waiting > 0)){
			red_passed = 0;
			printf("Give priority to \033[0;31mRed\033[0m.\n");
		}
		else if ((red_waiting == 0) && (blue_waiting > 0)) {
			blue_passed = 0;
			printf("Give priority to \033[0;34mBlue\033[0m.\n");
		}
	);
}

void red_car (int *id) {
	
	//ENTER_BRIDGE
	CCR_EXEC(bridge,1,red_waiting++;);
	
	CCR_EXEC(bridge, (blue_active == 0) && !red_passed,
	if(red_active < N-1){
		red_active++;
		red_waiting--;
	}
	else{
		red_active++;
		red_waiting--;
		red_passed = 1;
	}
	);
	
	sleep(BRIDGE_TIME);
	printf("\033[0;31mRed\033[0m car [%d] is passing...\n", *id);
	
	//EXIT_BRIDGE
	CCR_EXEC(bridge,1,
	red_active--;
	printf("\033[0;31mRed\033[0m car exited\n");
	//printf("\033[0;31mRed\033[0m car exited! W [%d], A [%d] & \033[0;34mBlue\033[0m W [%d], A [%d]\n", red_waiting, red_active, blue_waiting, blue_active);
	
	if ((red_active == 0) && (blue_waiting > 0)) {
		blue_passed = 0;
		printf("Give priority to \033[0;34mBlue\033[0m.\n");
	}
	else if ((blue_waiting == 0) && (red_waiting > 0)){
		red_passed = 0;
		printf("Give priority to \033[0;31mRed\033[0m.\n");
		
	}
	);
}

int main(int argc, char **argv) {
	
	if (argc < 3) {
		printf("***Not enough arguments***\n");
		return 0;
	}
	
	int i, *red_id, *blue_id, blue, red;

	blue = atoi(argv[1]);
	red = atoi(argv[2]);
	pthread_t blue_thread[blue], red_thread[red];
	
	red_id = calloc(red, sizeof(int));
	blue_id = calloc(blue, sizeof(int));
	
	printf("%d blue cars - %d red cars\n\n", blue, red);
	CCR_INIT(bridge);
	
	CCR_EXEC(bridge, 1,
	blue_waiting = 0;
	red_waiting = 0;
	blue_active = 0;
	red_active = 0; 
	red_passed = 0;
	blue_passed = 0;);
	
	//Create blue_car threads
	for (i=0; i<blue; i++) {
		blue_id[i] = i;
		if (pthread_create(&blue_thread[i], NULL, (void *) blue_car, (void *) &blue_id[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	//Create red_car threads
	for (i=0; i<red; i++) {
		red_id[i] = i;
		if (pthread_create(&red_thread[i], NULL, (void *) red_car, (void *) &red_id[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	
	for (i=0; i<blue; i++) {
		pthread_join(blue_thread[i], NULL);
	}
	for (i=0; i<red; i++) {
		pthread_join(red_thread[i], NULL);
	}
	
	free(red_id);
	free(blue_id);
	return 0;
}
