#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX 1000
#define SA struct sockaddr
#define SAI struct sockaddr_in

int main() {
    int sockfd, connfd, len, n;
    char buffer[MAX];
    SAI serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (SA*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);
    len = sizeof(cli_addr);
    connfd = accept(sockfd, (SA*)&cli_addr, (socklen_t*)&len);

    printf("Server connected.\n");

    bzero(buffer, MAX);
    read(connfd, buffer, MAX);
    int totalMessages = atoi(buffer);
    printf("Expecting %d messages...\n", totalMessages);

    int expectedSeq = 0;
    int received = 0;

    while (received < totalMessages) {
        bzero(buffer, MAX);
        read(connfd, buffer, MAX);

        int msgSeq = buffer[0] - '0'; // First char is seq number
        char* data = buffer + 2;

        printf("Received (Seq %d): %s\n", msgSeq, data);

        if (msgSeq == expectedSeq) {
            // Simulate delay for demonstration
            if (strcmp(data, "delay") == 0) {
                sleep(3); // simulate ACK loss/delay
                printf("Simulated delay... (No ACK sent)\n");
                continue;
            }

            char ackMsg[10];
            sprintf(ackMsg, "ACK %d", expectedSeq);
            write(connfd, ackMsg, sizeof(ackMsg));
            printf("Sent: %s\n", ackMsg);
            expectedSeq = 1 - expectedSeq; // toggle seq
            received++;
        } else {
            // Duplicate message received
            char ackMsg[10];
            sprintf(ackMsg, "ACK %d", 1 - expectedSeq); // resend last ack
            write(connfd, ackMsg, sizeof(ackMsg));
            printf("Duplicate! Resent: %s\n", ackMsg);
        }
    }

    close(sockfd);
    return 0;
}
