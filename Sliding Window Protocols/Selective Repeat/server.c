#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 9009
#define TOTAL_PACKETS 9
#define WINDOW_SIZE 3

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int sent[TOTAL_PACKETS] = {0}, ack;
    char buffer[1024];
    struct timeval timeout = {2, 0}; // 2 second timeout

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Selective Repeat Server running...\nWaiting for client...\n");

    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    int base = 0;

    while (base < TOTAL_PACKETS) {
        for (int i = 0; i < WINDOW_SIZE && (base + i) < TOTAL_PACKETS; i++) {
            int seq = base + i;
            if (!sent[seq]) {
                sprintf(buffer, "Packet %d", seq);
                send(client_socket, buffer, sizeof(buffer), 0);
                printf("Sent: %s\n", buffer);
            }
        }

        for (int i = 0; i < WINDOW_SIZE && (base + i) < TOTAL_PACKETS; i++) {
            int seq = base + i;
            if (!sent[seq]) {
                int n = recv(client_socket, buffer, sizeof(buffer), 0);
                if (n > 0) {
                    if (strncmp(buffer, "ACK", 3) == 0) {
                        int ack_num = buffer[3] - '0';
                        sent[ack_num] = 1;
                        printf("Received ACK for Packet %d\n", ack_num);
                    } else {
                        printf("Corrupted ACK for Packet %d. Resending...\n", seq);
                    }
                } else {
                    printf("Timeout on Packet %d. Resending...\n", seq);
                }
            }
        }

        while (sent[base] && base < TOTAL_PACKETS) {
            base++;
        }
    }

    close(client_socket);
    close(server_socket);
    printf("All packets acknowledged. Closing server.\n");

    return 0;
}
