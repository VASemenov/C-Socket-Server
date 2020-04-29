#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define SERVER_BACKLOG 200
#define CLIENT 1
#define SERVER 0
#define ERROR_CODE -1

void run_server();
void read_html(char* response_html);
void *socket_thread(void *client_socket);
void check(int operation, char* message);


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

    /* Creating server socket descriptor */
    server_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* Establish server socket */
    check(server_desc, "server descriptor");
    check(setsockopt(server_desc, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "socket options");

    /* Setting up address of the server to localhost:8080 */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    check(bind(server_desc, (struct sockaddr *) &address, sizeof(address)), "bind failed");
    check(listen(server_desc, SERVER_BACKLOG), "listen failure");

    while (true) 
    {
        address_len = sizeof(address);

        /* Creating client socket descriptor */
        client_desc = accept(server_desc, (struct sockaddr *) &client_address, (socklen_t*) &address_len);
        check(client_desc, "client descriptor");

        /* Assign client socket desctiptor to pointer 
        (to use it in thread) */
        int *client_socket = malloc(sizeof(int) * 2);
        *client_socket = client_desc;

        /* Creating new thread */
        pthread_t pthread_desc;
        pthread_create(&pthread_desc, NULL, socket_thread, client_socket);
    }
}


void *socket_thread(void *client_socket) {
    int client_desc = *((int *)client_socket);
    free(client_socket);
    
    /* Standard header for html file that will be sent to client */
    char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    char buffer[BUFFER_SIZE] = {'0'};
    char response_html[BUFFER_SIZE] = {'0'};

    /* Read index.html and save for later */
    read_html(response_html);

    /* Get request from client */
    memset(buffer, 0, BUFFER_SIZE);

    read(client_desc, buffer, BUFFER_SIZE);
    printf("%s", buffer);

    /* Send data from index.html to client */
    write(client_desc, header, strlen(header));
    write(client_desc, response_html, strlen(response_html));

    close(client_desc);
    
    return NULL;
}


void read_html(char *response_html)
{
    /* Preparing html file as a default response to client */
    FILE* index_html;
    index_html = fopen("./index.html", "r");
    fread(response_html, 1, BUFFER_SIZE, index_html);
    fclose(index_html);
}

void check(int operation, char *message) {
    if (operation == ERROR_CODE) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}
