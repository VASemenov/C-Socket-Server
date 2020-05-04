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
#include "settings.h"
// #include "../sendfile/sendfile2.h"

#define BUFFER_SIZE 8192
#define ERROR_CODE -1
#define BUFFER_SIZE 8192

ssize_t sendfile2(int out_fd, int in_fd, off_t *offset, size_t count);
void sb_sendfile(int client_desc, char* path, char* content_type);

void run_server();
void *socket_thread(void *client_socket);
void check(int operation, char* message);
void print_time();
ssize_t sendfile2(int out_fd, int in_fd, off_t *offset, size_t count);
void sb_sendfile(int client_desc, char* path, char* content_type);

int main()
{
    run_server();

    return 0;
}


void run_server()
{
    /* server and client socket descriptors */
    int server_desc, client_desc;
    int address_len, opt = 1;
    struct sockaddr_in address, client_address;
    pid_t pids[50];
    int pid, i = 0;

    /* Read index.html and save for later */
    // read_html();

    /* Creating server socket descriptor */
    server_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* Establish server socket */
    check(server_desc, "server descriptor");
    check(setsockopt(server_desc, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "socket options");

    /* Setting up address of the server to localhost:8080 */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IP);
    address.sin_port = htons(PORT);
    address_len = sizeof(address);

    check(bind(server_desc, (struct sockaddr *) &address, sizeof(address)), "bind failed");
    check(listen(server_desc, SERVER_BACKLOG), "listen failure");

    while (true)
    {
        /* Creating client socket descriptor */
        client_desc = accept(server_desc, (struct sockaddr *) NULL, NULL);
        check(client_desc, "client descriptor");
        print_time();
        printf("Client %d| Server %d\n", client_desc, server_desc);
        /* Assign client socket desctiptor to pointer
        (to use it in thread) */
        int *client_socket = malloc(sizeof(int) * 2);
        *client_socket = client_desc;

        /* Creating new thread */
        pthread_t pthread_desc;
        pthread_create(&pthread_desc, NULL, socket_thread, client_socket);
    }
}


void *socket_thread(void *client_socket)
{
    int client_desc = *((int *)client_socket);
    free(client_socket);


    char buffer[BUFFER_SIZE] = {'0'};

    /* Get request from client */
    memset(buffer, 0, BUFFER_SIZE);

    check(read(client_desc, buffer, BUFFER_SIZE), "client read");
    printf("\n%s\n\n", buffer);

    if (!strncmp(buffer, "GET /static/styles.css", sizeof("GET /static/styles.css")-1))
        sb_sendfile(client_desc, "./files/static/styles.css", "text/css");
    else
        sb_sendfile(client_desc, "./files/index.html", "text/html");

    close(client_desc);

    return NULL;
}

ssize_t sendfile2(int out_fd, int in_fd, off_t *offset, size_t count)
{
    off_t orig;
    char buf[BUFFER_SIZE];
    size_t toRead, numRead, numSent, totSent;

    if (offset != NULL) {

        /* Save current file offset and set offset to value in '*offset' */
        // get offset inside file from beggining to current position
        orig = lseek(in_fd, 0, SEEK_CUR);

        // If any error in lseek
        if (orig == -1)
            return -1;
        // If any error in lseek with origin set to beginning of a file
        if (lseek(in_fd, *offset, SEEK_SET) == -1)
            return -1;
    }

    totSent = 0;

    while (count > 0) {
        // How many bytes left to read
        toRead = fmin(BUFFER_SIZE, count);

        // Read file with in_fd descriptor into buf
        numRead = read(in_fd, buf, toRead);
        // Handle error if something breaks
        if (numRead == -1)
            return -1;
        // Stop if file is empty
        if (numRead == 0)
            break;                      /* EOF */

        // Send file to output descriptor
        numSent = write(out_fd, buf, numRead);

        // Handle errors
        if (numSent == -1)
            return -1;

        // If for some reason 0 bytes were transferred
        if (numSent == 0)               /* Should never happen */ //<- exactly
            perror("sendfile: write() transferred 0 bytes");

        // decrease count and exit if the whole file is sent
        count -= numSent;
        // just keep track of how much is sent?
        totSent += numSent;
    }

    if (offset != NULL) {

        /* Return updated file offset in '*offset', and reset the file offset
           to the value it had when we were called. */

        *offset = lseek(in_fd, 0, SEEK_CUR);
        if (*offset == -1)
            return -1;
        if (lseek(in_fd, orig, SEEK_SET) == -1)
            return -1;
    }

    return totSent;
}


void sb_sendfile(int client_desc, char* path, char* content_type)
{
    int fd;

    /* Standard header for html file that will be sent to client */
    char* header = malloc(96);
    strcpy(header, "HTTP/1.1 200 OK\r\nContent-Type: ");
    strcat(header, content_type);
    strcat(header, "\r\n\r\n");

    printf("%s", header);

    off_t header_size = sizeof(header);

    printf("Path: $%s\n", path);
    check(fd = open(path, O_RDONLY), path);
    // file_size = lseek(fd, 0, SEEK_END);
    // lseek(fd, 0, SEEK_SET);

    // char *buf = malloc(BUFFER_SIZE);
    // read(fd, buf, BUFFER_SIZE);
    // printf("File:\n%s\n", buf);


    check(write(client_desc, header, strlen(header)), "client write");
    check(sendfile2(client_desc, fd, NULL, BUFFER_SIZE), "sending file");

    close(fd);
    free(header);
}

void check(int operation, char *message)
{
    if (operation == ERROR_CODE)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}


void print_time()
{
    char buffer[26];
    struct tm *tm_info;
    int millisec;
    time_t timer;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec / 1000.0); // Round to nearest millisec
    if (millisec >= 1000) { // Allow for rounding up to nearest second
        millisec -= 1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%H:%M:%S", tm_info);
    printf("%s.%03d | ", buffer, millisec);

}
