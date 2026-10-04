#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

int get_local_ip(char *ip, size_t size)
{
    char hostname[256];
    struct hostent *host;

    if (gethostname(hostname, sizeof(hostname)) != 0)
        return -1;

    host = gethostbyname(hostname);

    if (host == NULL)
        return -1;

    struct in_addr **addr_list =
        (struct in_addr **)host->h_addr_list;

    if (addr_list[0] == NULL)
        return -1;

    snprintf(ip, size, "%s", inet_ntoa(*addr_list[0]));

    return 0;
}

#endif
