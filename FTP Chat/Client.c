/*Reverses the input file content and saves it in the output file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Connected to server.\n");

    // Get filename
    char filename[100];
    printf("Enter the name of the file to send: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("File not found");
        return 1;
    }

    // Send file content
    int bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        write(sock, buffer, bytes_read);
    }

    fclose(file);
    close(sock);
    return 0;
}
