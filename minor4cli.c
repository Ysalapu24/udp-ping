/*
 * minor4cli.c - UDP Ping Client
 * CSCE 3600 - Systems Programming
 * University of North Texas
 * Author: Yeshwanth Salapu
 *
 * Sends UDP ping packets to a server and measures RTT.
 * Usage: ./client <server_ip> <port> <num_pings> <timeout_ms>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Returns elapsed milliseconds between two timespec values
double elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0
         + (end.tv_nsec - start.tv_nsec) / 1.0e6;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <num_pings> <timeout_ms>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip  = argv[1];
    int port               = atoi(argv[2]);
    int num_pings          = atoi(argv[3]);
    int timeout_ms         = atoi(argv[4]);

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'\n", server_ip);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Ping Client\n");
    printf("Target: %s:%d | Pings: %d | Timeout: %dms\n\n",
           server_ip, port, num_pings, timeout_ms);

    // Stats
    int   received  = 0;
    double rtt_min  = 1e9;
    double rtt_max  = 0;
    double rtt_sum  = 0;

    char send_buf[BUFFER_SIZE];
    char recv_buf[BUFFER_SIZE];

    for (int seq = 1; seq <= num_pings; seq++) {
        snprintf(send_buf, sizeof(send_buf), "PING %d", seq);

        struct timespec t_send, t_recv;
        clock_gettime(CLOCK_MONOTONIC, &t_send);

        int sent = sendto(sockfd, send_buf, strlen(send_buf), 0,
                          (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        // Wait for reply with timeout
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        struct timeval tv;
        tv.tv_sec  = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        int ready = select(sockfd + 1, &read_fds, NULL, NULL, &tv);

        if (ready < 0) {
            perror("select");
            continue;
        } else if (ready == 0) {
            printf("Ping seq=%d | REQUEST TIMEOUT\n", seq);
            continue;
        }

        // Receive reply
        socklen_t addr_len = sizeof(server_addr);
        int n = recvfrom(sockfd, recv_buf, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&server_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        clock_gettime(CLOCK_MONOTONIC, &t_recv);
        recv_buf[n] = '\0';

        double rtt = elapsed_ms(t_send, t_recv);
        received++;

        if (rtt < rtt_min) rtt_min = rtt;
        if (rtt > rtt_max) rtt_max = rtt;
        rtt_sum += rtt;

        printf("Reply from %s | seq=%d | RTT=%.3fms\n", server_ip, seq, rtt);

        usleep(500000); // 500ms between pings
    }

    // Summary
    int lost = num_pings - received;
    double loss_pct = (lost * 100.0) / num_pings;
    double rtt_avg  = received > 0 ? rtt_sum / received : 0;

    printf("\n--- Ping Statistics ---\n");
    printf("Transmitted: %d | Received: %d | Lost: %d (%.1f%% loss)\n",
           num_pings, received, lost, loss_pct);
    if (received > 0) {
        printf("RTT min/avg/max: %.3f / %.3f / %.3f ms\n",
               rtt_min, rtt_avg, rtt_max);
    }

    close(sockfd);
    return 0;
}
