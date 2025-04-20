#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9090
#define SA struct sockaddr

void sendMatrix(int sockfd, int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        write(sockfd, matrix[i], cols * sizeof(int));
    }
}

void receiveMatrix(int sockfd, int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        read(sockfd, matrix[i], cols * sizeof(int));
    }
}

void displayMatrix(int **matrix, int rows, int cols, const char *name) {
    printf("\n%s (%d×%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int **allocateMatrix(int rows, int cols) {
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *)malloc(cols * sizeof(int));
    }
    return matrix;
}

void freeMatrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void func(int sockfd) {
    int r1, c1, r2, c2;

    // Get Matrix A dimensions
    printf("Enter rows and columns for Matrix A: ");
    scanf("%d %d", &r1, &c1);

    int **A = allocateMatrix(r1, c1);
    printf("Enter elements of Matrix A:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%d", &A[i][j]);
        }
    }
    displayMatrix(A, r1, c1, "Matrix A");

    // Send Matrix A dimensions and data
    write(sockfd, &r1, sizeof(int));
    write(sockfd, &c1, sizeof(int));
    sendMatrix(sockfd, A, r1, c1);

    // Get Matrix B dimensions
    printf("Enter rows and columns for Matrix B: ");
    scanf("%d %d", &r2, &c2);

    if (c1 != r2) {
        printf("Matrix multiplication not possible. Exiting...\n");
        freeMatrix(A, r1);
        return;
    }

    int **B = allocateMatrix(r2, c2);
    printf("Enter elements of Matrix B:\n");
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) {
            scanf("%d", &B[i][j]);
        }
    }
    displayMatrix(B, r2, c2, "Matrix B");

    // Send Matrix B dimensions and data
    write(sockfd, &r2, sizeof(int));
    write(sockfd, &c2, sizeof(int));
    sendMatrix(sockfd, B, r2, c2);

    // Receive Transposed Matrix of (A × B)
    int **transposed = allocateMatrix(c2, r1);
    receiveMatrix(sockfd, transposed, c2, r1);
    displayMatrix(transposed, c2, r1, "Received Transposed Matrix");

    // Free allocated memory
    freeMatrix(A, r1);
    freeMatrix(B, r2);
    freeMatrix(transposed, c2);
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
