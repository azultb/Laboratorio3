//Laboratorio 3
//Alumnas: Azul Barra y Alexia Insua
//https://github.com/azultb/Laboratorio3.git

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
    int pipeCredito[2];
    int pipeDebito[2];

    pid_t hijoCredito;
    pid_t hijoDebito;

    double monto;
    ssize_t leidos;

    int creditoFinalizado = 0;
    int debitoFinalizado = 0;

    mem = mmap(NULL, sizeof(memoriaCompartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if(mem==MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    sem_init(&mem->semaforo, 1, 1);
    mem->saldo = 0;

    if(pipe(pipeCredito)==-1)
    {
        perror("Error al crear pipeCredito");
        exit(1);
    }

    if(pipe(pipeDebito)==-1)
    {
        perror("Error al crear pipeDebito");
        exit(1);
    }

    fcntl(pipeCredito[0], F_SETFL, O_NONBLOCK);
    fcntl(pipeDebito[0], F_SETFL, O_NONBLOCK);

    hijoCredito = fork();
    if(hijoCredito==0)
    {
        close(pipeCredito[0]);
        close(pipeDebito[0]);
        close(pipeDebito[1]);
        credito("credito.txt", pipeCredito);
    }

    hijoDebito = fork();
    if(hijoDebito==0)
    {
        close(pipeDebito[0]);
        close(pipeCredito[0]);
        close(pipeCredito[1]);
        debito("debito.txt", pipeDebito);
    }

    close(pipeCredito[1]);
    close(pipeDebito[1]);

    while(!creditoFinalizado || !debitoFinalizado)
    {
        if(!creditoFinalizado)
        {
            leidos = read(pipeCredito[0], &monto, sizeof(double));
            if(leidos>0)
            {
                printf("Credito: %.2f, monto);
            }
            else if(leidos==0)
            {
                creditoFinalizado = 1;
                close(pipeCredito[0]);
            }
        }

        if(!debitoFinalizado)
        {
            leidos = read(pipeDebito[0], &monto, sizeof(double));
            if(leidos>0)
            {
                printf("Debito: %.2f, monto);
            }
            else if(leidos==0)
            {
                debitoFinalizado = 1;
                close(pipeDebito[0]);
            }
        }
    }
    wait(NULL);
    wait(NULL);

    printf("\nSaldo final: %.2f\n", mem->saldo);

    sem_destroy(&mem->semaforo);
    munmap(mem, sizeof(memoriaCompartida));

    return 0;
}