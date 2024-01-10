#include "header.h"
#include "shm.h"

void read_product_info(ProductInfo *product_info, int *num_products)
{
    FILE *file = fopen("items", "r");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Skip the first line
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL)
    {
        perror("Error reading from file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%49s %d", product_info[*num_products].product_name, &product_info[*num_products].total_amount) == 2)
    {
        (*num_products)++;
    }

    fclose(file);
}

int create_shared_memory(ProductInfo **shared_product_info, int num_products)
{
    int shmid = shmget(SHM_KEY, num_products * sizeof(ProductInfo), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    *shared_product_info = (ProductInfo *)shmat(shmid, NULL, 0);
    if (*shared_product_info == (ProductInfo *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

void attach_shared_memory(ProductInfo **shared_product_info, int num_products)
{
    int shmid = shmget(SHM_KEY, num_products * sizeof(ProductInfo), 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    *shared_product_info = (ProductInfo *)shmat(shmid, NULL, 0);
    if (*shared_product_info == (ProductInfo *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
}

void initialize_product_info(ProductInfo *shared_product_info, ProductInfo *product_info, int num_products)
{
    for (int i = 0; i < num_products; i++)
    {
        strncpy(shared_product_info[i].product_name, product_info[i].product_name, sizeof(shared_product_info[i].product_name) - 1);
        shared_product_info[i].product_name[sizeof(shared_product_info[i].product_name) - 1] = '\0';
        shared_product_info[i].total_amount = product_info[i].total_amount;
        shared_product_info[i].on_shelves = 0;
        shared_product_info[i].in_storage = shared_product_info[i].total_amount;
    }
}

void display_initial_product_info(ProductInfo *shared_product_info, int num_products)
{
    printf("Initial Product Information:\n");
    display_product_info(shared_product_info, num_products);
}

void move_products_to_shelves(ProductInfo *shared_product_info, int index, int amount)
{
    shared_product_info[index].on_shelves += amount;
    shared_product_info[index].in_storage -= amount;
}

void display_updated_product_info(ProductInfo *shared_product_info, int num_products)
{
    printf("\nUpdated Product Information:\n");
    display_product_info(shared_product_info, num_products);
}

void display_product_info(ProductInfo *products, int num_products)
{
    printf("%-20s %-15s %-15s %-15s\n", "Product Name", "Total Amount", "On Shelves", "In Storage");
    for (int i = 0; i < num_products; i++)
    {
        printf("%-20s %-15d %-15d %-15d\n",
               products[i].product_name,
               products[i].total_amount,
               products[i].on_shelves,
               products[i].in_storage);
    }
}

void detach_from_shared_memory(ProductInfo *shared_product_info, int shmid)
{
    if (shmdt(shared_product_info) == -1)
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
}
