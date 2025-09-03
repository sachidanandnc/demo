#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 2

int main() {
    int server_fd, client_socks[MAX_CLIENTS], new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set readfds;
    char buffer[1024];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for %d clients...\n", MAX_CLIENTS);

    // Accept two clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_socks[i] = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socks[i] < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Client %d connected!\n", i+1);
    }

    printf("Ready to receive key presses...\n");

    // Game input loop
    while (1) {
        FD_ZERO(&readfds);
        int max_sd = -1;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            FD_SET(client_socks[i], &readfds);
            if (client_socks[i] > max_sd)
                max_sd = client_socks[i];
        }

        // Wait for input from any client
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            break;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(client_socks[i], &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                int valread = read(client_socks[i], buffer, sizeof(buffer)-1);
                if (valread <= 0) {
                    printf("Client %d disconnected.\n", i+1);
                    close(client_socks[i]);
                } else {
                    printf("Client %d pressed: %s\n", i+1, buffer);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}

