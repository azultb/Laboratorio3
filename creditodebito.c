//Laboratorio 3
//Alumnas: Azul Barra y Alexia Insua

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>

typedef struct 
{
    sem_t semaforo;
    double saldo;
} memoriaCompartida;

memoriaCompartida *mem;

void credito(char *archivo_montos, int p[])
{
    FILE *f;
    double monto;

    f=fopen(archivo_montos,"r");

    if(f==NULL)
    {
        perror("Error al abrir credito.txt");
        exit(1);
    }

    while(fscanf(f,"%lf",&monto)==1)
    {
        sem_wait(&mem->semaforo);
        mem->saldo += monto;
        sem_post(&mem->semaforo);

        write(p[1], &monto, sizeof(double));
        usleep(1000);
    }

    fclose(f);
    close(p[1]);

    exit(0);
}

void debito(char *archivo_montos, int p[])
{
    FILE *f;
    double monto;

    f=fopen(archivo_montos,"r");

    if(f==NULL)
    {
        perror("Error al abrir debito.txt");
        exit(1);
    }

    while(fscanf(f,"%lf",&monto)==1)
    {
        sem_wait(&mem->semaforo);
        mem->saldo -= monto;
        sem_post(&mem->semaforo);

        write(p[1], &monto, sizeof(double));
        usleep(1000);
    }

    fclose(f);
    close(p[1]);

    exit(0);
}

int main()
{
    
}