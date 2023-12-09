#include "header.h"

void endRun(int shmid, char *shm, pid_t pid)
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
    // Terminate the child process using SIGTERM
    if (kill(pid, SIGTERM) == -1)
    {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    pid_t pid;
    int shmid;
    char *shm,*s;

    FILE *items = fopen(argv[1], "r");
    if (items == NULL)
    {
        perror("Error opening Items file");
        return -1;
    }

    readUserDefined();

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
        endRun(shmid, shm, pid);
        return -5;
    }

    // Read and accumulate the rest of the file into shared memory
    while (fgets(shm + strlen(shm), SHM_SIZE - strlen(shm), items) != NULL);

    printf("Products in Store:\n");
    displayProductData(shm);

    close(items);
    pid = fork();

    switch (pid)
    {
    case -1:
        perror("fork");
        exit(-6);
        break;

    case 0: // CUSTOMER

        srand((unsigned int)getpid());

        char *token = strtok(shm, "\n");
        char modifiedData[SHM_SIZE];
        int offset = 0;

        while (token != NULL)
        {
            int quantity, price;
            char product[20];
            sscanf(token, "%s %d %d", product, &quantity, &price);

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
            else{
                offset += snprintf(modifiedData + offset, SHM_SIZE - offset, "%s %d %d\n", product, quantity, price);
            }
            token = strtok(NULL, "\n");
        }

        // Copy the modified data back to shared memory
        strcpy(shm, modifiedData);
        return 0;

    default:
        sleep(1);

        // Display product data after child modification
        printf("\nRemaining Products in Store:\n");
        displayProductData(shm);

        endRun(shmid, shm, pid);
        exit(0);
    }

    return 0;
}
