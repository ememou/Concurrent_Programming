#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SIZE 64

char *circ_buffer;
volatile int head;
volatile int tail;
int size_of_input;

void pipe_init(int size){												//Desmeush mnhmhs gia ton buffer kai arxikopoihsh head kai tail
	
	circ_buffer = calloc(size, sizeof(char));
	
	head = 0;
	tail = 0;
}

void pipe_close() {														//Apeleu8erwsh mnhmhs
	
	free(circ_buffer);
}


int pipe_read (char *c) {												
		
	while (1) {
		if (tail == size_of_input-1) {									//Elegxos mexri na diabastei olo to arxeio
			break;
		}
		if (tail < head ) {												//Diabasma dedomenwn otan mono exoun graftei
			tail++;
			return 1;
		}
		
	}
	pipe_close();														//Otan teleiwsoun ta dedomena kane free ton buffer
	return 0;
}

void pipe_write(char c) {
	
	while(1){
		if(head >= tail && head%SIZE<=tail%SIZE){						//Elegxos oti exoun graftei perissotera dedomena apo auta pou exoun grafei kai oti den 8a gra4oume panw se kati to opoio den exei diabastei
			circ_buffer[head%SIZE] = c;									//gra4imo xarakthra ston buffer
			head++;
			break;
		}
	}
}
