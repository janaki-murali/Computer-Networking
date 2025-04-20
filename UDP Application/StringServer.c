/*Alternatively prints characters of string as upper and lower case*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

void alternateCase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (i % 2 == 0)
            str[i] = toupper(str[i]);
        else
            str[i] = tolower(str[i]);
    }
}

int main() {
    int serverSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    // Create UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Setup server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2136);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Changed from 127.0.0.36 (unusual)

    // Bind socket to the IP and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        return -1;
    }

    printf("Server is listening on port 2136...\n");
    addrSize = sizeof(clientAddr);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0,
                               (struct sockaddr*)&clientAddr, &addrSize);

        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            printf("Client: %s\n", buffer);

            if (strcmp(buffer, "exit") == 0) {
                printf("Client exited. Waiting for new messages...\n");
                continue;
            }

            alternateCase(buffer);

            sendto(serverSocket, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&clientAddr, addrSize);

            printf("Modified string sent to client: %s\n", buffer);
        }
    }

    close(serverSocket);
    return 0;
}
