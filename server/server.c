#include "server.h"

int main()
{
    run_server();

    return 0;
}


void run_server()
{
    /* server and client socket descriptors */
    int server_desc, client_desc;
    server server_socket;
    struct sockaddr_in client_address;

    server_socket.ip = IP;
    server_socket.port = PORT;
    server_socket.backlog = SERVER_BACKLOG;
    server_socket.name = "Server";

    /* Creating server socket descriptor */
    server_desc = tcp_socket(server_socket);

    while (true)
    {
        // TODO: Move to separate function?
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
