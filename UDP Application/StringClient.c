/*Alternatively prints characters of string as upper and lower case*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addrSize;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2136);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Match with server

    printf("Connected to server...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf("Enter a sentence: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

        // Send message to server
        sendto(clientSocket, buffer, strlen(buffer), 0,
               (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting client...\n");
            break;
        }

        addrSize = sizeof(serverAddr);
        int recvLen = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0,
                               (struct sockaddr*)&serverAddr, &addrSize);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            printf("Modified sentence from server: %s\n", buffer);
        } else {
            perror("No response from server");
            printf("Server might have exited. Closing the client...\n");
            break;
        }
    }

    close(clientSocket);
    return 0;
}
