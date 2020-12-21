#include <pthread.h >
#include <stdio.h>

void *f_thread();

int main(){
	pthread_t tid;
	pthread_create(&tid, NULL, f_thread, NULL);
	
	pthread_join(tid, NULL);
	
	return 0;
}

void *f_thread(){
	for(;;){
		printf("Thread executando...\n");
	}
}
