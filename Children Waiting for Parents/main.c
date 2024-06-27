#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv){ 
	pid_t pid; 
	printf("parent (PID %d): process started\n", getpid());
	printf("parent (PID %d): forking child_1\n", getpid());
	pid = fork();
	if (pid > 0){
		printf("parent (PID %d): fork successful for child_1 (PID %d)\n", getpid(), pid);
		printf("parent (PID %d): waiting for child_1 (PID %d) to complete\n", getpid(), pid);
	wait(NULL); 
	printf("parent (PID %d): forking child_2\n", getpid());
	pid = fork();
	if (pid > 0){
		printf("parent (PID %d): fork successful for child_2 (PID %d)\n", getpid(), pid);
		printf("parent (PID %d): waiting for child_2 (PID %d) to complete\n", getpid(), pid);
		wait(NULL);
		printf("parent (PID %d): completed\n", getpid());
	}else{
		printf("child_2 (PID %d): calling an external program [%s]\n", getpid(), argv[1]);
		char pidchar[100];
		sprintf(pidchar,"%d",getpid()); 
		execl(argv[1],strcat(pidchar," for child_2"),NULL);
	}
	}else{
		printf("child_1 (PID %d): forking child_1.1\n", getpid());
			pid = fork();
		if (pid > 0){
			printf("child_1 (PID %d): waiting for child_1.1 (PID %d) to complete\n", getpid(),pid);
			wait(NULL);
			printf("child_1 (PID %d): completed\n", getpid());
		}else{
			printf("child_1.1 (PID %d): calling an external program [%s]\n", getpid(), argv[1]);
			char pidchar[100];
			sprintf(pidchar,"%d",getpid());
			execl(argv[1],strcat(pidchar," for child_1.1"),NULL);
		}
	}
}
