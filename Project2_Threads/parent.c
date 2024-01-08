#include "header.h"

int main(int argc, char *argv[])
{
    int Shelving_Teams = 0;
    int Employees_Number = 0;
    int Shelf_drop_Threshold = 0;
    int Simulation_Time_Threshold = 0;
    readUserDefined(&Shelving_Teams, &Employees_Number, &Shelf_drop_Threshold, &Simulation_Time_Threshold);

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