#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../mySemHeader/mysem.h"

typedef struct worker_arg{
	int number;
	int avl;
	int next;
	int term_sig;
	int thread;
	int has_job;
}worker_arg_t;

int term=0;

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
	
	while(1) {
		if (term && !arg->has_job) break;
		
		mysem_down(arg->avl);
		if (arg->has_job == 0) break;
		
		if(primetest(arg->number)){
			printf("\033[1;36mThread: %d\033[0m, Number: %d \033[1;33mis prime\033[0m\n", arg->thread, arg->number);
		}
		else{
			printf("\033[1;36mThread: %d\033[0m, Number: %d \033[1;32mis not prime\033[0m\n", arg->thread, arg->number);
		}
		
		arg->has_job--;
		
		mysem_up(arg->next);
	}
	printf("\033[1;36mDone Thread: %d\033[0m\n", arg->thread);
	mysem_up(arg->term_sig);
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
	
	for (i=0; i<atoi(argv[1]); i++) {															//Arxikopoihsh dedomenwn kai dhmiourgia threads
		worker[i].avl = mysem_create(0);
		worker[i].next = mysem_create(1);
		worker[i].term_sig = mysem_create(0);
		worker[i].thread = i;
		worker[i].has_job = 0;
		if(pthread_create(&thread, NULL, (void *) worker_thread,(void *) &worker[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	
	while (argv[j] != NULL) {																	//Elegxos gia to an uparxoun dedomena kai oso yparxoun ta3inomhsh tous ston katallhlo worker
		for (i=0; i<atoi(argv[1]); i++) {
			if(i >= argc-2 || jobs == 0) break;
			
			mysem_down(worker[i].next);
			
			worker[i].number = atoi(argv[j]);
			jobs--;
			j++;
			worker[i].has_job++;
			
			if(worker[i].has_job == 1){
				mysem_up(worker[i].avl);
			}
		}
	}
	
	term=1;
	
	for(i=0; i<atoi(argv[1]); i++) {
		if (worker[i].has_job == 0) {
			mysem_up(worker[i].avl);
		}
		mysem_down(worker[i].term_sig);
	}
	
	for (i=0; i<atoi(argv[1]); i++) {
		mysem_destroy(worker[i].avl);
		mysem_destroy(worker[i].next);
		mysem_destroy(worker[i].term_sig);
	}
	free(worker);																				//Apodesmeysh mnhmhs
	
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time elapsed: %lf\n", cpu_time_used);
	
	return 0;
}
