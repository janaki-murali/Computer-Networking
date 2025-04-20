#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9090
#define MAX 1024
#define SA struct sockaddr

void receiveMatrix(int connfd, int matrix[MAX][MAX], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        read(connfd, matrix[i], cols * sizeof(int));
    }
}

void sendMatrix(int connfd, int matrix[MAX][MAX], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        write(connfd, matrix[i], cols * sizeof(int));
    }
}

void displayMatrix(int matrix[MAX][MAX], int rows, int cols, const char *name) {
    printf("%s (%d×%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void multiplyMatrices(int A[MAX][MAX], int B[MAX][MAX], int result[MAX][MAX], int r1, int c1, int c2) {
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            result[i][j] = 0;
            for (int k = 0; k < c1; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void transposeMatrix(int matrix[MAX][MAX], int transposed[MAX][MAX], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            transposed[j][i] = matrix[i][j];
        }
    }
}

void func(int connfd) {
    int A[MAX][MAX], B[MAX][MAX], result[MAX][MAX], transposed[MAX][MAX];
    int r1, c1, r2, c2;

    // Receive dimensions of Matrix A
    read(connfd, &r1, sizeof(int));
    read(connfd, &c1, sizeof(int));

    // Receive Matrix A
    receiveMatrix(connfd, A, r1, c1);
    displayMatrix(A, r1, c1, "Received Matrix A");

    // Receive dimensions of Matrix B
    read(connfd, &r2, sizeof(int));
    read(connfd, &c2, sizeof(int));

    // Receive Matrix B
    receiveMatrix(connfd, B, r2, c2);
    displayMatrix(B, r2, c2, "Received Matrix B");

    // Validate matrix multiplication condition
    if (c1 != r2) {
        printf("Matrix multiplication not possible, dimensions mismatch!\n");
        close(connfd);
        return;
    }

    // Perform matrix multiplication
    multiplyMatrices(A, B, result, r1, c1, c2);
    displayMatrix(result, r1, c2, "Multiplication Result");

    // Compute transpose
    transposeMatrix(result, transposed, r1, c2);
    displayMatrix(transposed, c2, r1, "Transpose of the Result");

    // Send the transposed matrix back to client
    sendMatrix(connfd, transposed, c2, r1);
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
