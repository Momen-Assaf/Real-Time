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

// void readUserDefined()
// {
//     FILE *file = fopen("userDefined.txt", "r");

//     if (file == NULL)
//     {
//         perror("Error opening file");
//         return 1;
//     }
//     // Variables to store the values
//     int arrival_rate_min, arrival_rate_max;
//     int shopping_time_min, shopping_time_max;
//     double scanning_time_min, scanning_time_max;
//     int behavior_decay_time, customer_impatience_threshold;
//     int cashier_leave_threshold, customer_leave_threshold;
//     int income_threshold;

//     // Read values from the file
//     fscanf(file, "arrival_rate_range: %d, %d\n", &arrival_rate_min, &arrival_rate_max);
//     fscanf(file, "shopping_time_range: %d, %d\n", &shopping_time_min, &shopping_time_max);
//     fscanf(file, "scanning_time_range: %lf, %lf\n", &scanning_time_min, &scanning_time_max);
//     fscanf(file, "behavior_decay_time: %d\n", &behavior_decay_time);
//     fscanf(file, "customer_impatience_threshold: %d\n", &customer_impatience_threshold);
//     fscanf(file, "cashier_leave_threshold: %d\n", &cashier_leave_threshold);
//     fscanf(file, "customer_leave_threshold: %d\n", &customer_leave_threshold);
//     fscanf(file, "income_threshold: %d\n", &income_threshold);
// }