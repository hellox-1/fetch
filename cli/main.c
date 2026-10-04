#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "./include/ip.h"

#define PORT 8000
#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    char ip[INET_ADDRSTRLEN];

    if (get_local_ip(ip, sizeof(ip)) != 0)
    {
        printf("Failed to get IP\n");
        return 1;
    }

   // printf("IP: %s\n", ip);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    printf("Connecting to http://%s:%d\n", ip, PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("Connected\n");

    char request[1024];

    snprintf(
        request,
        sizeof(request),
        "GET / HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "Connection: close\r\n"
        "\r\n",
        ip,
        PORT
    );

    send(sock, request, strlen(request), 0);

    char buffer[BUFFER_SIZE];
    int bytes;

    printf("\n--- Response ---\n");

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    if (bytes < 0)
        perror("recv");
    close(sock);

    return 0;
}
