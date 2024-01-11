#include "header.h"
#include "shm.h"

// Semaphores declarations
sem_t *mutex;   // Protects critical sections
sem_t *buy_sem; // Controls buying process

int main(int argc, char *argv[])
{
    int Shelving_Teams, Shelf_Amount, Employees_Number, Shelf_drop_Threshold, Simulation_Time_Threshold;
    int shmid;
    // function that reads the userDefined
    readUserDefined(&Shelving_Teams, &Shelf_Amount, &Employees_Number, &Shelf_drop_Threshold, &Simulation_Time_Threshold);

    int num_products = 0;
    ProductInfo product_info[MAX_PRODUCTS];
    ProductInfo *shared_product_info;

    // Initialize semaphores
    mutex = sem_open("/mutex_sem", O_CREAT, 0666, 5);
    buy_sem = sem_open("/buy_sem", O_CREAT, 0666, 5);


    read_product_info(product_info, &num_products);
    shmid = create_shared_memory(SHM_KEY,&shared_product_info, num_products);
    initialize_product_info(shared_product_info, product_info, num_products);

    for (int i = 0; i < num_products; i++)
    {
        move_products_to_shelves(shared_product_info, i, Shelf_Amount);
    }

    display_initial_product_info(shared_product_info, num_products);

    // display_updated_product_info(shared_product_info, num_products);

    // ogl fork
    pid_t ogl_id = fork();

    if (ogl_id < 0)
    {
        perror("Error creating ogl process:");
        exit(EXIT_FAILURE);
    }
    else if (ogl_id == 0)
    {
        execlp("./ogl", "OPENGL", NULL);
        perror("Error opening ogl process:");
        exit(EXIT_FAILURE);
    }
    char str_product[20];
    sprintf(str_product, "%d", num_products);

    // shelving teams fork
    pid_t shelvteam_id[Shelving_Teams];

    for (int i = 0; i < Shelving_Teams; i++)
    {
        shelvteam_id[i] = fork();

        if (shelvteam_id[i] < 0)
        {
            perror("Error creating shelving team process:");
            exit(EXIT_FAILURE);
        }
        else if (shelvteam_id[i] == 0)
        {
            execlp("./shelvingTeam", "SHELVING TEAM", str_product, NULL);
            perror("Error opening shelving team process:");
            exit(EXIT_FAILURE);
        }
    }
    int a = 5;
    // customers fork
    while (1)
    {
        sleep(1);

        pid_t customer_id = fork();

        if (customer_id < 0)
        {
            perror("Error creating customer process:");
            exit(EXIT_FAILURE);
        }
        else if (customer_id == 0)
        {
            customer_process(shared_product_info, num_products);
            exit(EXIT_SUCCESS);
        }
        a--;
        if (a == 0)break;
    }
    // Wait for all child processes to finish
    for (int i = 0; i < 10; i++) {
        wait(NULL);
    }
    sem_close(mutex);
    sem_close(buy_sem);
    sem_unlink("/mutex_sem");
    sem_unlink("/buy_sem");
    display_updated_product_info(shared_product_info, num_products);
    detach_from_shared_memory(shared_product_info, shmid);

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

void customer_process(ProductInfo *shared_product_info, int num_products)
{
    int customer_id = getpid();
    srand(customer_id); // Set a different seed for each customer

    sleep(rand() % 5); // Sleep for a random time

    printf("Customer %d says hi!\n", customer_id);

    int product_index = rand() % num_products;
    int amount_to_remove = rand() % shared_product_info[product_index].on_shelves + 1;

    sem_wait(buy_sem); // Wait for permission to buy

    sem_wait(mutex); // Enter critical section

    // Adjust the comment to reflect the action
    printf("Customer %d bought %d units of %s.\n", customer_id, amount_to_remove, shared_product_info[product_index].product_name);

    shared_product_info[product_index].on_shelves -= amount_to_remove;
    shared_product_info[product_index].total_amount -= amount_to_remove;

    sem_post(mutex); // Exit critical section

    sem_post(buy_sem); // Release permission to buy
}