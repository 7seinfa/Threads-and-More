#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Process { //structure to store process and info
	int totBrst; //total bursts to complete
	int curBrst; //current bursts
	int curQuantam; //how many times it has run under current quantam
	int arrTime; //arrival time
	char* processName; //process name
	int waitTime; //wait time
	int turnAround; //turn around time
};

int main (int argc, char **argv){
	FILE* file = fopen("rr_input.txt", "r");
	if (!file) {
		return 1;
	}
	char line[128];
	int curLine = 0;
	while (fgets(line, 128, file)){ //load line
		curLine++;
		printf("Test case #%d: %s", curLine, line);
		int len = 0;
		for (int x = 0; x < strlen(line); x++){ //see how many spaces there are
			if(line[x]==' ') len++;
		}
		struct Process processes[(len+1)/3]; //amount of processes is amount of spaces +1 divided by 3 (process name, burst time, arrival time)
		char* curPtr = strtok(line," "); //split line by space
		int i = 0;
		int curPr = -1; //current process
		while (i < len){
			switch (i%3){
				case 0: //whenever i is divisible by three, we start a new process
					curPr++; 
					processes[curPr].processName = curPtr;
					break;
				case 1: //if i%3 = 1, then we are looking at arrival time
					processes[curPr].arrTime = atoi(curPtr);
					break;
				case 2: //if i%3 = 2, then we are looking at burst time, initialize everything else too
					processes[curPr].totBrst = atoi(curPtr);
					processes[curPr].curBrst = 0;
					processes[curPr].curQuantam = 0;
					processes[curPr].waitTime = -1;
					processes[curPr].turnAround = -1;
					break;
			}
			i++;
			curPtr = strtok(NULL, " ");
		}
		int quantam = atoi(curPtr); //value after last space is quantam
		int curTime = 0; //store current time
		char *queue[(len+1)/3]; //create a queue with the number of processes, initialize each value in queue with "NULL"
		for (int x = 0; x < (len+1)/3; x++){
			queue[x] = "NULL";
		}
		i = 0;
		printf("Number of Processes: %d, Quantam: %d\nProcess Scheduling Started\n", (len+1)/3, quantam);
		float avgTurn = 0.00;
		float avgWait = 0.00;
		int finished = 0;
		while (finished==0){ //while not finished
			int check = 1;
			i = 0;
			while (i < (len+1)/3){ //make sure we are there is still a process not finished to decide if we should display next CPU time
				if (processes[i].curBrst < processes[i].totBrst){
					check = 0;
				}
				i++;
			}
			if (check==0)
				printf("CPU Time %d: ", curTime);
			i = 0;
			while(i < (len+1)/3){ //check for new arrivals at current cpu time, add to queue
				if (processes[i].arrTime == curTime) {
					printf("[%s Arrived] ", processes[i].processName);		
					int x = 0;
					while (queue[x] != "NULL") {
						x++;
					}
					queue[x] = processes[i].processName;
				}
				i++;
			}
			int didTime = 0;
			i = 0;
			while (didTime==0){ //try to complete a task
				if (i < (len+1)/3){ //make sure we are not out of range
					if (processes[i].curQuantam < quantam && processes[i].curBrst < processes[i].totBrst && queue[0] == processes[i].processName){
						//find first item in queue, make sure it is not completed, then finish one burst of it
						processes[i].curBrst++;
				      	processes[i].curQuantam++;       
						printf("%s [%d/%d]\n", processes[i].processName, processes[i].curBrst, processes[i].totBrst);
						if (processes[i].curBrst>=processes[i].totBrst){ //if we finish the process, then find turn around and wait time
							processes[i].turnAround = curTime - processes[i].arrTime+1;
							processes[i].waitTime = processes[i].turnAround - processes[i].totBrst;
							avgTurn+=processes[i].turnAround;
							avgWait+=processes[i].waitTime;
							printf("Process %s completed with Turn Around Time: %d, Waiting Time: %d\n", processes[i].processName, processes[i].turnAround, processes[i].waitTime);
						}

						if (processes[i].curQuantam >= quantam || processes[i].curBrst >= processes[i].totBrst){
							//if we have done this turn of quantam, move to end of queue, if finished then remove from queue
							int x = 0;
							while (x < (len+1)/3-1){
								queue[x] = queue[x+1];
								queue[x+1]="NULL";
								x++;
							}
							x = 0;
							while (x < (len+1)/3 && queue[x]!="NULL") x++;
							if (processes[i].curBrst<processes[i].totBrst) queue[x] = processes[i].processName;
						}
						didTime = 1; //we have updated something so end while loop
					}
					i++;
				}else{ //if we've checked every item in process list, then check to see if we have finished every process or if they're at max quantam time
					finished = 1;
					didTime = 1;
					i = 0;
					while (i < (len+1)/3){ //if not finished, and it has arrived, set quantam to 0, we not done yet
						if (processes[i].curBrst < processes[i].totBrst && processes[i].arrTime <= curTime) {
							processes[i].curQuantam = 0;
							finished = 0;
							didTime = 0;
						}
						i++;
					}
					if (finished == 1){ //if we didn't find any arrived unfinished processes
						i = 0;
						while (i < (len+1)/3){ //if not finished but hasn't arrived, display "None", we done this time
							if (processes[i].curBrst < processes[i].totBrst){
								didTime=1;
								finished=0;
								printf("None\n");
								i=(len+1)/3;
							}
							i++;
						}
					}
					i = 0;
				}	
			}
			curTime++;
		}
		avgTurn = avgTurn / ((len+1)/3);
		avgWait = avgWait / ((len+1)/3);
		printf("Process scheduling completed with Avg Turn Around Time: %.2f, Avg Waiting Time: %.2f\n\n", avgTurn, avgWait);

	}
	return 0;
}
