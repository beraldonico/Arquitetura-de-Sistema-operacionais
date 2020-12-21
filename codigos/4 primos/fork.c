#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int main(){
	int x;
	int r, t, q, i, j, d = 0;
	printf("insir um numero para checar:");
	scanf("%d", &x);
	r = fork();
	t = fork();
	q = x/2;
	if( r == 0){
		for( i = 1; i < q; i++){
			if( x%i != 0){
				d = 1;
			}
		}
	}
	if( t == 0){
		for( j = q; j >= x; j++){
			if( x%j != 0){
				d = 1;
			}
		}
	}
	
	wait();
	if( d != 1){
		printf(" %d é primo\n" ,x);
	}
	
	return 0;
}
