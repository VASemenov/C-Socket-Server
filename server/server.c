#include "server.h"

void run_server();
void *socket_thread(void *client_socket);
void check(int operation, char* message);
void print_time();

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
