#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int r;

    r = fork(); // cria processo

    if (r == 0) {
        printf("Processo filho de ID: %d \n", getpid());
        printf("Processo filho de: %d \n", getppid());
    
    }

    else if (r > 0) {
    	wait();
        printf("Eu sou o processo pai de ID: %d \n", getpid());
        printf("Eu sou o processo pai de %d com pai de id:%d\n", r, getppid());
    }
    else {
        printf("Erro ao tentar criar processo filho\n");
    exit(0);
    }

    return 0;
}
