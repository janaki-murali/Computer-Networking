/*Print current date and time to the clients*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5566
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    printf("UDP Server is running on port %d...\n", PORT);

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        addr_size = sizeof(client_addr);

        // Receive message from client
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
        printf("[Client %s:%d] Sent: %s\n", 
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        // Get current date and time
        FILE *fp = popen("date", "r");
        if (!fp) {
            perror("Date command failed");
            continue;
        }

        fgets(buffer, BUFFER_SIZE, fp);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline
        pclose(fp);

        // Send date/time to client
        sendto(sockfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_addr, addr_size);
        printf("Replied to client [%s:%d] with date: %s\n", 
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    }

    close(sockfd);
    return 0;
}
