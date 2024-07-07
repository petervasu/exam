#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main() {
    char *ip = "127.0.0.1";
    int port = 5565;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    FILE *file;
    int read_size;

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
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Bind to the port number: %d\n", port);

    if (listen(server_sock, 5) < 0) {
        perror("Listen error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    addr_size = sizeof(client_addr); 
    if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size)) < 0) {
        perror("Accept error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    memset(buffer, 0, BUFFER_SIZE);
    int filename_size = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    buffer[filename_size] = '\0'; 

    file = fopen(buffer, "rb");
    if (file == NULL) {
        perror("File open failed");
        strcpy(buffer, "file not found");
        send(client_sock, buffer, strlen(buffer), 0);
    } else {
        strcpy(buffer, "file transfer initiated");
        send(client_sock, buffer, strlen(buffer), 0);

        while ((read_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            if (send(client_sock, buffer, read_size, 0) != read_size) {
                perror("Send failed");
                break;
            }
        }

        printf("File sent successfully\n");
        fclose(file);
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
