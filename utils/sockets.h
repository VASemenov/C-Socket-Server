#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utils/error_handler.h"

typedef struct server {
    char *name;
    int descriptor;
    char *ip;
    int port;
    int backlog;
    int address_len;
} server;

int tcp_socket(server config);