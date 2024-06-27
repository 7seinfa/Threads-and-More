#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>

void *subtract (void *thread_id);
void *prime_check (void *thread_id);
void *reverse_num (void *thread_id);

int port[2];
int lengthOfString;

int main (int argc, char** argv){
	if (argc!=3){
		perror("invalid command line arguments error");
		printf("Please enter exactly 3 arguments\n");
		exit(0);
	}
	int status = pipe(port);

	if (status < 0) {
		perror("pipe error");
		exit(0);
	}

	int x, y;
	x = atoi(argv[1]);
	y = atoi(argv[2]);
	printf("parent (PID %d) reveives X=%d and Y=%d from the user\n", getpid(), x, y);
	printf("parent (PID %d) writes X=%d and Y=%d to the pipe\n", getpid(), x, y);
	char XY[124];
	sprintf(XY,"%d %d",x, y);
	write(port[1], XY, strlen(XY));
	pthread_t thread1;
	if(pthread_create(&thread1, NULL, subtract, 1)){
		printf("Error creating thread\n");
		exit(0);
	}
	pthread_join(thread1, NULL);
	pthread_t thread2;
	if(pthread_create(&thread2, NULL, prime_check, 2)){
		printf("Error creating thread\n");
		exit(0);
	}
	pthread_join(thread2, NULL);
	pthread_t thread3;
	if(pthread_create(&thread3, NULL, reverse_num, 3)){
		printf("Error creating thread\n");
		exit(0);
	}
	pthread_join(thread3, NULL);
}

void *subtract(void *thread_id) {
	char X[124];
	read(port[0], &X, sizeof(X));
	char* XY;
	XY = strtok(X," ");
	int x = atoi(XY);
	XY = strtok(NULL, " ");
	int y = atoi(XY);
	printf("thread(TID %d) reads X=%d and Y=%d from the pipe\n",thread_id, x, y);
	int XminusY = x-y;
	printf("thread(TID %d) writes X-Y=%d to the pipe\n",thread_id, XminusY);
	char XminusYString[124];
	sprintf(XminusYString,"%d",XminusY);
	write(port[1], XminusYString, strlen(XminusYString));
}

void *prime_check(void *thread_id){
	char X[124];
	read(port[0], &X, sizeof(X));
	int x = atoi(X);
	printf("thread(TID %d) reads X-Y=%d from the pipe\n",thread_id,x);
	
	int isPrime = 1;
	for (int i = 2; i < x; i++){
		if (x % i == 0) isPrime = 0;
	}

	if (isPrime == 1) printf("thread(TID %d) identified that %d is a prime number\n",thread_id, x);
	else printf("thread(TID %d) identified that %d is not a prime number\n",thread_id, x);
	printf("thread(TID %d) writes %s to the pipe\n",thread_id,X);
	write(port[1], X, strlen(X));
}

void *reverse_num(void *thread_id){
	char X[124];
	read(port[0], &X, sizeof(X));
	printf("thread(TID %d) reads X-Y=%s from the pipe\n",thread_id, X);
	char revX[124];
	
	for (int i = 0; i < strlen(X)/2; i++){
		revX[i] = X[strlen(X)-i-1];
		revX[strlen(X)-i-1]=X[i];
	}
	printf("thread(TID %d) reversed number is %s\n",thread_id,revX);
}
