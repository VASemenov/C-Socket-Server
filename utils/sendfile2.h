#include <sys/types.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include "../utils/error_handler.h"

#define BUFFER_SIZE 8192

ssize_t sendfile2(int out_fd, int in_fd, off_t *offset, size_t count);
void sb_sendfile(int client_desc, char* path, char* content_type);

