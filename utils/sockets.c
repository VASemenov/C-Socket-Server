#include "sockets.h"

int tcp_socket(server config) {

    /* server socket descriptor */
    int server_desc;
    int address_len, opt = 1;
    struct sockaddr_in address, client_address;
    pid_t pids[50];
    int pid, i = 0;

    /* Setup error massages */
    // TODO: Move to error handler
    char *desc_error_msg = malloc(64);
    char *opt_error_msg = malloc(64);

    strcat(desc_error_msg, config.name);
    strcat(desc_error_msg, " descriptor");

    strcat(opt_error_msg, config.name);
    strcat(opt_error_msg, " options");

    /* Creating server socket descriptor */
    server_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* Establish server socket */
    check(server_desc, desc_error_msg);
    check(setsockopt(server_desc, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), opt_error_msg);

    /* Setting up address of the server to localhost:8080 */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(config.ip);
    address.sin_port = htons(config.port);

    check(bind(server_desc, (struct sockaddr *) &address, sizeof(address)), "bind failed");
    check(listen(server_desc, config.backlog), "listen failure");

    config.descriptor = server_desc;
    config.address_len = sizeof(address);

    return server_desc;
}
