#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int seq, expected_seq = 0, ack;
    struct timeval timeout = {3, 0}; // 3-second timeout

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        exit(1);
    }

    // Start listening
    if (listen(server_socket, 5) == -1)
    {
        perror("Listening failed");
        exit(1);
    }
    printf("Server is listening on port %d...\n", PORT);

    // Accept client connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1)
    {
        perror("Accept failed");
        exit(1);
    }
    printf("Client connected.\n");

    // Set timeout option for socket
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Process incoming packets
    while (expected_seq < TOTAL_PACKETS)
    {
        if (recv(client_socket, &seq, sizeof(seq), 0) > 0)
        {
            printf("Received packet: %d\n", seq);

            if (seq == expected_seq)
            {
                expected_seq++;
            }
            else
            {
                printf("Out of order. Expected: %d\n", expected_seq);
            }

            // Send ACK
            ack = expected_seq;
            send(client_socket, &ack, sizeof(ack), 0);
            printf("Sent ACK: %d\n", ack);
        }
        else
        {
            printf("Timeout! No packet received. Waiting...\n");
        }
    }

    // Close sockets
    close(client_socket);
    close(server_socket);

    return 0;
}
