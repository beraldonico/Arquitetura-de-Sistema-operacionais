#include <pthread.h>
#include <stdio.h>

void *a_thread();
void *b_thread();
int q, x, d = 0;

int main(){
	printf("insir um numero para checar:");
	scanf("%d", &x);
	q = x/2;
	
	pthread_t tid1;
	pthread_t tid2;
	pthread_create(&tid1, NULL, a_thread, NULL);
	pthread_create(&tid2, NULL, b_thread, NULL);
	
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	
	if(d != 1){
		printf("%d é primo\n" ,x);
	}
	else{
		printf("%d não é primo\n" ,x);
	}
	
	return 0;
}

void *a_thread(){
	int i;
	for( i = 2; i <= q; i++){
		if( x%i == 0){
			d = 1;
		}
	}
	printf("Nova thread criada. TID = %d\n", pthread_self());
}

void *b_thread(){
	int j;
	for( j = q + 1; j < x; j++){
		if( x%j == 0){
			d = 1;
		}
	}
	printf("Nova thread criada. TID = %d\n", pthread_self());
}
