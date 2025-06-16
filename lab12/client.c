
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>  
#include <netinet/in.h>  


#define SERVER_QUEUE "/server_queue"
#define MAX_SIZE 256
#define MAX_NAME 32

int sock;
int running = 1;
char client_name[MAX_NAME];
struct sockaddr_in server_addr;

void handle_sigint(int sig){
    running = 0;
    printf("\nZamykanie klienta\n");
    send(sock, "STOP", 4, 0);
    close(sock);
    exit(0);
}

void* receiving(void* arg) {
    char buf[MAX_SIZE];
    while(running) {
        memset(buf, 0, MAX_SIZE);
        int n = recv(sock, buf, MAX_SIZE, 0);
        if (n > 0) {
            printf("%s", buf);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){ // name, port
    if (argc != 3) {
        return 1;
    }
    signal(SIGINT, handle_sigint);
    strncpy(client_name, argv[1], MAX_NAME);

    int port = atoi(argv[2]);
    char msg[256];
    char response[256];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // localhost

    //laczenie z serwerem
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    send(sock, client_name, strlen(client_name), 0);
    
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiving, NULL);
    while (running) {
        printf("> ");
        fflush(stdout);
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0; // usuń newline

        if (strncmp(msg, "STOP", 4) == 0) {
            send(sock, "STOP", 4, 0);
            running = 0;
            break;
        } else {
            send(sock, msg, strlen(msg), 0);
        }
    }

    return 0;
}