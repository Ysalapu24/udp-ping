/*
 * minor4svr.c - UDP Ping Server
 * CSCE 3600 - Systems Programming
 * University of North Texas
 * Author: Yeshwanth Salapu
 *
 * Simulates packet loss and responds to UDP ping requests.
 * Usage: ./server <port> <loss_rate>
 *   loss_rate: integer 0-100 (percent chance of dropping a packet)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <loss_rate (0-100)>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port      = atoi(argv[1]);
    int loss_rate = atoi(argv[2]);

    if (loss_rate < 0 || loss_rate > 100) {
        fprintf(stderr, "Error: loss_rate must be between 0 and 100\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL));

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Bind to port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Ping Server listening on port %d (loss rate: %d%%)\n", port, loss_rate);
    printf("Waiting for pings...\n\n");

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&client_addr, &client_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }
        buffer[n] = '\0';

        // Simulate packet loss
        int rand_val = rand() % 100;
        if (rand_val < loss_rate) {
            printf("[DROPPED] Packet from %s:%d | Seq: %s\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port),
                   buffer);
            continue;
        }

        // Echo packet back to client
        int sent = sendto(sockfd, buffer, n, 0,
                          (struct sockaddr *)&client_addr, client_len);
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        printf("[REPLIED] Packet from %s:%d | Seq: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);
    }

    close(sockfd);
    return 0;
}
