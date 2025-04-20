/*Reverses the input file content and saves it in the output file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void reverse(char *str, int len) {
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server is waiting for a connection...\n");
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    printf("Client connected.\n");

    // Receive file content
    FILE *temp = fopen("received.txt", "w");
    int bytes_read;
    while ((bytes_read = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, sizeof(char), bytes_read, temp);
        if (bytes_read < BUFFER_SIZE) break; // Assuming end of file
    }
    fclose(temp);

    // Read entire file content
    temp = fopen("received.txt", "r");
    fseek(temp, 0, SEEK_END);
    long size = ftell(temp);
    rewind(temp);

    char *content = (char *)malloc(size + 1);
    fread(content, sizeof(char), size, temp);
    content[size] = '\0';
    fclose(temp);

    // Reverse content
    reverse(content, strlen(content));
    printf("Reversed Content:\n%s\n", content);

    // Ask output file name
    char output_file[100];
    printf("Enter output file name to save reversed content: ");
    scanf("%s", output_file);

    FILE *out = fopen(output_file, "w");
    fprintf(out, "%s", content);
    fclose(out);
    free(content);

    close(new_socket);
    close(server_fd);

    return 0;
}
