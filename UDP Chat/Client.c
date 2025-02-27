#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


int main()
{
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addrSize;


    clientSocket = socket(AF_INET, SOCK_DGRAM , 0);
    if(clientSocket < 0)
    {
        perror("Socket Creation Failed\n");
        return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2136);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.36");


    printf("Connected to Server...\n");


    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("Enter a message : ");
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));


        if(strcmp(buffer, "exit") == 0)
        {
            printf("Exiting Client...\n");
            break;
        }


        addrSize = sizeof(serverAddr);
        int recvLen = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&serverAddr, &addrSize);
        if(recvLen > 0)
        {
            buffer[recvLen] = '\0';
            printf("Server : %s\n", buffer);


            if(strcmp(buffer, "exit") == 0)
            {
                printf("Server has exited...Exiting Client...\n");
                break;
            }
        }
        else
        {
            perror("No Response from Server\n");
            printf("Server has exited....Closing the Client...\n");
        }
    }


    close(clientSocket);
    return 0;
}
