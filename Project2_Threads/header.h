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

#define MAX_PRODUCT_NAME_LENGTH 50
#define MAX_PRODUCTS 100
#define SHM_KEY 1111

pid_t ogl_id;

int shmid;
int Shelving_Teams, Shelf_Amount, Employees_Number, Shelf_drop_Threshold, Simulation_Time_Threshold;
