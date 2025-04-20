/*Application - Capitalizing every word sent by client*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

void capitalizing(char *str)
{
    int capitalize = 1;
    for(int i=0; str[i]!='\0'; i++)
    {
        if(isspace(str[i]))
        {
            capitalize = 1;
        }
        else if(capitalize && isalpha(str[i]))
        {
            str[i] = toupper(str[i]);
            capitalize = 0;
        }
    }
}

int main()
{
    int server,newSock;
    char buffer[1024];
    struct sockaddr_in servAddr;
    struct sockaddr_storage store;
    socklen_t addrSize;

    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0)
    {
        perror("Socket Failed");
        return -1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(3621);
    servAddr.sin_addr.s_addr = inet_addr("127.0.21");

    if(bind(server,(struct sockaddr *)&servAddr,sizeof(servAddr)) <0)
    {
        perror("Bind Failed");
        return -1;
    }

    if(listen(server,5) == 0)
    {
        printf("Server is listening...\n");
    }
    else
    {
        perror("Listening Failed");
        return -1;
    }

    while(1)
    {
        addrSize = sizeof(store);
        newSock = accept(server,(struct sockaddr*)&store,&addrSize);
        if(newSock < 0)
        {
            perror("Accept Failed\n");
            continue;
        }
        printf("Client Connected\n");

        while(1)
        {
            memset(buffer,0,sizeof(buffer));
            int valread = recv(newSock,buffer,sizeof(buffer)-1,0);
            if(valread > 0)
            {
                buffer[valread]='\0';
                printf("Client : %s\n",buffer);
                if(strcmp(buffer,"exit") == 0)
                {
                    printf("Client exited...Closing Connection...\n");
                    break;
                }

                capitalizing(buffer);
                printf("Capitalized Message : %s\n",buffer);

                send(newSock, buffer, strlen(buffer), 0);
            }
            else if(valread == 0)
            {
                printf("Connected disconnected\n");
                break;
            }
            else
            {
                perror("recv failed");
                break;
            }
        }
        printf("Waiting for new client...\n");
    }
    close(newSock);
    close(server);
    return 0;
}
