//13:30


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define ADDRESS 127.0.0.1
#define CLIENTS 5
#define BUFFER 256

struct metadado
{
	int position_inodes;
	int position_files;
	int size_filesystem;
	int size_freespace;
	int size_blocks;
	
}file_system;


struct inode
{
	char name[20];
	time_t creation_time;
	int start;
	int size;
};
FILE *FS;

pthread_mutex_t mutexserver;
pthread_mutex_t mutexdisk;


int client(int network_socket, struct sockaddr_in server_address);
void menu();
int server(int network_socket, struct sockaddr_in server_address);
void *server_thread(void* socket_thread);
void *disk_thread();
void create_file_system();
int execute_comand( char *message);
int touch(char *file_name, char *file_content);

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
	char client_message[BUFFER];
	struct sockaddr_in address = server_address;
	
	int connection_status = connect(client_socket, (struct sockaddr*)&address, sizeof(address));
	
	
	if (connection_status == -1)
	{	
		perror("Error");
		printf("Connecting server...\n\n");
		remove("file_system.bin");
		return 1;
	}
	
	while(1)  {
		printf("DO NOT FINISH CLIENT WITH CTRL + C\n");
		//menu();
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
	/*
	printf("\n	COMANDS ACCEPTED							");
    printf("\nLIST                  -> 'ls' 				");
    printf("\nMAKE DIRECTORY        -> 'mkdir arg'          ");
    printf("\nMAKE ARQUIVE          -> 'touch arg'          ");
    printf("\nCOPY                  -> 'cp arg0 arg1'       ");
    printf("\nREMOVE ARQUIVE        -> 'rm arg, rm -r arg'  ");
    printf("\nREMOVE DIRECTORY      -> 'rmdir arg, rm -r arg'  ");
    printf("\nCLOSE CLIENT          -> 'exit'  ");
    printf("\n\n");
    */
}

int server(int network_socket, struct sockaddr_in server_address)
{	
	int server_socket = network_socket;
	int connection_socket, *socket_thread;
	struct sockaddr_in address = server_address;
	struct sockaddr_in client;
	int pipe_limit = 1;
	
	if((access("file_system.bin", F_OK)) != 0 )
	{
		create_file_system();
	}
	
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
	
	unlink("server_pipe");
	unlink("disk_pipe");
	
	pthread_mutex_init(&mutexserver, 0);
	pthread_mutex_init(&mutexdisk, 0);
	
	//int size = sizeof(client);
	//while(connection_socket = accept(server_socket, (struct sockaddr*)&client, &size))
	while(connection_socket = accept(server_socket, NULL, NULL) )
	{
		pthread_t tserver;
		socket_thread = malloc(sizeof(connection_socket));
		*socket_thread = connection_socket;
		pthread_create(&tserver,NULL,server_thread,(void*)socket_thread);
		
		if(pipe_limit == 1)
		{
			pthread_t tdisk;
			pthread_create(&tdisk,NULL,disk_thread,NULL);
			pipe_limit = 0;
		}
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
	
	mkfifo("server_pipe", 0666);
	mkfifo("disk_pipe", 0666);
	int server_pipe = open("server_pipe", O_WRONLY);
	int disk_pipe = open("disk_pipe", O_RDONLY);
	
	while(1)
	{	
		if( recv(thread_socket, client_response, sizeof(client_response), 0) < 0)
		{
			perror("Server recv error");
			break;
		}
		if( write(server_pipe, client_response, sizeof(client_response)) < 0)
		{
			perror("Server write error");
			break;
		}
		
		//printf("client %i, server:", thread_socket);
		//puts(client_response);
		
		/*
		pthread_mutex_lock(&mutexserver);
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
		pthread_mutex_unlock(&mutexserver);
		*/
		
		if( read(disk_pipe, server_message, sizeof(server_message)) < 0)
		{
			perror("Server read error");
			break;
		}
		if( send(thread_socket, server_message, sizeof(server_message), 0) < 0)
		{
			perror("Server send error");
			break;
		}
		if( !strcmp(client_response,"exit\n"))
		{	
			break;
		}
	}
	pthread_exit(0);
}

void *disk_thread()
{
	char disk_message[BUFFER];
	char server_response[BUFFER];
	int server_pipe = open("server_pipe", O_RDONLY);
	int disk_pipe = open("disk_pipe", O_WRONLY);
	int check;
	
	while(1)
	{	
		sleep(1);
		if( read(server_pipe, server_response, sizeof(server_response)) < 0)
		{
			perror("Disk read error");
			break;
		}
		
		pthread_mutex_lock(&mutexdisk);
		check = execute_comand(server_response);
		pthread_mutex_unlock(&mutexdisk);
		
		if(check == 1)
		{
			strncpy(disk_message, "sucefull", BUFFER);
		}
		else
		{
			strncpy(disk_message, "failed", BUFFER);
		}
		
		//printf("client disk:");
		//puts(server_response);
		//strncpy(disk_message, server_response, BUFFER);
		
		if( write(disk_pipe, disk_message, sizeof(disk_message)) < 0)
		{
			perror("Disk write error");
			break;
		}
		if( !strcmp(server_response,"exit\n"))
		{	
			break;
		}
	}
	pthread_exit(0);
}


void create_file_system()
{
	int i;
	file_system.size_filesystem = 1024;
	file_system.size_blocks = 4;
	file_system.position_inodes = 24;
	file_system.position_files = 400;
	file_system.size_freespace = 624;
	
	/*
	printf( "file system: %d\n", file_system.size_filesystem);
	printf( "blocks: %d\n", file_system.size_blocks);
	printf( "inodes position: %d\n", file_system.position_inodes);
	printf( "file positon: %d\n", file_system.position_files);
	printf( "free space: %d\n", file_system.size_freespace);
	*/
	
	FS = fopen ("file_system.bin", "wb");
	if(FS != NULL)
	{	
		for(i = 0; i < 1024; i++)
		{
			fputc(0, FS);
		}
		fseek ( FS, 1, SEEK_SET );
		fwrite(&file_system, sizeof(struct metadado), 1, FS);
	}
	else
	{
		exit(0);
	}
	fclose(FS);
}

int execute_comand( char *message)
{
	char file_name[BUFFER], file_content[BUFFER], comand[BUFFER], *aux;
	int check;
	puts(message);
	strcpy(comand, strtok(message, " "));
	strcpy(file_name, strtok(NULL, " "));
	while( (aux = strtok(NULL, " ")) != NULL)
	{
		strcat(file_content, aux);
	}
	
	printf("comand do execute1\n");
	puts(comand);
	printf("file name do execute2\n");
	puts(file_name);
	printf("file content do execute3\n");
	puts(file_content);
	
	check = touch(file_name, file_content);
	
	return check;
}


int touch(char *file_name, char *file_content)
{
	struct inode node;
	FS = fopen ("file_system.bin", "wb");
	
	printf("file name do touch1\n");
	puts(file_name);
	printf("file content do touch2\n");
	puts(file_content);
	
    strcpy(node.name,file_name);
    node.size = strlen(file_content);
    node.creation_time = time(NULL);
    node.start = file_system.size_filesystem - file_system.size_freespace;
    //printf("%d", node.start);

    file_system.size_freespace -=  ceil(node.size);
	
	rewind(FS);
    fseek ( FS, file_system.position_inodes, SEEK_SET );
    fwrite (&node, sizeof(struct inode), 1, FS);
    
    file_system.position_inodes += sizeof(struct inode);
	
	rewind(FS);
    fseek ( FS, node.start, SEEK_SET );
  
    fwrite (&file_content, strlen(file_content), 1, FS);
	
	rewind(FS);
    fseek ( FS, 0, SEEK_SET );
    fwrite(&file_system, sizeof(file_system), 1, FS);
    
    printf("opa1\n");
    
    fclose(FS);
    return 1;
}
