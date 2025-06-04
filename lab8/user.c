#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define SHM_NAME "/shm1"
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"

#define TEXT_LEN 10
#define N 8 // uzytkownicy, zadania
#define M 3 // drukarki
#define S 1
#define TASKS 1

typedef struct {
    char jobs[TASKS][TEXT_LEN + 1];
    int in;
    int out;
} PrintQueue;

char* random_chars(){
    char* res = malloc(TEXT_LEN + 1);
    if(res == NULL){
        return NULL;
    }
    for(int i = 0; i < TEXT_LEN; i++){
        res[i] = 'a' + rand() % 26;
    }
    res[TEXT_LEN] = '\0';
    return res;
}

int main() {
    srand(getpid());
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    PrintQueue* queue = mmap(NULL, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t* mutex = sem_open(SEM_MUTEX, 0);
    sem_t* full = sem_open(SEM_FULL, 0);
    sem_t* empty = sem_open(SEM_EMPTY, 0);

    while(1){
        char* text = random_chars();
        sem_wait(full); // full - 1
        sem_wait(mutex);
        strcpy(queue->jobs[queue->in], text);
        queue->in = (queue->in + 1) % TASKS;
        sem_post(mutex);
        sem_post(empty); // +1
        printf("[USER %d] Wyslano zadanie: %s\n", getpid(), text);
        sleep(5 + rand() % 6);
    }
    return 0;
}