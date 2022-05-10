#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "my_monitor.h"

int primetest (int v) {
	
	int i, flag = 0;
	
	for (i = 2; i <= v/2; ++i) {
		if(v%i == 0){																			//Den einai prwtos
			flag = 1;
			break;
		}
	}
		
	if (flag == 0 && v!=1) {																	//Einai prwtos
		return 1;
	}
	
	return 0;
}

void worker_thread (worker_arg_t *arg) {														//Thread handler
	
	while(1){
		if(get_job(arg)) break;
		
		if(primetest(arg->number)){
			printf("\033[1;36mThread: %d\033[0m, Number: %d \033[1;33mis prime\033[0m\n", arg->thread, arg->number);
		}
		else{
			printf("\033[1;36mThread: %d\033[0m, Number: %d \033[1;32mis not prime\033[0m\n", arg->thread, arg->number);
		}
		
		finished_job(arg);
	}
	printf("\033[1;36mDone Thread: %d\033[0m\n", arg->thread);
	signal_to_term_main();
}

int main(int argc, char **argv) {
	
	worker_arg_t *worker;
	int jobs = argc-2;
	int i, j=2;
	pthread_t thread;
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	
	if (argc < 2) {
		printf("***NOT ENOUGH ARGUMENTS!!!***\n");
		return 0;
	}
	
	worker = calloc(atoi(argv[1]), sizeof(worker_arg_t));										//Desmeush mnhmhs gia ta dedomena pou pernane ston thread handler
	monitor_init(atoi(argv[1]), jobs);
	
	for (i=0; i<atoi(argv[1]); i++) {															//Arxikopoihsh dedomenwn kai dhmiourgia threads
		worker[i].thread = i;
		worker[i].available = 1;
		if(pthread_create(&thread, NULL, (void *) worker_thread,(void *) &worker[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	
	while (argv[j] != NULL) {																	//Elegxos gia to an uparxoun dedomena kai oso yparxoun ta3inomhsh tous ston katallhlo worker
		give_job(worker, atoi(argv[j]), jobs);			
		jobs--;
		j++;
	}
	
	term_main();
	
	free(worker);	
																				//Apodesmeysh mnhmhs
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time elapsed: %lf\n", cpu_time_used);
	
	return 0;
}
