#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define ADDRESS 127.0.0.1
#define CLIENTS 5
#define BUFFER 256

pthread_mutex_t mutex;

int client(int network_socket, struct sockaddr_in server_address);
void menu();
int server(int network_socket, struct sockaddr_in server_address);
void *server_thread(void* socket_thread);

int main ()
{
	int option;
	
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
	//server_address.sin_addr.s_addr = inet_addr("ADDRESS");
	
	while(1)
	{
		if( client(network_socket, server_address) )
		{
			if( server(network_socket, server_address))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return 0;
}

int client(int network_socket, struct sockaddr_in server_address)
{
	int client_socket = network_socket;
	char server_response[BUFFER];
	char client_message[BUFFER] = "Client connected to server";
	struct sockaddr_in address = server_address;
	
	int connection_status = connect(client_socket, (struct sockaddr*)&address, sizeof(address));
	
	
	if (connection_status == -1)
	{	
		printf("Client error !!!\n");
		perror("Error");
		printf("Connecting server...\n\n");
		return 1;
	}
	else
	{
		send(client_socket, client_message , sizeof(client_message) , 0);
		recv(client_socket, server_response, sizeof(server_response), 0);

		puts(server_response);
	}
	
	while(1)  {
		printf("DO NOT FINISH CLIENT WITH CTRL + C\n");
		menu();
        printf("INSERT COMAND: ");
        fgets(client_message, BUFFER, stdin);
        if( send(client_socket, client_message , sizeof(client_message) , 0) < 0) {
            perror("Error");
            break;
        }
        if( recv(client_socket, server_response , sizeof(server_response), 0) < 0){
            perror("Error");
            break;
        }
		if( !strcmp(client_message,"exit\n"))
        {	
        	printf("Finishing client...\n\n");
        	break;
        }
        system("clear");
        printf("Server Response:");
        puts(server_response);
        
	}
	return 0;
}

void menu()
{
	printf("\n	COMANDS ACCEPTED							");
    printf("\nLIST                  -> 'ls' 				");
    printf("\nMAKE DIRECTORY        -> 'mkdir arg'          ");
    printf("\nMAKE ARQUIVE          -> 'touch arg'          ");
    printf("\nCOPY                  -> 'cp arg0 arg1'       ");
    printf("\nREMOVE ARQUIVE        -> 'rm arg, rm -r arg'  ");
    printf("\nREMOVE DIRECTORY      -> 'rmdir arg, rm -r arg'  ");
    printf("\nCLOSE CLIENT          -> 'exit'  ");
    printf("\n\n");
}

int server(int network_socket, struct sockaddr_in server_address)
{	
	int server_socket = network_socket;
	int connection_socket, *socket_thread;
	struct sockaddr_in address = server_address;
	struct sockaddr_in client;
	
	if(bind(server_socket, (struct sockaddr*) &address, sizeof(address)) < 0)
	{
		printf("Server error !!!\n");
		perror("Bind failed !!! Error");
		printf("\n");
		return 0;
	}
	
	printf("Server connected\n");
	printf("Shutdown server with Ctrl + C\n\n");
	listen(server_socket, CLIENTS);
	
	//int size = sizeof(client);
	//while(connection_socket = accept(server_socket, (struct sockaddr*)&client, &size))
	while(connection_socket = accept(server_socket, NULL, NULL) )
	{
		pthread_mutex_init(&mutex, 0);
		pthread_t tserver;
		socket_thread = malloc(sizeof(connection_socket));
		*socket_thread = connection_socket;
		pthread_create(&tserver,NULL,server_thread,(void*)socket_thread);
	}
	return 1;
}

void *server_thread(void* socket_thread)
{
	int thread_socket = *(int*)socket_thread;
	char server_message[BUFFER];
	char client_response[BUFFER];
	FILE *comand;
	char message[BUFFER];
	
	while(1)
	{	
		if( recv(thread_socket, client_response, sizeof(client_response), 0) < 0)
		{
			perror("Error");
			break;
		}
		printf("%i:", thread_socket);
		puts(client_response);
		
		pthread_mutex_lock(&mutex);
		if( strcmp(client_response,"Client connected to server") != 0)
		{	
			comand = popen(client_response, "r");
			while (fgets(message, BUFFER, comand) != NULL)
   			{	
   		    	strcat(client_response,message);
   			}
			pclose(comand);
			strncpy(server_message, client_response, BUFFER);
		}
		pthread_mutex_unlock(&mutex);
		
		if( send(thread_socket, server_message, sizeof(server_message), 0) < 0)
		{
			perror("Error");
			break;
		}
		if( !strcmp(client_response,"exit\n"))
		{	
			break;
		}
	}
	pthread_exit(0);
}
