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