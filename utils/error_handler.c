#include "error_handler.h"

void check(int operation, char *message)
{
    if (operation == ERROR_CODE)
    {
        perror(message);
        // printf("%s\n", message);
        // exit(EXIT_FAILURE);
    }
}

void check_desc(int operation, char *message, int desc)
{
    if (operation == ERROR_CODE)
    {
        perror(message);
        print_time();
        printf("ERROR on desctiptor %d\n", desc);
        // exit(EXIT_FAILURE);
    }
}