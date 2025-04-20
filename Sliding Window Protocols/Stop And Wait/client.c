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
    int sockfd, n;
    char buffer[MAX], recvBuf[MAX];
    SAI serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    connect(sockfd, (SA*)&serv_addr, sizeof(serv_addr));
    printf("Connected to server.\n");

    printf("Enter number of messages: ");
    int total;
    scanf("%d", &total);
    getchar(); // consume newline

    sprintf(buffer, "%d", total);
    write(sockfd, buffer, MAX);

    int seq = 0;
    int sent = 0;

    while (sent < total) {
        bzero(buffer, MAX);
        printf("Enter message %d: ", sent + 1);
        fgets(buffer + 2, MAX - 2, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // remove newline

        buffer[0] = seq + '0'; // add sequence number
        buffer[1] = '|';

        write(sockfd, buffer, MAX);

        // Wait for ACK
        bzero(recvBuf, MAX);
        read(sockfd, recvBuf, MAX);

        if (strncmp(recvBuf, "ACK", 3) == 0 && recvBuf[4] - '0' == seq) {
            printf("Received: %s\n", recvBuf);
            seq = 1 - seq; // toggle sequence
            sent++;
        } else {
            printf("No valid ACK, resending...\n");
            // Don't toggle seq, resend same message
        }
    }

    close(sockfd);
    return 0;
}
