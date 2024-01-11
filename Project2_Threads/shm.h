#ifndef SHM_H
#define SHM_H

#include "header.h"

typedef struct
{
    char product_name[50];
    int total_amount;
    int on_shelves;
    int in_storage;
} ProductInfo;

// Function prototypes
void read_product_info(ProductInfo *product_info, int *num_products);
int create_shared_memory(int key, ProductInfo **shared_product_info, int num_products);
void attach_shared_memory(ProductInfo **shared_product_info, int num_products);
void initialize_product_info(ProductInfo *shared_product_info, ProductInfo *product_info, int num_products);
void display_initial_product_info(ProductInfo *shared_product_info, int num_products);
void move_products_to_shelves(ProductInfo *shared_product_info, int index, int amount);
void display_updated_product_info(ProductInfo *shared_product_info, int num_products);
void display_product_info(ProductInfo *products, int num_products);
void detach_from_shared_memory(ProductInfo *shared_product_info, int shmid);

#endif