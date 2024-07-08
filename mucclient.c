#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5566
#define BUFFER_SIZE 1024

void *receive_messages(void *sock) {
    int client_sock = *(int *)sock;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("%s", buffer);
    }

    if (read_size == 0) {
        printf("Server disconnected\n");
    } else if (read_size == -1) {
        perror("Receive error");
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    pthread_t receive_thread;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    if (pthread_create(&receive_thread, NULL, receive_messages, (void *)&sock) < 0) {
        perror("Thread creation error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Send error");
            break;
        }
    }

    close(sock);
    pthread_cancel(receive_thread);

    return 0;
}
