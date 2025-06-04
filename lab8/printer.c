#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SHM_NAME "/shm1"
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"


#define N 8 // uzytkownicy, zadania
#define M 3 // drukarki
#define S 1
#define TEXT_LEN 10
#define TASKS 1

typedef struct {
    char jobs[TASKS][TEXT_LEN + 1];
    int in;
    int out;
} PrintQueue;

void do_print(char* text, int sec) {
    int n = strlen(text);
    for (int i = 0; i < n; i++){
        printf("%c", text[i]);
        fflush(stdout);
        sleep(sec);
    }
    printf("\n");
}

void cleanup(int sig) {
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    exit(0);
}


int main() {
    signal(SIGINT, cleanup);
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(PrintQueue));
    PrintQueue* queue = mmap(NULL, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    queue->in = 0;
    queue->out = 0;

    sem_t* mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    sem_t* full = sem_open(SEM_FULL, O_CREAT, 0666, TASKS);
    sem_t* empty = sem_open(SEM_EMPTY, O_CREAT, 0666, 0);

    while (1) {
        sem_wait(empty);
        sem_wait(mutex);

        char text[TEXT_LEN + 1];
        strcpy(text, queue->jobs[queue->out]);
        queue->out = (queue->out + 1) % TASKS;

        sem_post(mutex);
        sem_post(full);

        printf("[DRUKARKA %d] Drukowanie zadania: %s\n", getpid(), text);
        do_print(text, 1);
    }
}