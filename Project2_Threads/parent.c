#include "header.h"

int main(int argc, char *argv[])
{
    ogl_id = fork();
    
    if(ogl_id < 0){
        perror("Error creating ogl process:");
        exit(EXIT_FAILURE);
        
    }
    else if( ogl_id == 0 ){
        execlp("./ogl.c", "OPENGL" , NULL);
        perror("Error opening ogl process:");
        exit(EXIT_FAILURE);
    }

    for( int i = 0; i < 3; i++){
        shelvteam_id[i] = fork();
    
        if(shelvteam_id[i] < 0){
            perror("Error creating shelving team process:");
            exit(EXIT_FAILURE);
            
        }
        else if( shelvteam_id[i] == 0 ){
            execlp("./shelvingTeam.c", "SHELVING TEAM" , NULL);
            perror("Error opening shelving team process:");
            exit(EXIT_FAILURE);
        }
    }

    while(1){
        sleep(2);
        pid_t customer_id = fork();
    
        if(customer_id < 0){
            perror("Error creating customer process:");
            exit(EXIT_FAILURE);
            
        }
        else if( customer_id == 0 ){
            execlp("./customer.c", "CUSTOMER" , NULL);
            perror("Error opening customer process:");
            exit(EXIT_FAILURE);
        }
        break;
    }
    exit(0);

}