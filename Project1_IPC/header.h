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
#include <semaphore.h>

#define SHM_SIZE 4096
#define SEM_KEY 1234
#define MAX_CUS 50
#define MAX_CASHIER 3
#define MSG_SIZE 4096
#define FLAG_KEY 23

#define LEFT_CASHIER_KEY 1234
#define LEFT_CUSTOMER_KEY 4321
#define FINISH_CONDITION_KEY 5678

pid_t customer_id[MAX_CUS], cashier_id[MAX_CASHIER], parent_id, helper_id;
int shmid, semid;
char *shm;

int leftCashier, finishCondition, leftCustomer;
int *leftCashierValue;
int *finishConditionValue;
int *leftCustomerValue;

struct cashier
{
    int cashier_id;
    int behavior;
    int totalProfit;
};

struct msg_buffer
{
    long msg_type;
    char msg_data[MSG_SIZE];
};

union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

void alarm_handler(int signum)
{
    (*leftCustomerValue)++;
    printf("\n A Customer left the store\n");
    exit(0);
}

void displayProductData(char *data)
{
    printf("Product Quantity Price\n");
    printf("%s", data);
}
