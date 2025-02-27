/*Application- To check if number sent by client is prime or not and function accordingly*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define BUFFER_SIZE 1024

void check_prime_composite(int num, char *response)
{
    int prime=1;
    char factors[BUFFER_SIZE] = "";
    int fact_count = 0;
    int i;
    if(num < 2)
    {
	sprintf(response, "%d is neither prime nor composite", num);
	return;
    }

    for(i=1;i<=num;i++)
    {
	if(num % i == 0)
	{
	    prime = 0;
	    char temp[20];
            sprintf(temp, " %d", i);
	    strcat(factors, temp);
	    //strcat(" ",temp);
	    fact_count ++;
	}
    }

    if(fact_count == 2)
    {
	sprintf(response, "%d is a prime number", num);
    }
    else
    {
	sprintf(response, "%d is a composite number and factors are : %s", num, factors);
    }
}

int main()
{
    int serverSocket;
    char buffer[BUFFER_SIZE];
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
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.37");

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

	    int num = atoi(buffer);
	    char response[BUFFER_SIZE];
	    check_prime_composite(num, response);
	    printf("Resposnse from Server : %s\n", response);
	    sendto(serverSocket, response, strlen(response), 0, (struct sockaddr*) &clientAddr, addrSize);
	}
    }
    close(serverSocket);
    return 0;
}
