#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

void error_exit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    const char *message = "Hello, server!";
    char buffer[1024];
    ssize_t bytes_received;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_exit("socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    int convertAdress = inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    if (convertAdress <= 0) {
        close(sock);
        error_exit("inet_pton");
    }

    int connection = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (connection == -1) {
        close(sock);
        error_exit("connect");
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);

    if (send(sock, message, strlen(message), 0) == -1) {
        close(sock);
        error_exit("send");
    }
    printf("Sent message: %s\n", message);

    bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    } else if (bytes_received == 0) {
        printf("Server closed connection.\n");
    } else {
        perror("recv");
    }

    // Close the socket
    close(sock);
    printf("Connection closed.\n");

    return 0;
}
