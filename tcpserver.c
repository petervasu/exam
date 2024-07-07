#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    char *ip = "127.0.0.1";
    int port = 5566;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }
    printf("TCP server socket created.\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }
    printf("Bind to the port number: %d\n", port);

    if (listen(server_sock, 5) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    while (1) {
        addr_size = sizeof(client_addr); 
        if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size)) < 0) {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }
        printf("Client connected.\n");

        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_sock, buffer, BUFFER_SIZE, 0) < 0) {
            perror("Receive error");
            exit(EXIT_FAILURE);
        }
        printf("Client: %s\n", buffer);

        char *response = "HI, THIS IS SERVER. HAVE A NICE DAY!!!";
        if (send(client_sock, response, strlen(response), 0) < 0) {
            perror("Send error");
            exit(EXIT_FAILURE);
        }
        printf("Server: %s\n", response);

        close(client_sock);
        printf("Client disconnected.\n\n");
    }

    close(server_sock);
    return 0;
}
