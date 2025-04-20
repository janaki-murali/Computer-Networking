#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define MAX 80
#define PORT 9090
#define SA struct sockaddr

void func(int connfd) {
    char buff[MAX];
    int n;

    for (;;) {
        bzero(buff, MAX);
        ssize_t bytes_read = read(connfd, buff, sizeof(buff) - 1);

        if (bytes_read <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }

        buff[bytes_read] = '\0';
        printf("From client: %s\n", buff);

        if (strncmp("exit", buff, 4) == 0) {
            printf("Client requested to exit. Closing connection...\n");
            break;
        }

        printf("To client: ");
        memset(buff, 0, MAX);
        n = 0;
        while ((buff[n++] = getchar()) != '\n' && n < MAX - 1);
        buff[n - 1] = '\0';

        ssize_t bytes_written = write(connfd, buff, strlen(buff));
        if (bytes_written < 0) {
            perror("Error writing to client");
            break;
        }
    }
}

int main() {
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created...\n");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully binded...\n");

    if (listen(sockfd, 5) != 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        perror("Server accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Server accepted the client...\n");

    func(connfd);

    close(connfd);
    close(sockfd);
    printf("Server closed.\n");

    return 0;
}
