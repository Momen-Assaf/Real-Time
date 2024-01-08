#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>

pid_t ogl_id;

void readUserDefined(int *Shelving_Teams, int *Employees_Number, int *Shelf_drop_Threshold, int *Simulation_Time_Threshold)
{
    FILE *file = fopen("userDefined.txt", "r");
    if (file == NULL)
    {
        printf("Error opening the file.\n");
        return;
    }
    fscanf(file, "Shelving_Teams: %d\n", Shelving_Teams);
    fscanf(file, "Employees_Number: %d\n", Employees_Number);
    fscanf(file, "Shelf_drop_Threshold: %d\n", Shelf_drop_Threshold);
    fscanf(file, "Simulation_Time_Threshold: %d\n", Simulation_Time_Threshold);

    fclose(file);
}