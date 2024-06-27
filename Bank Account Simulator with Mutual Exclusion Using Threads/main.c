#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *client (void* line);

int balances[124]; //store balances of each account
pthread_mutex_t lock; //use to lock account

int main (int argc, char **argv) {
	//open file
	FILE* file = fopen("assignment_5_input.txt", "r");
	if (!file) return 1;
	
	//number of accounts and clients
	int numAccount = 0;
	int numClient = 0;

	pthread_t clients[256]; //store threads of clients
	char clientLines[256][256]; //stores lines that clients need to read
	char line[256];
	while (fgets(line, 256, file)){ //go through each line of file
		//copy the line into a string
		if (line[0]=='a') { //if the line starts with a, we are looking at an account
			//we want to go to the second word to check if we are looking at balance
			char* ptr = strtok(line, " ");
			ptr = strtok(NULL, " ");
			if (strcmp(ptr,"balance")==0){ 	//if we're looking at balance, go to the next word,
											//which will be the balance, and add it to our balances variable
				ptr = strtok(NULL, " ");
				balances[numAccount] = atoi(ptr);
			}
			numAccount++;
		}else{ //if it doesn't start with a, we're looking at a client, so create and join a thread for this client, and give it the line
			strcpy(clientLines[numClient],line);
			if (pthread_create(&clients[numClient], NULL, client, clientLines[numClient])){ 
				printf("Error creating thread\n");
				exit(0);
			}
			//pthread_join(clients[numClient], NULL);
			numClient++;
		}
	}

	int i;
	for (i = 0; i < numClient; i++){
		pthread_join(clients[i], NULL);
	}

	printf("No. of Accounts: %d\n", numAccount);
	printf("No. of Clients: %d\n", numClient);
	for (i = 0; i < numAccount; i++){
		printf("account%d, balance: %d\n", i+1, balances[i]);
	}
	pthread_mutex_destroy(&lock);
}

void withdraw(int accNum, int amount){ //withdraw amount from account accNum
	pthread_mutex_lock(&lock);
	if (amount <= balances[accNum]) balances[accNum] -= amount;
	pthread_mutex_unlock(&lock);
}

void deposit(int accNum, int amount){ //deposit amount in account accNum
	pthread_mutex_lock(&lock);
	balances[accNum] += amount;
	pthread_mutex_unlock(&lock);
}

void *client(void *line){ //client thread function, parses line and withdraws/deposits
	//skip to the second word
	char* ptr = strtok(line, " ");
	ptr = strtok(NULL, " ");
	while (ptr != NULL) { //as long as there are words left in the string
		if (strcmp(ptr,"deposit")==0) { //if we need to deposit
			//skip to after the t in "account" and get account number
			ptr = strtok(NULL, "t");
			ptr = strtok(NULL, " ");
			int accNum = atoi(ptr) - 1;
			//skip to next word for amount to deposit, and run deposit function
			ptr = strtok(NULL, " ");
			deposit(accNum, atoi(ptr));
		} else if(strcmp(ptr,"withdraw")==0){ //if we need to withdraw
			//skip to after the t in "account" and get account number
			ptr = strtok(NULL, "t");
			ptr = strtok(NULL, " ");
			int accNum = atoi(ptr) - 1;
			//skip to next word for amount to withdraw, and run withdraw function
			ptr = strtok(NULL, " ");
			withdraw(accNum,atoi(ptr));
		}
		ptr = strtok(NULL, " "); //go to next word
	}
}


