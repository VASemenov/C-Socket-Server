#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../sendfile/sendfile2.h"
#include "../error_handler/error_handler.h"

#define IP "0.0.0.0"
#define PORT 3030
#define SERVER_BACKLOG 1200

// TODO: Move to config
