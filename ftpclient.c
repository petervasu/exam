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

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE];
    int read_size;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }
    printf("TCP client socket created.\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    printf("Enter the filename: ");
    scanf("%s", filename);

    if (send(sock, filename, strlen(filename), 0) < 0) {
        perror("Send filename failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) < 0) {
        perror("Receive error");
        close(sock);
        exit(EXIT_FAILURE);
    }
    buffer[read_size] = '\0';
    printf("Server: %s\n", buffer);

    if (strcmp(buffer, "file transfer initiated") == 0) {
        FILE *file = fopen(filename, "wb");
        if (file == NULL) {
            perror("File open failed");
            close(sock);
            exit(EXIT_FAILURE);
        }

        while ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, read_size, file);
        }

        printf("File received successfully\n");
        fclose(file);
    }

    close(sock);
    return 0;
}
