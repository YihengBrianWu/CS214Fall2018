#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include"bankingClient.h"

int sockN = -1;
volatile int status = 0;
pthread_mutex_t lock;

int main(int argc, char** argv){
	if(argc != 3){
		fprintf(stderr, "Invalid argument number\n");
		exit(0);
	}

	int portno = atoi(argv[2]);
	sockN = socket(AF_INET, SOCK_STREAM, 0);

	if(sockN < 0){
		fprintf(stderr, "can't create socket\n");
		exit(0);
	}

	struct hostent* server = gethostbyname(argv[1]);

	if(server == NULL){
		fprintf(stderr, "can't find this host\n");
		exit(0);
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	bcopy((char*)server -> h_addr, (char*)&address.sin_addr.s_addr, server->h_length);
	address.sin_port = htons(portno);

	int con = 0;

	while(1){
		con = connect(sockN,(struct sockaddr*)&address, sizeof(address));
		if(con < 0){
			printf("try again to connect\n");
			sleep(2);
		}else{
			printf("success connecting\n");
			break;
		}
	}
	
	signal(SIGINT,intHandler);
	
	pthread_mutex_init(&lock,NULL);
	
	pthread_t thread1;
	pthread_t thread2;

	pthread_create(&thread1, NULL, input, NULL);
	pthread_create(&thread2, NULL, receive , NULL);

	while(1){
		pthread_mutex_lock(&lock);
		if(status == 0)
		{
			pthread_mutex_unlock(&lock);
			continue;
		}
		else if(status == 1)
		{
			pthread_mutex_unlock(&lock);
			pthread_join(thread1, NULL);
			pthread_join(thread2, NULL);
			break;
		}
		else if(status == 2)
		{
			pthread_mutex_unlock(&lock);
			pthread_join(thread2, NULL);
			pthread_cancel(thread1);
			break;
		}
		else if(status == 3)
		{	
			char* temp = "-1";
			send(sockN , temp , strlen(temp) , MSG_NOSIGNAL);
			printf("SIGINT detected, disconnecting server and closing client...");
			pthread_cancel(thread1);
			pthread_cancel(thread2);
			break;
		}
	}

	close(sockN);
	pthread_mutex_destroy(&lock);
	return 0;
}

void* receive(void* ptr){
	int size = 1024;
   char* buffer = (char*)malloc(size * sizeof(char));
   pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
			
	while(1){
		pthread_mutex_lock(&lock);
		if(status == 0)
		{
			pthread_mutex_unlock(&lock);
		}
		else if(status == 1)
		{
			pthread_mutex_unlock(&lock);
			pthread_exit(NULL);
		}
		
		if(recv(sockN , buffer , size , 0 ) > 0)
		{
			printf("%s\n",buffer);
		}
		else
		{
			pthread_mutex_lock(&lock);
			status = 2;			
			pthread_mutex_unlock(&lock);	
				
			printf("The server is disconnected, closing client\n");
			pthread_exit(NULL);	
		} 
		//printf("Please wait....\n");
		

		memset(buffer,0,strlen(buffer));
	}
	

			
}

void* input(void* ptr){
	char readIn = '\0';
	int size = 1024;
   char* buffer = (char*)malloc(size * sizeof(char));
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	int count = 0;
	
	while(1){
		printf("Enter command:\n");
		read(0,&readIn,1);
		while(readIn != '\n')
		{
			buffer[count] = readIn;
			count++;

			if(count == 1023)
			{
				size += size;
				buffer = (char*)realloc(buffer, size);
			}

			read(0,&readIn,1);
		}

		buffer[count] = '\0';
		count = 0;

		if(strcmp(buffer,"quit") == 0){
			
			if(send(sockN , buffer , strlen(buffer) , MSG_NOSIGNAL ) > 0)
			{
					memset(buffer,0,strlen(buffer));		
			}
			else
			{
					pthread_mutex_lock(&lock);
					status = 1;			
					pthread_mutex_unlock(&lock);	
					
				printf("Failed to send the message to server, closing client\n");
				pthread_exit(NULL);
			}
			
			pthread_mutex_lock(&lock);
			status = 1;			
			pthread_mutex_unlock(&lock);	
			
			pthread_exit(NULL);
		}
		
		if(checkinput(buffer) == 1){
			printf("Invalid command!\nValid commands are:\ncreate accountname\nserve accountname\n");
			printf("deposit amount\nwithdraw amount\nquery\nend\nquit\n");
			memset(buffer,0,strlen(buffer));
			continue;
		}

		if(send(sockN , buffer , strlen(buffer) , MSG_NOSIGNAL) > 0)
		{
			memset(buffer,0,strlen(buffer));
		}
		else
		{
			printf("Failed to send the message to server, closing client\n");
			
			pthread_mutex_lock(&lock);
			status = 1;			
			pthread_mutex_unlock(&lock);	
					
			pthread_exit(NULL);
		}
		
		sleep(2);
	}
	
}

void intHandler(){
		pthread_mutex_lock(&lock);
		status = 3;			
		pthread_mutex_unlock(&lock);	
}
/*
	Return 0 if input is valid, 1 if input is invalid.
*/
int checkinput(char* buffer){
	
	if(strcmp(buffer, "end") == 0){
		return 0;
	}
	else if(strcmp(buffer, "query") == 0){
		return 0;
	}
	else if(strncmp(buffer, "create",6) == 0){
		if(buffer[6] != ' ' || strlen(buffer) < 8)
		{
			return 1;
		}
		else 
		{
			return 0;
		}		
	}
	else if(strncmp(buffer, "serve",5) == 0){
		if(buffer[5] != ' ' || strlen(buffer) < 7)
		{
			return 1;
		}
		else 
		{
			return 0;
		}
		
	}
	else if(strncmp(buffer, "deposit", 7) == 0){
		if(buffer[7] != ' ' || strlen(buffer) < 9)
		{
			return 1;
		}
		else 
		{
			char* amount = strstr(buffer, " ") + 1;
			if(isdouble(amount) == 0)
			{
				return  0;
			}
			else 
			{
				return 1;
			}
		}
	}
	else if(strncmp(buffer, "withdraw", 8) == 0){
		if(buffer[8] != ' ' || strlen(buffer) < 10)
		{
			return 1;
		}
		else 
		{
			char* amount = strstr(buffer, " ") + 1;
			if(isdouble(amount) == 0)
			{
				return  0;
			}
			else 
			{
				return 1;
			}
		}
	}
	else{
		return 1;	
	}
}

int isdouble(char* buffer){
	
	char a = '\0';
	int point = 0;
	int c = 0;
	
	while(c < strlen(buffer)){
		
		a = buffer[c];
		
		if(a == '.')
		{
			if(point == 1)
			{
				return 1;
			}
			else
			{
				point = 1;
			}
		}
		else if(a < '0' || a > '9')
		{
			return 1;	
		}

		c++;
	}
	return 0;
}


