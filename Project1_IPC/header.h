#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define SHM_SIZE 1024

void displayProductData(char *data) {
    printf("Product Quantity Price\n");
    printf("%s", data);
}