#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    int base = 0, next_seq = 0, ack, i;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        exit(1);
    }
    printf("Connected to server on port %d...\n", PORT);

    srand(time(NULL)); // Initialize random seed

    // Start sending packets
    while (base < TOTAL_PACKETS)
    {
        // Send packets within the window
        for (i = 0; i < WINDOW_SIZE && (base + i) < TOTAL_PACKETS; i++)
        {
            next_seq = base + i;

            // Simulate 25% packet drop
            if (rand() % 4 != 0)
            {
                send(client_socket, &next_seq, sizeof(next_seq), 0);
                printf("Sent packet: %d\n", next_seq);
            }
            else
            {
                printf("Simulated drop: %d\n", next_seq);
            }
        }

        // Wait for ACK
        if (recv(client_socket, &ack, sizeof(ack), 0) > 0)
        {
            printf("Received ACK: %d\n", ack);
            base = ack; // Move the window forward
        }
        else
        {
            printf("Timeout! Resending from base: %d\n", base);
        }
    }

    // Close the socket
    close(client_socket);
    return 0;
}
