#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd; // deskryptor gniazda serwera, deskryptor klienta
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // domain (af_inet oznacza polaczenie internetowe ), type (sock stream albo sock dgram), protocol (protocol zazwyczaj 0)
    if (server_fd == -1) {
        perror("error socket");
        exit(1);
    }

    struct sockaddr_in server_addr =  // struktura zawierajaca sin_family, sin_port;, sin_addr
    {
        .sin_family = AF_INET,
        .sin_port = htons(PORT), // port w sieciowym bajtowym porządku
        .sin_addr.s_addr = INADDR_ANY // każda karta sieciowa
    };

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Serwer rozpoczął nasłuchiwanie\n");

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept");
        exit(1);
    }

    int n = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Odebrano: %s\n", buffer);
        send(client_fd, buffer, n, 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}   