#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    char *ip = "127.0.0.1";
    int port = 5566;

    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    printf("Enter a message to be sent to the server:\n");
    fgets(buffer, sizeof(buffer), stdin);

    if (sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in sendto");
        exit(EXIT_FAILURE);
    }
    printf("Client: %s\n", buffer);

    memset(buffer, 0, BUFFER_SIZE);
    addr_size = sizeof(server_addr);
    if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_size) < 0) {
        perror("Receive error");
        exit(EXIT_FAILURE);
    }
    printf("Server: %s\n", buffer);

    close(sockfd);
    return 0;
}
