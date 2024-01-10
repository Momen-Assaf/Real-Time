#include "header.h"
#include "shm.h"

int main(int argc, char *argv[])
{
    int Shelving_Teams, Shelf_Amount, Employees_Number, Shelf_drop_Threshold, Simulation_Time_Threshold;
    int shmid;
    // function that reads the userDefined
    readUserDefined(&Shelving_Teams, &Shelf_Amount, &Employees_Number, &Shelf_drop_Threshold, &Simulation_Time_Threshold);

    int num_products = 0;
    ProductInfo product_info[MAX_PRODUCTS];
    ProductInfo *shared_product_info;

    read_product_info(product_info, &num_products);
    shmid = create_shared_memory(&shared_product_info, num_products);
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

    // shelving teams fork
    pid_t shelvteam_id[Shelving_Teams];

    for (int i = 0; i < Shelving_Teams; i++)
    {
        char str_product[20];
        sprintf(str_product, "%d", num_products);
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
    sleep(1);
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