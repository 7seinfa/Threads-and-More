#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv){ 
    if (argc!=4){ //make sure exactly 3 arguments
        perror("invalid command line argumets error");
	printf("Please enter exactly 3 arguments\n");
	exit(0);
    }
    pid_t pid;
    char X[100];
    int port[2];
    int status = pipe(port);
    
    if (status < 0){ //pipe failed
        perror("pipe error");
        exit(0);
    } 
    
    pid = fork();
    if (pid < 0){ //fork failed
        perror("fork error");
        exit(0);
    }

    if (pid > 0 ){ //parent
	printf("parent (PID %d): created child (PID %d)\n", getpid(), pid);
        
	//wait for something to write and then concatenate Y to it and write
	read(port[0], &X, sizeof(X));
	printf("parent (PID %d): read from pipe \"%s\"\n", getpid(), X);
	printf("parent (PID %d): received Y = \"%s\"\n", getpid(), argv[2]);
	printf("parent (PID %d): \"%s\" + Y = ", getpid(), X);
	strcat(strcat(X," "), argv[2]);
	printf("\"%s\"\n",X);
	printf("parent (PID %d): writing into pipe \"%s\"\n",getpid(),X);
	write(port[1],X,strlen(X));
	
	wait(NULL); //wait for child
	
	//read final string
	read(port[0], &X, sizeof(X));
	printf("parent (PID %d): read from pipe \"%s\"\n",getpid(), X);
	printf("parent (PID %d): all tasks completed\n", getpid());
    }

    if (pid == 0) { //child
	//use first argument as X and write to pipe
        printf("child (PID %d): received X = \"%s\"\n", getpid(), argv[1]);
	printf("child (PID %d): writing \"%s\" into pipe\n", getpid(), argv[1]);
	write(port[1], argv[1], strlen(argv[1]));
	
	sleep(0); //allow parent to read before child tries
        
	//read from parent and concatenate Z to it
	read(port[0], &X, sizeof(X));
	printf("child (PID %d): read from pipe \"%s\"\n", getpid(), X);
	printf("child (PID %d): received Z = \"%s\"\n", getpid(), argv[3]);
	printf("child (PID %d): \"%s\" + Z = ", getpid(), X);
	printf("\"%s\"\n", strcat(strcat(X," "),argv[3]));
	write(port[1], X, strlen(X));
	printf("child (PID %d): all tasks completed\n", getpid());
    }
}
