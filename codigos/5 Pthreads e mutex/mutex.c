#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void *a_thread();
int cont = 0;

int main(){
	int i, n;
	printf("numero de thread: ");
	scanf("%d", &n);
	
	pthread_t *tid = malloc(sizeof(pthread_t));
	
	for( i = 0; i < n; i++){
		
		pthread_create(tid, NULL, a_thread, NULL);
		
	}
	pthread_join(*tid, NULL);
	
	printf("contador: %d\n", cont);
	printf("fim da thread principal\n");
	return 0;
}

void *a_thread(){
	int i;
	printf("Nova thread criada. TID = %ld\n", pthread_self());
	for(i = 0; i < 100; i++){
		cont++;
	}
}

