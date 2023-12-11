#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/sem.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234 
#define MAX_CUS 3


void displayProductData(char *data)
{
    printf("Product Quantity Price\n");
    printf("%s", data);
}
