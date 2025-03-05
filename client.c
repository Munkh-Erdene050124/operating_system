#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h> // For inet_pton

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to server. Type '/exit' to quit.\n");

    while (1) {
        // Prompt user for message
        printf("You: ");
        fgets(message, BUFFER_SIZE, stdin);

        // Remove newline character from fgets
        message[strcspn(message, "\n")] = '\0';

        // Check for exit command
        if (strcmp(message, "/exit") == 0) {
            printf("Exiting chat...\n");
            break;
        }

        // Send message to server
        if (send(sock, message, strlen(message), 0) == SOCKET_ERROR) {
            printf("Send failed. Error Code: %d\n", WSAGetLastError());
            break;
        }

        // Receive response from server
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received == SOCKET_ERROR) {
            printf("Receive failed. Error Code: %d\n", WSAGetLastError());
            break;
        } else if (bytes_received == 0) {
            printf("Connection closed by server\n");
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate received data
        printf("Server: %s\n", buffer);
    }

    // Close socket
    closesocket(sock);
    WSACleanup();

    return 0;
}