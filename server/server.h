#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../utils/sendfile2.h"
#include "../utils/error_handler.h"
#include "../utils/logger.h"

#define IP "0.0.0.0"
#define PORT 3030
#define SERVER_BACKLOG 1200

void run_server();
void *socket_thread(void *client_socket);
