#include "header.h"

void kill_sharedmem(int shmid ,char *shm){
     // Detach the shared memory segment
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Delete the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}
int main() {
    pid_t pid;
    int shmid;
    char *shm, *s;

    FILE *items = fopen("items","r");
    if(items == NULL ){
        perror("Error opening Items file");
        return -1;
    }

    // Skip the first line
    char line[256];
    if (fgets(line, sizeof(line), items) == NULL) {
        perror("Error reading file");
        fclose(items);
        return 1;
    }

    // Create shared memory segment
    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget fail");
        exit(3);
    }

    // Attach shared memory segment to the process address space
    if ((shm = (char *)shmat(shmid, 0, 0)) == (char *)-1) {
        perror("shmat: parent");
        exit(4);
    }

    char fileData[SHM_SIZE];

    // Read and discard the first line
    if (fgets(shm, SHM_SIZE, items) == NULL) {
        perror("Error reading file");
        fclose(items);
        kill_sharedmem(shmid,shm);
        return 1;
    }

    // Read and accumulate the rest of the file into shared memory
    while (fgets(shm + strlen(shm), SHM_SIZE - strlen(shm), items) != NULL);

    printf("All Data:\n%s", shm);

    close(items);

    sleep(1);

    kill_sharedmem(shmid,shm);

    return 0;
}
