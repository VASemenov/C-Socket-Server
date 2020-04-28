#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define CLIENT 1
#define SERVER 0

void run_server();
void read_html();
void check_socket(int socket, int is_client);
void set_socket_options(int socket, int option, int opt);
void bind_socket(int socket, struct sockaddr_in address);
void listen__for_client(int socket);


int main()
{
    run_server();

    return 0;
}


void run_server()
{
    /* server and client socket descriptors */
    int server_desc, client_desc;
    int address_len, opt;

    /* Standard header for html file that will be sent to client */
    char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    char buffer[BUFFER_SIZE] = {0};
    char response_html[BUFFER_SIZE] = {0};
    struct sockaddr_in address;

    /* Read index.html and save for later */
    read_html(response_html);

    opt = 1;

    while (1) {
        /* Creating server socket descriptor */
        server_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        check_socket(server_desc, SERVER);
        set_socket_options(server_desc, SO_REUSEPORT, opt);

        address_len = sizeof(address);

        bind_socket(server_desc, address);
        listen__for_client(server_desc);

        /* Creating client socket descriptor */
        client_desc = accept(server_desc, (struct sockaddr *) &address, (socklen_t*) &address_len);
        close(server_desc);
        check_socket(client_desc, CLIENT);

        /* Creating child process for client socket */
        if (!fork()) {

            /* Get request from client */
            read(client_desc, buffer, BUFFER_SIZE);
            printf("Received: %s", buffer);

            /* Send data from index.html to client */
            write(client_desc, header, strlen(header));
            write(client_desc, response_html, strlen(response_html));

            close(client_desc);
            exit(EXIT_SUCCESS);
        }

        close(client_desc);
    }
}


void read_html(char* response_html)
{
    /* Preparing html file as a default response to client */
    FILE* index_html;
    index_html = fopen("./index.html", "r");
    fread(response_html, 1, BUFFER_SIZE, index_html);
    fclose(index_html);
}


void check_socket(int socket, int is_client) {
    /* Checking if server socket wasn't created */
    if (socket == 0 && is_client == CLIENT)
    {
        perror("client socket");
        exit(EXIT_FAILURE);
    }
    else if (socket == 0 && is_client == SERVER)
    {
        perror("server socket");
        exit(EXIT_FAILURE);
    }
}

void set_socket_options(int socket, int option, int opt) {
    /* Setting socket options to reuse default port */
    if (setsockopt(socket, SOL_SOCKET, option, &opt, sizeof(opt)))
    {
        perror("server socket options");
        exit(EXIT_FAILURE);
    }
}

void bind_socket(int socket, struct sockaddr_in address) {
    /* Setting up address of the server to localhost:8080 */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    /* Binding socket to address */
    if (bind(socket, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void listen__for_client(int socket) {
    /* Listen for client connection */
    if (listen(socket, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}
