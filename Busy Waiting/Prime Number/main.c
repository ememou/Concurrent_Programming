#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct job{
	int number;
	int return_val;
}job_t;

typedef struct worker_arg{
	job_t jobs; 
	int available;
	int pos;
	int term;
}worker_arg_t;

worker_arg_t *worker;
job_t *output;
int stop = 0;

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

void *worker_thread (worker_arg_t *arg) {														//Thread handler
	
	while (1) {
		if (stop) {																				//Shma apo thn main gia na stamathsei to thread
			arg->term = 1;
			break;
		}
		if (!arg->available){																	//Elegxos dia8esimothtas gia ana8esh ergasias
			arg->jobs.return_val = primetest(arg->jobs.number);
			output[arg->pos] = arg->jobs;
			arg->available = 1;
		}
	}
	return NULL;
}

int main(int argc, char **argv) {
	
	int jobs = argc-2;
	int i, j=2;
	pthread_t thread;
	clock_t start, end;
	double cpu_time_used;
	start = clock();
	
	if (argc < 2) {
		printf("*NOT ENOUGH ARGUMENTS!!!*");
		return 0;
	}
	
	worker = calloc(atoi(argv[1]), sizeof(worker_arg_t));										//Desmeush mnhmhs gia ta dedomena pou pernane ston thread handler
	output = calloc(argc-2, sizeof(job_t));														//Desmeush mnhmhs gia ton pinaka output opou kai 8a perastoun ta apotelesmata
	
	for (i=0; i<atoi(argv[1]); i++) {															//Arxikopoihsh dedomenwn kai dhmiourgia threads
		worker[i].available = 1;
		worker[i].term = 0;
		if(pthread_create(&thread, NULL, (void *) worker_thread,(void *) &worker[i])){
			printf("### ERROR ###\n");
			return(0);
		}
	}
	
	while (argv[j] != NULL) {																	//Elegxos gia to an uparxoun dedomena kai oso yparxoun ta3inomhsh tous ston katallhlo worker
		for (i=0; i<atoi(argv[1]); i++) {
			if(i >= argc-2 || jobs == 0) break;
			if (worker[i].available) {
				jobs--;
				worker[i].available = 0;
				worker[i].jobs.number = atoi(argv[j]);
				worker[i].pos = j-2;
				j++;
			}
		}
	}
	
	while (1) {																					//Elegxos gia to an ola ta threads einai dia8esima dld exoun teleiwsei
		int threads_available = 0;
		for (i=0; i<atoi(argv[1]); i++) {
			if (worker[i].available) {
				threads_available++;
			}
		}
		if (threads_available == atoi(argv[1])) break;
	}
	
	stop = 1;																					//Shma termatismou twn threads
	
	while (1) {																					//Elegxos gia to an exoun termatisei ola omala
		int threads_stop = 0;
		for (i=0; i<atoi(argv[1]); i++) {
			if (worker[i].term) {
				threads_stop++;
			}
		}
		if (threads_stop == atoi(argv[1])) break;
	}
		
	for (i=0; i<argc-2; i++) {
		printf("*(%d)*\t", output[i].number);
		if (output[i].return_val) printf("is prime!\n");
		else printf("is not prime!\n");
	}
	
	free(worker);																				//Apodesmeysh mnhmhs
	free(output);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time elapsed: %lf\n", cpu_time_used);
		
	return 0;
}
