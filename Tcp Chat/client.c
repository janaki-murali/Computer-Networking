#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 80
#define PORT 9090
#define SA struct sockaddr

void func(int sockfd) {
    char buff[MAX];
    int n;

    for (;;) {
        memset(buff, 0, sizeof(buff));
        printf("Enter the string: ");
        n = 0;

        while ((buff[n++] = getchar()) != '\n' && n < MAX - 1);
        buff[n - 1] = '\0';

        ssize_t bytes_written = write(sockfd, buff, strlen(buff));
        if (bytes_written < 0) {
            perror("Error writing to server");
            break;
        }

        if (strncmp("exit", buff, 4) == 0) {
            printf("Client Exit...\n");
            break;
        }

        memset(buff, 0, sizeof(buff));
        ssize_t bytes_read = read(sockfd, buff, sizeof(buff) - 1);
        if (bytes_read <= 0) {
            printf("Server disconnected or error occurred.\n");
            break;
        }
        buff[bytes_read] = '\0';

        printf("From Server: %s\n", buff);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created...\n");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server...\n");

    func(sockfd);

    close(sockfd);
    printf("Client closed.\n");

    return 0;
}
