#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

int main(int argc, char * argv[]) {
    if (argc != 4) {
        printf("Usage: %s<peer_ip> <peer_port> <message>\n", argv[0]);
        exit(1);
    }

    //get info about our peer
    const char * peer_ip = argv[1];
    int peer_port = atoi(argv[2]);
    const char * message = argv[3];

    struct sockaddr_in peer_addr = {
        .sin_family = AF_INET, // address type
        .sin_port = htons(peer_port) // port num, converted
    };

    if (inet_pton(AF_INET,peer_ip, &(peer_addr.sin_addr)) <= 0) {
        perror("ip error");
        exit(1);
    }

    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("socket create");
        exit(1);
    }
 
    if (sendto(udp_socket, message, strlen(message) + 1, 0, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0) {
        perror("send");
        close(udp_socket);
        exit(1);
    }

    printf("Sent \"%s\" to %s: %d\n", message, peer_ip, peer_port);
    close(udp_socket);
    return 0;
}