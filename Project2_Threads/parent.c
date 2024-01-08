#include "header.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

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

    semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semid == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Set initial values for the semaphores
    union semun arg;
    arg.val = 1;

    if (semctl(semid, 0, SETVAL, arg) == -1)
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    // Create or get shared memory segment for leftCashier
    leftCashier = shmget(LEFT_CASHIER_KEY, sizeof(int), IPC_CREAT | 0666);
    if (leftCashier == -1)
    {
        perror("shmget leftCashier");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory segment for leftCashier
    leftCashierValue = (int *)shmat(leftCashier, NULL, 0);
    if (leftCashierValue == (int *)(-1))
    {
        perror("shmat leftCashier");
        exit(EXIT_FAILURE);
    }

    // Create or get shared memory segment for finishCondition
    finishCondition = shmget(FINISH_CONDITION_KEY, sizeof(int), IPC_CREAT | 0666);
    if (finishCondition == -1)
    {
        perror("shmget finishCondition");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory segment for finishCondition
    finishConditionValue = (int *)shmat(finishCondition, NULL, 0);
    if (finishConditionValue == (int *)(-1))
    {
        perror("shmat finishCondition");
        exit(EXIT_FAILURE);
    }

    // Create or get shared memory segment for finishCondition
    leftCustomer = shmget(LEFT_CUSTOMER_KEY, sizeof(int), IPC_CREAT | 0666);
    if (leftCustomer == -1)
    {
        perror("shmget leftCustomer");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory segment for finishCondition
    leftCustomerValue = (int *)shmat(leftCustomer, NULL, 0);
    if (leftCustomerValue == (int *)(-1))
    {
        perror("shmat leftCustomer");
        exit(EXIT_FAILURE);
    }

    char fileData[SHM_SIZE];

    // Read and discard the first line
    if (fgets(shm, SHM_SIZE, items) == NULL)
    {
        perror("Error reading file");
        fclose(items);
        return -5;
    }

    // Read and accumulate the rest of the file into shared memory
    while (fgets(shm + strlen(shm), SHM_SIZE - strlen(shm), items) != NULL)
        ;

    printf("Products in Store:\n");
    displayProductData(shm);

    close(items);
    ogl_id = fork();
    switch (ogl_id)
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        break;

    case 0:
        execlp("./my_program", "ogl", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    parent_id = getpid();

    struct cashier cashiers[MAX_CASHIER];

    for (int i = 0; i < MAX_CASHIER; i++)
    {
        cashier_id[i] = fork();

        switch (cashier_id[i])
        {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0: // CASHIER
            // execlp("./cashier.c", "cashier", NULL);

            cashiers[i].cashier_id = i + 1;
            srand(time(NULL) + getpid());
            cashiers[i].behavior = rand() % 6 + 51; // Random behavior between
            cashiers[i].totalProfit = 0;

            int msgid;
            struct msg_buffer message;

            msgid = msgget(cashiers[i].cashier_id, 0666 | IPC_CREAT);

            while (1 == 1)
            {
                msgrcv(msgid, &message, sizeof(message), 1, 0);
                // msgsnd(msgid,&empty_message,sizeof(empty_message),0);
                // printf("\nData received is:\n%s\n", message.msg_data);

                // Count the number of lines in the msg
                int numLines = 0;
                for (int i = 0; i < strlen(message.msg_data); i++)
                {
                    if (message.msg_data[i] == '\n')
                    {
                        numLines++;
                    }
                }

                // Allocate memory for the transactions array
                const char *transactions[numLines];

                // Tokenize the msg string and populate the transactions array
                char *msgCopy = strdup(message.msg_data); // Create a copy of msg to avoid modifying the original
                char *token = strtok(msgCopy, "\n");
                int index = 0;
                while (token != NULL)
                {
                    transactions[index++] = token;
                    token = strtok(NULL, "\n");
                }
                double scanning_time = ((double)rand() / RAND_MAX) * (scanning_time_max - scanning_time_min) + scanning_time_min;
                sleep(scanning_time * numLines);
                for (int j = 0; j < sizeof(transactions) / sizeof(transactions[0]); ++j)
                {

                    processTransaction(&cashiers[i], transactions[j], income_threshold, behavior_decay_time);
                }
                cashiers[i].behavior -= behavior_decay_time;
                printf("\nCashier %d with behaviour %d made : %d profit\n", cashiers[i].cashier_id, cashiers[i].behavior, cashiers[i].totalProfit);
            }
            exit(0);
        }
    }

    helper_id = fork();

    switch (helper_id)
    {
    case -1:
        perror("fork");
        exit(-6);
        break;

    case 0:

        for (int i = 0; i < MAX_CUS; i++)
        {
            srand(time(NULL) + getpid());

            int arrival_time = arrival_rate_min + rand() % (arrival_rate_max - arrival_rate_min + 1);
            sleep(arrival_time);
            customer_id[i] = fork();
            switch (customer_id[i])
            {
            case -1:
                perror("fork");
                exit(-6);
                break;

            case 0: // CUSTOMER

                waitSemaphore(semid, 0);

                int msgid[MAX_CASHIER];
                struct msg_buffer message;
                key_t key;

                int random_shopping_time = shopping_time_min + rand() % (shopping_time_max - shopping_time_min + 1);
                sleep(random_shopping_time);

                char *token = strtok(shm, "\n");
                char modifiedData[SHM_SIZE];
                int offset = 0, msg_offset = 0;
                char msg[MSG_SIZE];

                while (token != NULL)
                {
                    int quantity, price;
                    char product[20];
                    sscanf(token, "%s %d %d", product, &quantity, &price);

                    // Remove random quantity between 0 and 2
                    int quantityToRemove = rand() % 3;
                    if (quantityToRemove > 0)
                    {
                        int quantityAfterRemoval = quantity - quantityToRemove;
                        if (quantityAfterRemoval < 0)
                        {
                            quantityToRemove = quantity;
                            quantityAfterRemoval = 0;
                        }

                        // Update the data in modified buffer
                        offset += snprintf(modifiedData + offset, SHM_SIZE - offset, "%s %d %d\n", product, quantityAfterRemoval, price);
                        msg_offset += snprintf(msg + msg_offset, SHM_SIZE - msg_offset, "%s %d %d\n", product, quantityToRemove, price);
                    }
                    else
                    {
                        offset += snprintf(modifiedData + offset, SHM_SIZE - offset, "%s %d %d\n", product, quantity, price);
                        msg_offset += snprintf(msg + msg_offset, SHM_SIZE - msg_offset, "%s 0 %d\n", product, price);
                    }
                    token = strtok(NULL, "\n");
                }

                key_t best;
                int lowest = 999;
                for (int k = 0; k < MAX_CASHIER; k++)
                {
                    struct msqid_ds buf;
                    msgid[k] = msgget(k + 1, 0666);
                    if (msgid[k] == -1)
                        continue;

                    // Get information about the message queue
                    if (msgctl(msgid[k], IPC_STAT, &buf) == -1)
                    {
                        perror("msgctl");
                        exit(EXIT_FAILURE);
                    }
                    if (buf.msg_qnum < lowest)
                    {
                        lowest = buf.msg_qnum;
                        best = msgid[k];
                    }
                }

                printf("\nCustomer %d bought:\n%s", (i + 1), msg);

                message.msg_type = 1;
                strcpy(message.msg_data, msg);
                signal(SIGALRM, alarm_handler);

                alarm(customer_impatience_threshold);
                msgsnd(best, &message, sizeof(message), 0);

                // Copy the modified data back to shared memory
                strcpy(shm, modifiedData);

                FILE *file = fopen("ogl.txt", "w"); // Open a file in write mode

                if (file != NULL)
                {
                    fprintf(file, "%d", best); // Write the integer value to the file
                    fclose(file);              // Close the file
                }
                signalSemaphore(semid, 0);

                exit(0);
            }

            if ((*leftCustomerValue) >= customer_leave_threshold)
            {
                printf("Customer leave threshold reached\n");
                exit(0);
            }
            if ((*leftCashierValue) >= cashier_leave_threshold)
            {
                printf("\nCashier leave threshold reached\n");
                exit(0);
            }

            if ((*finishConditionValue) >= 1)
            {
                printf("\nFinish condition reached\n");
                exit(0);
            }
        }
    }
    waitpid(helper_id, NULL, 0);

    printf("\nRemaining Products in Store:\n");
    displayProductData(shm);

    endRun();

    return 0;
}

// Perform wait operation on the semaphore
void waitSemaphore(int semid, int sem_num)
{
    struct sembuf operation;
    operation.sem_num = sem_num;
    operation.sem_op = -1; // Decrement semaphore value
    operation.sem_flg = 0; // No special flags

    if (semop(semid, &operation, 1) == -1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

// Perform signal operation on the semaphore
void signalSemaphore(int semid, int sem_num)
{
    struct sembuf operation;
    operation.sem_num = sem_num;
    operation.sem_op = 1;  // Increment semaphore value
    operation.sem_flg = 0; // No special flags

    if (semop(semid, &operation, 1) == -1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void processTransaction(struct cashier *cashier, const char *transaction, int income_threshold, int behavior_decay_time)
{
    char product[20];
    int numberOfItems, price;

    // Parse the transaction string
    sscanf(transaction, "%s %d %d", product, &numberOfItems, &price);

    // Skip if numberOfItems is 0
    if (numberOfItems == 0)
    {
        return;
    }
    // Calculate profit after behavior decrease
    int transactionProfit = numberOfItems * price;
    cashier->totalProfit += transactionProfit;

    // // Display transaction details
    // printf("Cashier %d processed %d items of %s for $%d (Profit: $%d, Behavior: %d)\n",
    //        cashier->cashier_id, numberOfItems, product, price, transactionProfit, cashier->behavior);

    // Check if profit goal is reached by any cashier
    if (cashier->totalProfit >= income_threshold)
    {
        printf("Cashier %d has reached the profit goal. Ending Run.\n", cashier->cashier_id);
        // kill(helper_id, SIGTERM);
        (*finishConditionValue)++;
        exit(0); // Terminate the child process
    }

    // Check if behavior is less than or equal to 0
    if (cashier->behavior <= 0)
    {
        printf("Cashier %d has exhausted behavior and is leaving. Total profit: $%d\n",
               cashier->cashier_id, cashier->totalProfit);
        (*leftCashierValue)++;
        exit(0); // Terminate the child process
    }
}

void endRun()
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

    // Detach the shared memory segment for leftCashier
    if (shmdt(leftCashierValue) == -1)
    {
        perror("shmdt leftCashier");
        exit(EXIT_FAILURE);
    }

    // Delete the shared memory segment
    if (shmctl(leftCashier, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    // Detach the shared memory segment for leftCUstomer
    if (shmdt(leftCustomerValue) == -1)
    {
        perror("shmdt finishCondition");
        exit(EXIT_FAILURE);
    }

    // Delete the shared memory segment
    if (shmctl(leftCustomer, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    // Detach the shared memory segment for finishCondition
    if (shmdt(finishConditionValue) == -1)
    {
        perror("shmdt finishCondition");
        exit(EXIT_FAILURE);
    }
    // Delete the shared memory segment
    if (shmctl(finishCondition, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i <= 100; i++) // delete message queue
    {
        if (msgctl(i, IPC_RMID, NULL) == -1)
        {
        }
    }

    // Cleanup semaphore
    if (semctl(semid, 0, IPC_RMID) == -1)
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CASHIER; i++)
    {
        if (kill(cashier_id[i], SIGTERM) == -1)
        {
        }
    }

    for (int i = 0; i < MAX_CUS; i++)
    {
        if (kill(customer_id[i], SIGTERM) == -1)
        {
        }
    }
}