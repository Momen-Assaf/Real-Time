#include "header.h"

int main() {
    printf("Hello, World!\n");

    FILE *items = fopen("items","r");
    if(items == NULL ){
        perror("Error opening Items file");
        return -1;
    }
    fclose(items);

    return 0;
}
