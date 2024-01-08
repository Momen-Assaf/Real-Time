#include "header.h"

int main(int argc, char *argv[])
{
    int Shelving_Teams, Employees_Number, Shelf_drop_Threshold, Simulation_Time_Threshold = 0;

    // function that reads the userDefined from header.h
    readUserDefined(&Shelving_Teams, &Employees_Number, &Shelf_drop_Threshold, &Simulation_Time_Threshold);

    int num_products = 0;
    ProductInfo product_info[MAX_PRODUCTS];

    read_product_info(product_info, &num_products);
    create_shared_memory(SHM_KEY, product_info, num_products);
    initialize_product_info(product_info, product_info, num_products);


    // ogl fork
    ogl_id = fork();

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

    //shelving teams fork
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
            execlp("./shelvingTeam", "SHELVING TEAM", NULL);
            perror("Error opening shelving team process:");
            exit(EXIT_FAILURE);
        }
    }

    // customers fork
    while (1)
    {
        sleep(2);
        pid_t customer_id = fork();

        if (customer_id < 0)
        {
            perror("Error creating customer process:");
            exit(EXIT_FAILURE);
        }
        else if (customer_id == 0)
        {
            execlp("./customer", "CUSTOMER", NULL);
            perror("Error opening customer process:");
            exit(EXIT_FAILURE);
        }
        break;
    }
    exit(0);
}

void read_product_info(ProductInfo *product_info, int *num_products) {
    FILE *file = fopen("items", "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Skip the first line
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL) {
        perror("Error reading from file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%49s %d", product_info[*num_products].product_name, &product_info[*num_products].total_amount) == 2) {
        (*num_products)++;
    }

    fclose(file);
}

void create_shared_memory(key_t key, ProductInfo *shared_product_info, int num_products) {
    int shmid = shmget(key, num_products * sizeof(ProductInfo), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    ProductInfo *shm_ptr = (ProductInfo *)shmat(shmid, NULL, 0);
    if ((void *)shm_ptr == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Assign shared memory pointer to the array
    memcpy(shared_product_info, shm_ptr, num_products * sizeof(ProductInfo));

    // Detach shared memory
    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void initialize_product_info(ProductInfo *shared_product_info, ProductInfo *product_info, int num_products) {
    for (int i = 0; i < num_products; i++) {
        strncpy(shared_product_info[i].product_name, product_info[i].product_name, sizeof(shared_product_info[i].product_name) - 1);
        shared_product_info[i].product_name[sizeof(shared_product_info[i].product_name) - 1] = '\0';
        shared_product_info[i].total_amount = product_info[i].total_amount;
        shared_product_info[i].on_shelves = 0;
        shared_product_info[i].in_storage = shared_product_info[i].total_amount;
    }
}
