#include "header.h"
#include "shm.h"

int Shelving_Teams, Shelf_Amount, Employees_Number, Shelf_drop_Threshold, Simulation_Time_Threshold;
int num_products = 0;

void *teamManagerFunction(ProductInfo **shared_product_info)
{
    // Team manager's code goes here
    printf("Team Manager is managing.\n");
    // display_initial_product_info(shared_product_info, num_products);

    for (int i = 0; i < num_products; i++)
    {
        printf("%-15d\n", shared_product_info[i]->on_shelves);
        if (shared_product_info[i]->on_shelves <= Shelf_drop_Threshold)
        {
        }
    }

    return NULL;
}

void *employeeFunction(void *arg)
{
    // Employee's code goes here
    int employeeID = *((int *)arg);
    printf("Employee %d is working.\n", employeeID);
    return NULL;
}

int main(int argc, char *argv[])
{

    // function that reads the userDefined
    readUserDefined(&Shelving_Teams, &Shelf_Amount, &Employees_Number, &Shelf_drop_Threshold, &Simulation_Time_Threshold);

    ProductInfo product_info[MAX_PRODUCTS];
    ProductInfo *shared_product_info;

    sscanf(argv[1], "%d", &num_products);

    // int shmid = create_shared_memory(BUFF_SHM_KEY,&shared_product_info, num_products);

    attach_shared_memory(&shared_product_info, num_products);


    pthread_t teamManagerThread;
    // Create teamManager thread
    if (pthread_create(&teamManagerThread, NULL, teamManagerFunction, shared_product_info)!= 0)
    {
        perror("Error creating teamManager thread");
        return 1;
    }

    pthread_t employeeThreads[Employees_Number];
    // Create employee threads
    for (int i = 0; i < Employees_Number; i++)
    {
        int *employeeID = malloc(sizeof(int));
        *employeeID = i + 1; // Employee IDs start from 1
        if (pthread_create(&employeeThreads[i], NULL, employeeFunction, (void *)employeeID) != 0)
        {
            perror("Error creating employee thread");
            return 1;
        }
    }

    // Wait for teamManager thread to finish
    if (pthread_join(teamManagerThread, NULL) != 0)
    {
        perror("Error joining teamManager thread");
        return 1;
    }
    // printf("%d",Employees_Number);
    // Wait for employee threads to finish
    for (int i = 0; i < Employees_Number; i++)
    {
        if (pthread_join(employeeThreads[i], NULL) != 0)
        {
            perror("Error joining employee thread");
            return 1;
        }
        free(employeeThreads[i]);
    }

    exit(0);
}

void readUserDefined(int *Shelving_Teams, int *Shelf_Amount, int *Employees_Number, int *Shelf_drop_Threshold, int *Simulation_Time_Threshold)
{
    FILE *file = fopen("userDefined.txt", "r");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }
    fscanf(file, "Shelving_Teams: %d\n", Shelving_Teams);
    fscanf(file, "Shelf_Amount: %d\n", Shelf_Amount);
    fscanf(file, "Employees_Number: %d\n", Employees_Number);
    fscanf(file, "Shelf_drop_Threshold: %d\n", Shelf_drop_Threshold);
    fscanf(file, "Simulation_Time_Threshold: %d\n", Simulation_Time_Threshold);

    fclose(file);
}