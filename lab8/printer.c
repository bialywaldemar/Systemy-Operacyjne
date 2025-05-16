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
#define TASKS 10

typedef struct {
    char jobs[TASKS][TEXT_LEN + 1];
    int in;
    int out;
} PrintQueue;

void do_print(char* text, int sec) {
    n = strlen(text);
    for (int i = 0; i < n; i++){
        printf("%c", text[i]);
        sleep(sec);
    }
    printf("\n");
}



int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(PrintQueue));
    PrintQueue* queue = mmap(NULL, sizeof(PrintQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    queue->in = 0;
    queue->out = 0;
}