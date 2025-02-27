#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void *receive_messages(void *sock_fd) {
    int sockfd = *((int *)sock_fd);
    char message[BUFFER_SIZE];
    int len;

    while((len = recv(sockfd, message, BUFFER_SIZE-1, 0)) > 0) {
        message[len] = '\0';
        printf("%s", message);
        fflush(stdout);
    }
    
    if(len == 0) {
        printf("\nServer disconnected.\n");
    } else if(len < 0) {
        perror("Receive failed");
    }
    
    close(sockfd);
    exit(1);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    pthread_t recv_thread;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Socket created successfully\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    printf("Connected to server\n");

    // Create thread to receive messages
    if(pthread_create(&recv_thread, NULL, receive_messages, &sockfd) < 0) {
        perror("Thread creation failed");
        exit(1);
    }

    // Get username
    char username[50];
    printf("Enter your username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Start chatting (type 'quit' to exit):\n");

    while(1) {
        fgets(message, BUFFER_SIZE, stdin);
        
        if(strncmp(message, "quit", 4) == 0) {
            break;
        }

        size_t max_user_len = BUFFER_SIZE / 2 - 3; // Reserve space for ": " and null terminator
        size_t max_msg_len = BUFFER_SIZE - max_user_len - 3; // Ensure total stays within bounds

        char formatted_msg[BUFFER_SIZE];
        snprintf(formatted_msg, BUFFER_SIZE, "%.*s: %.*s",
        (int)max_user_len, username, 
        (int)max_msg_len, message);

        // Send message
        if(send(sockfd, formatted_msg, strlen(formatted_msg), 0) < 0) {
            perror("Send failed");
            break;
        }
    }

    close(sockfd);
    return 0;
}
