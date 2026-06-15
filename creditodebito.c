//Laboratorio 3
//Alumnas: Azul Barra y Alexia Insua

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>

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
    }

    fclose(f);
    close(p[1]);

    exit(0);
}

int main()
{
    int pipeCredito[2];
    int pipeDebito[2];

    pid_t hijoCredito;
    pid_t hijoDebito;

    double monto;
    ssize_t leidos;

    int creditoFinalizado = 0;
    int debitoFinalizado = 0;

    mem = mmap(NULL, sizeof(memoriaCompartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

}