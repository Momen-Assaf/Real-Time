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
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/msg.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234 
#define MAX_CUS 3
#define MAX_CASHIER 2
#define MSG_SIZE 1024

struct cashier {
    int cashier_id;
    int behavior;
    int totalProfit;
};

struct msg_buffer {
    long msg_type;
    char msg_data[MSG_SIZE];
};


void displayProductData(char *data)
{
    printf("Product Quantity Price\n");
    printf("%s", data);
}
