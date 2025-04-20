/*Implements leaky bucket algorithm*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep function

void leaky_bucket(int packets[], int n, int bucket_size, int output_rate) {
    int bucket = 0;

    printf("\nTime Step | Incoming | Accepted | Dropped | Bucket Size | Sent | Remaining\n");
    printf("-----------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        int incoming = packets[i];
        int dropped = 0;
        int accepted = incoming;

        // Check for overflow
        if (bucket + incoming > bucket_size) {
            dropped = (bucket + incoming) - bucket_size;
            accepted = incoming - dropped;
            bucket = bucket_size;
        } else {
            bucket += incoming;
        }

        // Send packets at output rate
        int sent = (bucket < output_rate) ? bucket : output_rate;
        bucket -= sent;

        printf("%-9d | %-8d | %-8d | %-7d | %-12d | %-4d | %-8d\n",
               i + 1, incoming, accepted, dropped, bucket + sent, sent, bucket);

        sleep(1); // Simulate time step
    }

    // Drain remaining packets
    int step = n + 1;
    while (bucket > 0) {
        int sent = (bucket < output_rate) ? bucket : output_rate;
        bucket -= sent;
        printf("%-9s | %-8s | %-8s | %-7s | %-12d | %-4d | %-8d\n",
               "Drain", "--", "--", "--", bucket + sent, sent, bucket);
        sleep(1);
        step++;
    }
}

int main() {
    int n, bucket_size, output_rate;

    printf("Enter the number of time steps: ");
    scanf("%d", &n);
    if (n <= 0) {
        printf("Invalid number of time steps.\n");
        return 1;
    }

    printf("Enter the bucket size: ");
    scanf("%d", &bucket_size);
    if (bucket_size <= 0) {
        printf("Bucket size must be positive.\n");
        return 1;
    }

    printf("Enter the output rate: ");
    scanf("%d", &output_rate);
    if (output_rate <= 0) {
        printf("Output rate must be positive.\n");
        return 1;
    }

    int packets[n];
    printf("Enter the incoming packets at each time step:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d", &packets[i]);
        if (packets[i] < 0) {
            printf("Packet count must be non-negative.\n");
            return 1;
        }
    }

    leaky_bucket(packets, n, bucket_size, output_rate);
    return 0;
}
