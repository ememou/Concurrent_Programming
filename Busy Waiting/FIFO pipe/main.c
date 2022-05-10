#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pipe.h"

#define SIZE 64

int end = 0;

void t_read(){
	
	int fd;
	
	fd = open("output", O_CREAT | O_WRONLY, S_IRWXU);					//Dhmiourgia arxeiou e3odou kai elegxos gia swsto anoigma
	if(fd==-1){
		printf("###OPEN ERROR###\n");
		exit(1);
	}
	while(1){															//Anamonh mexri to telos tis anagnwshs tou arxeiou
		while (1) {														//An to fifo einai adeio perimene (otan einai sto idio shmeio to head kai to tail)
			if (head%SIZE != tail%SIZE) break;
		}
		if (write(fd, &circ_buffer[tail%SIZE], sizeof(char)) == -1) {	//Gra4imo byte sto arxeio e3odou
			printf("###WRITE ERROR###\n");
		}
		if(!pipe_read(&circ_buffer[tail%SIZE])){						//Elegxos gia to pote exei teleiwsei olh h anagnwsh apo ton buffer
			break;
		}
	}
	end=1;																//Shma sth main oti exoun teleiwsei ola ta threads
}

void t_write (char *name) {
	
	char c;
	int fd;
	
	fd = open(name, O_RDONLY, S_IREAD);									//Anoigma arxeiou eisodou
	
	if(fd==-1){															//Elegxos gia swsto anoigma
		printf("###OPEN ERROR###\n");
	}
	
	size_of_input = lseek(fd, 0, SEEK_END);
	lseek(fd,0,SEEK_SET);
	
	while (head<size_of_input) {										//Grafoume ston buffer mexri na graftoun ola ta dedomena
		while (1) {														//Blockarisma otan h fifo einai gemath
			if ((head+1)%SIZE != tail%SIZE) break;
		}
		if (read(fd, &c, sizeof(char)) == -1) {							//Diabasma xarakthra apo to arxeio eisodou ston buffer
			printf("###READ ERROR###\n");
			break;
		}
		pipe_write(c);													//Gra4imo tou xarakthra ston buffer
	}
}

int main(int argc, char **argv){
	
	pthread_t t1, t2;
	
	pipe_init(SIZE);
	
	if (pthread_create(&t1, NULL, (void *)t_write, (void *)argv[1])) {
		printf("###ERROR CREATING THREAD 1###\n");
		return(0);
	}
	
	if (pthread_create(&t2, NULL, (void *)t_read, NULL)) {
		printf("###ERROR CREATING THREAD 2###\n");
		return(0);
	}
	
	while (1) {															//Anamonh mexri na teleiwsoun ta threads
		if (end) {
			break;
		}
	}
		
	return (0);
}
