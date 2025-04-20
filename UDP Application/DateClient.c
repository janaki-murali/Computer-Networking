/*Print current date and time to the clients*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5566
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr_size = sizeof(server_addr);

    // Get user input
    printf("Enter a message to send to server: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

    // Send input to server
    sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&server_addr, addr_size);

    // Receive response from server
    bzero(buffer, BUFFER_SIZE);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    printf("Server responded with date/time: %s\n", buffer);

    close(sockfd);
    return 0;
}
