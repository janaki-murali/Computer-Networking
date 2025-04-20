#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9009
#define TOTAL_PACKETS 9

int isCorrupted() {
    return rand() % 4 == 0; // 25% chance to simulate corruption
}

int main() {
    srand(time(NULL));
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Connected to Selective Repeat Server...\n");

    int received[TOTAL_PACKETS] = {0}, count = 0;

    while (count < TOTAL_PACKETS) {
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n > 0) {
            int pkt_num = buffer[strlen(buffer) - 1] - '0';
            printf("Received: %s\n", buffer);

            if (!received[pkt_num]) {
                if (isCorrupted()) {
                    printf("Packet %d corrupted! Not sending ACK.\n", pkt_num);
                } else {
                    printf("Sending ACK for Packet %d\n", pkt_num);
                    sprintf(buffer, "ACK%d", pkt_num);
                    send(client_socket, buffer, sizeof(buffer), 0);
                    received[pkt_num] = 1;
                    count++;
                }
            }
        }
    }

    close(client_socket);
    printf("All packets received correctly. Closing client.\n");
    return 0;
}
