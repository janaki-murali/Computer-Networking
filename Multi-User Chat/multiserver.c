#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 8080

int clients[MAX_CLIENTS];
int n = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void send_to_all(char *msg, int current) {
    pthread_mutex_lock(&mutex);
    printf("Broadcasting: %s", msg);  // Debug print
    for(int i = 0; i < n; i++) {
        if(clients[i] != current) {
            if(send(clients[i], msg, strlen(msg), 0) < 0) {
                perror("Send failed");
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *arg) {
    int sock = *((int *)arg);
    char msg[BUFFER_SIZE];
    int len;

    // Send welcome message to the client
    char welcome[] = "Welcome to the chat!\n";
    send(sock, welcome, strlen(welcome), 0);

    while((len = recv(sock, msg, BUFFER_SIZE, 0)) > 0) {
        msg[len] = '\0';
        printf("Received message: %s", msg);  // Debug print
        send_to_all(msg, sock);
    }

    pthread_mutex_lock(&mutex);
    // Remove client from array
    for(int i = 0; i < n; i++) {
        if(clients[i] == sock) {
            for(int j = i; j < n-1; j++) {
                clients[j] = clients[j+1];
            }
            break;
        }
    }
    n--;
    pthread_mutex_unlock(&mutex);
    
    close(sock);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    pthread_t thread_id;
    socklen_t addr_size;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Socket created successfully\n");

    // Set socket options to reuse address
    int opt = 1;
    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    printf("Binding successful\n");

    // Listen
    if(listen(server_sock, 10) < 0) {
        perror("Listen failed");
        exit(1);
    }
    printf("Server listening on port %d...\n", PORT);

    addr_size = sizeof(client_addr);
    while(1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if(client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));

        pthread_mutex_lock(&mutex);
        if(n < MAX_CLIENTS) {
            clients[n++] = client_sock;
            pthread_create(&thread_id, NULL, handle_client, &client_sock);
            pthread_detach(thread_id);
        } else {
            printf("Max clients reached. Connection rejected\n");
            close(client_sock);
        }
        pthread_mutex_unlock(&mutex);
    }

    close(server_sock);
    return 0;
}
