#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>

#define SERVER_QUEUE "/server_queue"
#define MAX_SIZE 256

int running = 1;

void handle_sigint(int sig){
    running = 0;
    printf("\nZamykanie klienta\n");
}

int main(){
    char client_queue[64];
    char msg[256];
    char response[256];

    sprintf(client_queue, "/client_%d", getpid());
    struct mq_attr attr = {0, 10, 256, 0};

    mqd_t mq = mq_open(client_queue, O_CREAT | O_RDONLY, 0644, &attr);
    if(mq == -1){
        perror("mq_open");
        exit(1);
    }

    mqd_t server_mq = mq_open(SERVER_QUEUE, O_WRONLY);
    if(server_mq == -1){
        perror("server mq_open");
        exit(1);
    }
    sprintf(msg, "INIT:%s", client_queue);
    if(mq_send(server_mq, msg, strlen(msg) + 1, 0) == -1){
        perror("mq_send");
        exit(1);
    }
    printf("Klient %d wyslal %s\n", getpid(), msg);

    mq_receive(mq, response, 256, NULL);
    printf("Klient %d otrzymal %s\n", getpid(), response);
    printf("Aby zakonczyc rozmowe wcisnij CTRL+C\n");

    int id;
    sscanf(response, "ID:%d", &id);
    pid_t pid = fork();
    if(pid > 0) // wysylanie
    {
        while(running){
            memset(msg, 0, MAX_SIZE);
            printf("Podaj komunikat: ");
            if(fgets(msg, MAX_SIZE, stdin) != NULL) {
                msg[strcspn(msg, "\n")] = 0;
                char curr_msg[MAX_SIZE];
                sprintf(curr_msg, "%d:%s", id, msg);
                if(mq_send(server_mq, curr_msg, strlen(curr_msg) + 1, 0) == -1){
                    perror("mq_send komunikat");
                    exit(1);
                }
            }
            else{
                perror("blad komunikat\n");
                exit(1);
            }
        }
    }
    else if (pid == 0) // odczyt 
    {
        signal(SIGINT, handle_sigint);
        while(1){
            char incoming[MAX_SIZE];
            if(mq_receive(mq, incoming, MAX_SIZE, NULL) > 0){
                printf("\nODCZYT: %s\nPodaj komunikat:", incoming);
            }
        }
    }

    else {
        perror("fork");
        exit(1);
    }

    
    mq_close(mq);
    mq_unlink(client_queue);
    mq_close(server_mq);
    exit(0);
    return 0;
}
