#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int serverSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    serverSocket = socket(AF_INET, SOCK_DGRAM , 0);
    if(serverSocket < 0)
    {
        perror("Socket Creation Failed\n");
        return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2136);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.36");

    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Binding Failed\n");
        close(serverSocket);
        return -1;
    }

    printf("Server is listening...\n");
    addrSize = sizeof(clientAddr);

    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        int recvLen = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&clientAddr, &addrSize);
        if(recvLen > 0)
        {
            buffer[recvLen] = '\0';
            printf("Client: %s\n",buffer);
            if(strcmp(buffer,"exit") == 0)
            {
                printf("Client Exited...\n");
                continue;
            }

            memset(buffer, 0, sizeof(buffer));
            printf("Enter a message to client : ");
            fgets(buffer,sizeof(buffer),stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr*) &clientAddr, addrSize);

            if(strcmp(buffer, "exit") == 0)
            {
                printf("Server disconnected\n");
                break;
            }
        }
    }
    close(serverSocket);
    return 0;
}
