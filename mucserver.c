#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 5566
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(char *message, int exclude_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (client_sockets[i] != exclude_sock) {
            if (send(client_sockets[i], message, strlen(message), 0) < 0) {
                perror("Send failed");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        broadcast_message(buffer, client_sock);
    }

    close(client_sock);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (client_sockets[i] == client_sock) {
            client_sockets[i] = client_sockets[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    free(arg);
    pthread_exit(NULL);
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size))) {
        pthread_mutex_lock(&clients_mutex);
        if (client_count >= MAX_CLIENTS) {
            printf("Max clients reached. Rejecting client...\n");
            close(client_sock);
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }

        client_sockets[client_count++] = client_sock;
        pthread_mutex_unlock(&clients_mutex);

        new_sock = malloc(1);
        *new_sock = client_sock;
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Thread creation error");
            free(new_sock);
            continue;
        }
        pthread_detach(client_thread);
    }

    if (client_sock < 0) {
        perror("Accept error");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    return 0;
}