#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

int mysem_create(int val) {
	
	int semid;
	
	semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if(semid == -1) {
		perror("SEMGET");
		exit(1);
	}
	
	if(semctl(semid, 0, SETVAL, val) == -1){
		perror("SEMCTL");
		exit(1);
	}
	
	return (semid);
}

void mysem_down(int semid) {
	
	struct sembuf d_sops;

	d_sops.sem_op = -1;
	d_sops.sem_num = 0;
	d_sops.sem_flg = 0;
	
	if(semop(semid, &d_sops, 1) == -1){
		perror("SEMOP DOWN");
		exit(1);
	}
}

int mysem_up(int semid) {
	
	int ret;
	struct sembuf up_sops;
	
	up_sops.sem_op = 1;
	up_sops.sem_num = 0;
	up_sops.sem_flg = 0;
	
	
	ret = semctl(semid, 0, GETVAL, NULL);
	if(ret == -1){
		perror("SEMCTL UP");
		exit(1);
	}
	if(ret == 1){
		printf("***LOST CALL***\n");
		return (1);
	}
	
	if(semop(semid, &up_sops, 1) == -1){
		perror("SEMOP UP");
		exit(1);
	}
	
	return (0);
}

void mysem_destroy(int semid) {
	
	if(semctl(semid, 0, IPC_RMID, NULL) == -1){
		perror("SEMCTL");
		exit(1);
	}
}
