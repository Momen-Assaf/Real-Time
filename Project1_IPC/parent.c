#include "header.h"

void endRun(int shmid, char *shm, pid_t customer_id[20])
{
    // Detach the shared memory segment
    if (shmdt(shm) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Delete the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 20; i++)
    {
        if (kill(customer_id[i], SIGTERM) == -1)
        {
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int shmid;
    char *shm, *s;
    pid_t customer_id[20], parent_id;

    FILE *items = fopen(argv[1], "r");
    if (items == NULL)
    {
        perror("Error opening Items file");
        return -1;
    }

    FILE *file = fopen("userDefined.txt", "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    // Variables to store the values
    int arrival_rate_min, arrival_rate_max;
    int shopping_time_min, shopping_time_max;
    double scanning_time_min, scanning_time_max;
    int behavior_decay_time, customer_impatience_threshold;
    int cashier_leave_threshold, customer_leave_threshold;
    int income_threshold;

    // Read values from the file
    fscanf(file, "arrival_rate_range: %d, %d\n", &arrival_rate_min, &arrival_rate_max);
    fscanf(file, "shopping_time_range: %d, %d\n", &shopping_time_min, &shopping_time_max);
    fscanf(file, "scanning_time_range: %lf, %lf\n", &scanning_time_min, &scanning_time_max);
    fscanf(file, "behavior_decay_time: %d\n", &behavior_decay_time);
    fscanf(file, "customer_impatience_threshold: %d\n", &customer_impatience_threshold);
    fscanf(file, "cashier_leave_threshold: %d\n", &cashier_leave_threshold);
    fscanf(file, "customer_leave_threshold: %d\n", &customer_leave_threshold);
    fscanf(file, "income_threshold: %d\n", &income_threshold);

    // Skip the first line
    char line[256];
    if (fgets(line, sizeof(line), items) == NULL)
    {
        perror("Error reading file");
        fclose(items);
        return -2;
    }

    // Create shared memory segment
    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget fail");
        exit(-3);
    }

    // Attach shared memory segment to the process address space
    if ((shm = (char *)shmat(shmid, 0, 0)) == (char *)-1)
    {
        perror("shmat: parent");
        exit(-4);
    }

    char fileData[SHM_SIZE];

    // Read and discard the first line
    if (fgets(shm, SHM_SIZE, items) == NULL)
    {
        perror("Error reading file");
        fclose(items);
        endRun(shmid, shm, customer_id);
        return -5;
    }

    // Read and accumulate the rest of the file into shared memory
    while (fgets(shm + strlen(shm), SHM_SIZE - strlen(shm), items) != NULL)
        ;

    printf("Products in Store:\n");
    displayProductData(shm);

    close(items);

    parent_id = getpid();

    for (int i = 0; i < 20; i++)
    {
        srand((unsigned int)getpid());
        customer_id[i] = fork();
        int arrival_time = arrival_rate_min + rand() % (arrival_rate_max - arrival_rate_min + 1);
        sleep(arrival_time);
        switch (customer_id[i])
        {
        case -1:
            perror("fork");
            exit(-6);
            break;

        case 0: // CUSTOMER

            char *token = strtok(shm, "\n");
            char modifiedData[SHM_SIZE];
            int offset = 0;

            while (token != NULL)
            {
                int quantity, price;
                char product[20];
                sscanf(token, "%s %d %d", product, &quantity, &price);
                srand((unsigned int)getpid());
                // Remove random quantity between 0 and 5
                int quantityToRemove = rand() % 4;
                if (quantityToRemove > 0)
                {
                    printf("\nChild removing %d quantity from %s\n", quantityToRemove, product);

                    int quantityAfterRemoval = quantity - quantityToRemove;
                    if (quantityAfterRemoval < 0)
                    {
                        quantityAfterRemoval = 0;
                    }

                    // Update the data in modified buffer
                    offset += snprintf(modifiedData + offset, SHM_SIZE - offset, "%s %d %d\n", product, quantityAfterRemoval, price);

                    // Additional print statement to trace modification
                    printf("Child modified: %s %d %d\n", product, quantityAfterRemoval, price);
                }
                else
                {
                    offset += snprintf(modifiedData + offset, SHM_SIZE - offset, "%s %d %d\n", product, quantity, price);
                }
                token = strtok(NULL, "\n");
            }

            // Copy the modified data back to shared memory
            strcpy(shm, modifiedData);

            // Display product data after child modification
            printf("\nRemaining Products in Store:\n");
            displayProductData(shm);

            return 0;
        }
        printf("iam the parent pid %d created child with %d id",parent_id, customer_id[i]);
        // Display product data after child modification
        printf("\nRemaining Products in Store:\n");
        displayProductData(shm);
    }
    if( parent_id == getpid())endRun(shmid, shm, customer_id);
    
    return 0;
}
