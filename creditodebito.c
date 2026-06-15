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



int main()
{
    
}