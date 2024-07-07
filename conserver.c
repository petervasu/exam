#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 1024

int main() {
    char *ip = "127.0.0.1";
    int port = 5566;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];
    time_t ct;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }
    printf("UDP socket created.\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }
    printf("Bind to the port number: %d\n", port);
    printf("Listening...\n");

    while(1){
        memset(buffer, 0, BUFFER_SIZE);
        addr_size = sizeof(client_addr);
        if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size) < 0) {
            perror("Error in recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("Client: %s\n", buffer);

        ct = time(NULL);
        sprintf(buffer, "%s", ctime(&ct));
        if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
            perror("Send error");
            exit(EXIT_FAILURE);
        }
        printf("Server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
