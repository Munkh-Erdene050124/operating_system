#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);
    printf("Waiting for a connection...\n");
    client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    printf("Client connected!\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        printf("Client: %s\n", buffer);
        if (strcmp(buffer, "/exit") == 0) break;
        
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(client_socket, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "/exit") == 0) break;
    }

    closesocket(client_socket);
    closesocket(server_fd);
    WSACleanup();
    printf("Connection closed.\n");
    return 0;
}