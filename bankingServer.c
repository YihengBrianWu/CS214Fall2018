#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>

#include "bankingServer.h"

Account* head = NULL;
snode* shead = NULL;
int port = -1;

pthread_mutex_t addlock;
pthread_mutex_t startlock;
pthread_mutex_t endlock;

sem_t semaphore;


//return NULL if can't find
//return pointer if find
Account* getacc(char* name){
	Account* ptr = head;
	while(ptr != NULL){
		if(strcmp(ptr->name, name) == 0){
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL;
}

//add account to the link list
//return -1, already exists
//return 0 create success
int addacc(char* name){

	Account* ptr = head;
	pthread_mutex_lock(&addlock);
	
	if(head == NULL){
		Account* new = (Account*)malloc(sizeof(Account));
		if(strlen(name) < 255){
			strncpy(new->name,name,strlen(name));
		}
		else{
			strncpy(new->name,name,255);
			new -> name[255] = '\0';
		}		
		
		//new -> name[strlen(name)] = '\0';
		new -> balance = 0;
		new -> insession = 0;
		new -> next = NULL;
		head = new;
		pthread_mutex_unlock(&addlock);
		return 0;
	}	
	
	while(ptr -> next != NULL){
		if(strcmp(ptr -> name, name) == 0){
			pthread_mutex_unlock(&addlock);
			return -1;
		}
		ptr = ptr -> next;
	}

	if(strcmp(ptr -> name, name) == 0){
		pthread_mutex_unlock(&addlock);
		return -1;
	}
	else{
		Account* new = (Account*)malloc(sizeof(Account));
		if(strlen(name) <= 255){
			strncpy(new->name,name,strlen(name));
		}
		else{
			strncpy(new->name,name,255);
			new -> name[255] = '\0';
		}	
		//new -> name[strlen(name)] = '\0';
		new -> balance = 0;
		new -> insession = 0;
		new -> next = NULL;
		ptr -> next = new;
		pthread_mutex_unlock(&addlock);
		return 0;
	}

}

//start a service for an account
//return 1 strat session successful
//return 0 can't find this account
//return -1 already in session
int startacc(char* name){

	Account* ptr = head;
	while(ptr != NULL){
		if(strcmp(name, ptr->name) == 0){
			pthread_mutex_lock(&startlock);
			if(ptr -> insession == 0){
				ptr -> insession = 1;
				pthread_mutex_unlock(&startlock);
				return 1;
			}else{
				pthread_mutex_unlock(&startlock);
				return -1;
			}
		}
	}

	return 0;

}

/*
//use semaphore to print accountlist
//set semaphore to 0 at firt
void alr(){

	sem_post(&semaphore);

}

void* printAccounts(){

	while(1){
		signal(SIGALRM, alr);
		alarm(15);
		sem_wait(&semaphore);
		Account* ptr = head;
		printf("--------------------------------\n");
		while(ptr!= NULL){
			printf("account name: %s, current balance: %f\n",ptr->name, ptr->balance);
			ptr = ptr->next;
		}
		printf("--------------------------------\n");
	}


	pthread_exit(0);

}*/

void exithandler(){
	snode* ptr = shead;
	char response[] = "exit from serve side";
	
	while(ptr != NULL){
		write(ptr -> sock, response, strlen(response));
		close(ptr -> sock);
		ptr = ptr -> next;	
	}
	exit(0);
}



//client thread
void* clientThread(void* pt){

	int sock = *(int*)pt;
	char buffer[1024];
	//char tempbuffer[1024];

	int insession = 0;
	Account* ptr;

	//int tempcounter = 0;
	while(1){
		int readbytes = read(sock, buffer, 1024);

		/*
		while(tempbuffer[tempcounter] != '\0'){
			printf("wtf???%c\n",tempbuffer[tempcounter]);
				buffer[tempcounter] = tempbuffer[tempcounter];
				tempcounter ++;
		}
		buffer[tempcounter] = '\0';
		*/
		
		printf("%s\n", buffer);

		//command quit
		if(strcmp(buffer, "quit") == 0){
			char response[] = "quitting this service ";
			write(sock, response, strlen(response));
			if(insession == 1){
				pthread_mutex_lock(&endlock);
				ptr -> insession = 0;
				pthread_mutex_unlock(&endlock);
				insession = 0;
			}
			break;
		}

		//command end
		else if(strcmp(buffer, "end") == 0){
			if(insession == 0){
				char response[] = "your are not in an account service";
				write(sock, response, strlen(response));
			}else{
				char response[] = "Ending current session";
				write(sock, response, strlen(response));
				//lock
				pthread_mutex_lock(&endlock);
				ptr -> insession = 0;
				pthread_mutex_unlock(&endlock);
				//unlock
				insession = 0;
				ptr = NULL;
			}
		}

		//command query
		else if(strcmp(buffer, "query") == 0){
			if(insession == 0){
				char response[] = "your are not in an account service";
				write(sock, response, strlen(response));
			}else{
				char* bal = (char*)malloc(sizeof(char)*100);
				memset(bal, 0, 100);
				snprintf(bal, 100, "%lf", ptr->balance);
				write(sock, bal, strlen(bal));
				free(bal);
			}
		}

		//command create
		else if(strncmp(buffer, "create ", 7) == 0){
			char* accountname = strstr(buffer, " ") + 1;
			//printf("%s\n", accountname);
			int a = addacc(accountname);
			if(a == -1){
				char response[] = "account already exists";
				write(sock, response, strlen(response));
			}else{
				char response[] = "create success";
				write(sock, response, strlen(response));
			}
		}

		//comand serve
		else if(strncmp(buffer, "serve ", 6) == 0){
			if(insession != 0){
				char response[] = "One account is already in service";
				write(sock, response, strlen(response));
			}else{
				char* accountname = strstr(buffer, " ") + 1;
				int a = startacc(accountname);
				if(a == 0){
					char response[] = "can't find this account";
					write(sock, response, strlen(response));
				}
				else if(a == -1){
					char response[] = "this account is already in session";
					write(sock, response, strlen(response));
				}
				else if(a == 1){
					insession = 1;
					ptr = getacc(accountname);
					//printf("%s  hahaha\n", ptr->name);
					char response[] = "Now serving this account";
					write(sock, response, strlen(response));
				}
			}
		}

		//command deposit
		else if(strncmp(buffer, "deposit ", 8) == 0){
			if(insession == 0){
				char response[] = "no account in service";
				write(sock, response, strlen(response));
			}else{
				char* accountbalance = strstr(buffer, " ") + 1;
				
				//printf("%s?\n",accountbalance);
				float balance = atof(accountbalance);
				//printf("%s\n", ptr->name);
				ptr -> balance += balance;
				char response[] = "deposit success";
				write(sock, response, strlen(response));
			}
		}

		//command withdraw
		else if(strncmp(buffer, "withdraw ", 9) == 0){
			if(insession == 0){
				char response[] = "no account in service";
				write(sock, response, strlen(response));
			}else{
				char* withdrawmoney = strstr(buffer, " ") + 1;
				float money = atof(withdrawmoney);
				if(ptr->balance - money >= 0){
					ptr -> balance -= money;
					char response[] = "withdraw success";
					write(sock, response, strlen(response));
				}else{
					char response[] = "you don't have enough money in this account";
					write(sock, response, strlen(response));
				}
			}
		}

		//client terminated
		else if(readbytes == 0){
			printf("Connetion terminated\n");
			if(insession == 1){
				ptr -> insession = 0;
				insession = 0;
			}
			break;
		}
		else{
			char err[] = "wrong command";
			write(sock, err, strlen(err));
		}

		memset(buffer, 0, 1024);
	}

	char response[] = "server shut down, close connection";
	write(sock, response, strlen(response));
	close(sock);

}

void* listencon(void* listensock){
	int* sock = (int*)listensock;
	memset(sock, 0, sizeof(int));

	(*sock) = socket(AF_INET, SOCK_STREAM, 0);

	if((*sock) <= 0){
		fprintf(stderr, "can't create listening socket\n");
		exit(-1);
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	socklen_t len = sizeof(address);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if(bind(*sock, (struct sockaddr*)&address, sizeof(address)) < 0){
		fprintf(stderr, "can't bind listening socket\n");
		exit(-1);
	}

	if(listen(*sock, 10) < 0){
		fprintf(stderr, "can't listen\n");
		exit(-1);
	}
	while(1){
		int* newsock = (int*)malloc(sizeof(int));
		*newsock = accept(*sock, (struct sockaddr*)&address, &len);
		if(*newsock < 0){
			fprintf(stderr, "can't create new client socket\n");
			break;
		}
		printf("Accept a new connection\n");
		snode* temp = (snode*)malloc(sizeof(temp));
		temp -> sock = *newsock;
		temp -> next = shead;
		shead = temp;
		pthread_t* Cthread = (pthread_t*)malloc(sizeof(pthread_t));
		pthread_create(Cthread, NULL, clientThread, (void*)newsock);
	}

	pthread_exit(0);

}

int main(int argc, char** argv){

	if(argc != 2){
		fprintf(stderr, "invalid number of arguments\n");
		return -1;
	}

	port = atoi(argv[1]);

	pthread_mutex_init(&addlock, NULL);
	pthread_mutex_init(&startlock, NULL);
	pthread_mutex_init(&endlock, NULL);
	
	//sem_init(&semaphore, 0, 0);
	
	signal(SIGINT, exithandler);

	int* listensock = (int*)malloc(sizeof(int));
	pthread_t* listenthread = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(listenthread, NULL, listencon, (void*)listensock);
	
	//pthread_t* printthread = (pthread_t*)malloc(sizeof(pthread_t));
	//pthread_create(printthread, NULL, printAccounts, NULL);
	//pthread_detach(*printthread);
	
	pthread_join(*listenthread, NULL);

	pthread_mutex_destroy(&addlock);
	pthread_mutex_destroy(&startlock);
	pthread_mutex_destroy(&endlock);

	printf("server shutting down\n");
}