
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_QUEUE "/server_queue"
#define MAX_SIZE 256
#define MAX_CLIENTS 12
#define MAX_NAME 32

typedef struct {
    int id;
    char name[MAX_NAME];
    int active;
    struct sockaddr_in addr;
}ClientInfo;

ClientInfo clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int client_cnt = 0;
int running = 1;

void print_time(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void list() {
    printf("Lista klientów:\n");
    for(int i = 0; i < client_cnt; i++) {
        printf("%d. %s\n", i + 1, clients[i].name);
    }
}

void to_all_string(char* msg, int sender_id, char* sender_name, int server_sock) {
    pthread_mutex_lock(&clients_mutex);
    char time_buf[64];
    print_time(time_buf, sizeof(time_buf));

    char full_msg[MAX_SIZE];
    snprintf(full_msg, sizeof(full_msg), "[%s][%s]: %s\n", time_buf, sender_name, msg);
    for (int i = 0 ; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].id != sender_id) {
            sendto(server_sock, full_msg, strlen(full_msg), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void to_one_string(char* msg, int sender_id, int receiver_id, char* sender_name, int server_sock) {
    pthread_mutex_lock(&clients_mutex);
    char time_buf[64];
    print_time(time_buf, sizeof(time_buf));

    char full_msg[MAX_SIZE];
    snprintf(full_msg, sizeof(full_msg), "\n[%s][%s]: %s\n", time_buf, sender_name, msg);
    for (int i = 0 ; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].id == receiver_id) {
            sendto(server_sock, full_msg, strlen(full_msg), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* client_fun(void* arg) { // funkcja do watku klienta
    int server_sock = *((int*)arg);
    char buffer[MAX_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while(1) {
        memset(buffer, 0, MAX_SIZE);
        int read_size = recvfrom(server_sock, buffer, MAX_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (read_size < 0) {
            continue;
        }
        buffer[read_size] = '\0';

        pthread_mutex_lock(&clients_mutex);
        int id = -1;
        for (int i = 0; i < client_cnt; i++) {
            if (clients[i].active &&
                clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                clients[i].addr.sin_port == client_addr.sin_port) {
                id = i;
                break;
            }
        }

        if (id == -1) {
            id = client_cnt++;
            clients[id].id = id;
            clients[id].addr = client_addr;
            clients[id].active = 1;
            strncpy(clients[id].name, buffer, MAX_NAME); 
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }
        pthread_mutex_unlock(&clients_mutex);

        if (strncmp("LIST", buffer, 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            char list_buf[MAX_SIZE] = "";
            for (int i = 0; i < client_cnt; i++) {
                if (clients[i].active) {
                    char tmp[64];
                    snprintf(tmp, sizeof(tmp), "%d. %s\n", clients[i].id, clients[i].name);
                    strncat(list_buf, tmp, MAX_SIZE - strlen(list_buf) - 1);
                }
            }
            sendto(server_sock, list_buf, strlen(list_buf), 0, (struct sockaddr*)&client_addr, addr_len);
            pthread_mutex_unlock(&clients_mutex);
        }

        else if (strncmp("2ALL", buffer, 4) == 0) {
            to_all_string(buffer + 5, id, clients[id].name, server_sock);
        }

        else if (strncmp("2ONE", buffer, 4) == 0) {
            int target_id;
            char msg[MAX_SIZE];
            sscanf(buffer + 5, "%d %[^\n]", &target_id, msg);
            to_one_string(msg, id, target_id, clients[id].name, server_sock);
        }

        else if (strncmp("STOP", buffer, 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            clients[id].active = 0;
            pthread_mutex_unlock(&clients_mutex);
        }
    }

    return NULL;
}

// void* server_fun(void* arg) { // funkcja do watku serwera
//     while (running) {
//         pthread_mutex_lock(&clients_mutex);
//         for (int i = 0; i < client_cnt; i++) {
//             if (clients[i].active) {
//                 if (send(clients[i].sock, "ALIVE?\n", 6, 0) <= 0) {
//                     clients[i].active = 0;
//                     // close(clients[i].sock);
//                 }
//             }
//         }
//         pthread_mutex_unlock(&clients_mutex);
//         sleep(15);
//     }
//     return NULL;
// }



void handle_sigint (int sig){
    printf("Konczenie dzialania\n");
    mq_unlink(SERVER_QUEUE);
    exit(0);
}

int main(int argc, char* argv[]){
    if (argc != 2) {
        exit(1);
    }
    int port = atoi(argv[1]);

    signal(SIGINT, handle_sigint);
    
    int server_fd;
    server_fd = socket(AF_INET, SOCK_DGRAM, 0); // domain (af_inet oznacza polaczenie internetowe ), type (sock stream albo sock dgram), protocol (protocol zazwyczaj 0)
    if (server_fd == -1) {
        perror("error socket");
        exit(1);
    }

    struct sockaddr_in server_addr =  // struktura zawierajaca sin_family, sin_port;, sin_addr
    {
        .sin_family = AF_INET,
        .sin_port = htons(port), // port w sieciowym bajtowym porządku
        .sin_addr.s_addr = INADDR_ANY // każda karta sieciowa
    };

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // if (listen(server_fd, MAX_CLIENTS) < 0) {
    //     perror("listen");
    //     exit(1);
    // }

    printf("Serwer czeka na porcie %d\n", port);
    // alive ping
    // pthread_t server_tid;
    // pthread_create(&server_tid, NULL, server_fun, NULL);

    // while (running) {
    //     // nowe polaczenia
    //     struct sockaddr_in client_addr;
    //     socklen_t addr_len = sizeof(client_addr);
    //     int client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    //     if (client_sock < 0) {
    //         perror("accept");
    //         continue;
    //     }

    //     pthread_t tid;
    //     int* pclient = malloc(sizeof(int));
    //     *pclient = client_sock;
    //     pthread_create(&tid, NULL, client_fun, pclient);
    //     pthread_detach(tid);
    // }
    pthread_t client_tid;
    pthread_create(&client_tid, NULL, client_fun, &server_fd);
    pthread_join(client_tid, NULL);

    close(server_fd);
    return 0;
}
