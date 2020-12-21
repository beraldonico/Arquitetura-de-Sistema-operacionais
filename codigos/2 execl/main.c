#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int r;

    r = fork();

	if(r == 0){
    	//printf("bin feito") ;
		execl("/bin/ls" ,"-la", NULL);
    }
    else if(r > 0){
    	wait();
    	printf("processo pai terminado\n") ;
    }
    return 0;
}
