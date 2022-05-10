#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct position{
	int first;
	int last;
	int ready;
}pos_t;

int *input;

void quicksort (pos_t *pos){
	
	int i, j, pivot, temp;
	pthread_t threadaki1, threadaki2;
	pos_t *pos1, *pos2;
	
	if (pos->last - pos->first < 1 || pos->first >= pos->last) {					//Se periptwsh pou den yparxei ergasia teleiwnei h quickshort-thread 
		pos->ready = 1;
		return;
	}
	
	if (pos->first < pos->last) {													//Etoimos kwdikas quickshort
		pivot = pos->first;
		i = pos->first;
		j = pos->last;
		
		while(i < j){
			while ((input[i] <= input[pivot]) && i<pos->last) {
				i++;
			}
			while (input[j] > input[pivot]) {
				j--;
			}
			if (i < j){
				temp = input[i];
				input[i] = input[j];
				input[j] = temp;
			}
		}
		
		temp = input[pivot];
		input[pivot] = input[j];
		input[j] = temp;															//Telos etoimou kwdika
		
		pos1 = (pos_t*)malloc(sizeof(pos_t));										//Desmeush mnhmhs gia ta orismata tou thread1 kai thread2 kai arxikopoihsh metablhtwn 
		pos1->first = pos->first;
		pos1->last = j-1;
		pos1->ready = 0;
		
		pos2 = (pos_t*)malloc(sizeof(pos_t));
		pos2->first = j+1;
		pos2->ready = 0;
		pos2->last = pos->last;
		
		if (pthread_create(&threadaki1, NULL, (void *)quicksort, (void*)pos1)) {	//Anadromikh klhsh ths quickshort dhmiourgwntas 2 kainouria threads
			printf("ERROR!!!\n");
			return;
		}
		
		if (pthread_create(&threadaki2, NULL, (void *)quicksort, (void*)pos2)) {
			printf("ERROR!!!\n");
			return;
		}
		
		while (1) {																	//Anamonh mexri ta threads tou prohgoumenou kladou ths anadromhs na exoun teleiwsei tis ergasies tous
			if (pos1->ready && pos2->ready) break;
		}
		free(pos1);																	//Apeleu8erwsh ths mnhmhs
		free(pos2);
		pos->ready = 1;																//Telos ths ekastote anadromhs kai shma gia na eidopoihsoume thn main oti exoun teleiwsei ola ta epimerous quicksort-threads
	}
}

int main(int argc, char **argv) {
	
	int i;
	pthread_t threadaki;
	pos_t *pos;
	
	if (argc<2) { 
		printf("***Not enough arguments!***\n");
		return 0;
	}
	
	pos = (pos_t*)malloc(sizeof(pos_t));											//Desmeush mnhmhs gia ta orismata
	input = (int*)calloc(argc-1,sizeof(int));										//Desmeush mnhmhs gia ton pinaka
	
	for (i=0; i<argc-1; i++) {														//Arxikopoihsh pinaka
		input[i] = atoi(argv[i+1]);
	}
	
	pos->first = 0;																	//Arxikopoihsh orismatwn
	pos->last = argc-2;
	pos->ready = 0;
	
	if (pthread_create(&threadaki, NULL, (void *)quicksort, (void *) pos)) {
		printf("ERROR!!!\n");
		return 0;
	}
	
	while (1) {																		//Anamonh mexri na teleiwsoun ola ta threads
		if (pos->ready) break;
	}
	
	free(pos);																		//Apeleu9erwsh ths mnhmhs
																					
	printf("[");																	//Ektypwsh tou ta3inomimenou pinaka
	for (i=0; i<argc-1; i++) {
		if (i<argc-2) printf("[%d] ", input[i]);
		else printf("[%d]", input[i]);
	}
	printf("]\n");
	
	return 0;
}
