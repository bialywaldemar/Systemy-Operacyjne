
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SERVER_QUEUE "/server_queue"
#define MAX_SIZE 256
#define MAX_CLIENTS 12

typedef struct {
    int sock;
    char name[64];
    int active
}ClientInfo;

ClientInfo clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int client_cnt = 0;

void list() {
    printf("Lista klientów:\n");
    for(int i = 0; i < client_cnt; i++) {
        printf("%d. %s", i + 1, clients[i].name);
    }
}

void to_all_string(char* msg, int id) {

}


void handle_sigint (int sig){
    printf("Konczenie dzialania\n");
    mq_unlink(SERVER_QUEUE);
    exit(0);
}

int main(){
    signal(SIGINT, handle_sigint);
    mqd_t mq;
    char buffer[MAX_SIZE];

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0644, &attr);
    if(mq == -1){
        perror("mq_open (serwer)");
        exit(1);
    }

    printf("Serwer czeka na INIT\n");
    while(1){
        memset(buffer, 0, MAX_SIZE);
    
        if(mq_receive(mq, buffer, MAX_SIZE, NULL) == -1) {
            perror("mq_receive");
            exit(1);
        }

        if(strncmp(buffer, "INIT:", 5) == 0) // nowy klient
        {
            if(client_cnt >= MAX_CLIENTS) {
                printf("Jest maksymalna liczba klientów\n");
                continue;
            }
            char *client_queue_name = buffer + 5;
            printf("\nOtrzymano INIT od %s\n", client_queue_name);

            mqd_t client_mq = mq_open(client_queue_name, O_WRONLY);
            if (client_mq == -1) {
                perror("mq_open(cl queue)");
                continue;
            }

            clients[client_cnt].id = client_cnt;
            strncpy(clients[client_cnt].queue_name, client_queue_name, sizeof(clients[client_cnt].queue_name));
            clients[client_cnt].mq = client_mq;

            char response[MAX_SIZE];
            sprintf(response, "ID:%d", client_cnt++);
            mq_send(client_mq, response, strlen(response) + 1, 0);
            printf("Wyslano %s do %s", response, client_queue_name);
        }
        else // wiadomosc
        {
            int sender_id;
            char msg[MAX_SIZE];
            if(sscanf(buffer, "%d:%[^\n]", &sender_id, msg) == 2) {
                printf("\nOtrzymano wiadomosc od klienta %d: %s\n", sender_id, msg);
                char curr_msg[MAX_SIZE];
                sprintf(curr_msg, "Klient %d: %s", sender_id, msg);
                for (int i = 0; i < client_cnt; i++){
                    if(clients[i].id != sender_id) {
                        if (mq_send(clients[i].mq, curr_msg, strlen(curr_msg) + 1, 0) == -1) {
                            perror("mq_send do klientow");
                        }
                    }
                }
            }
        }
    }
    mq_close(mq);
    return 0;
}

