#include <stdio.h>
#include <errno.h>
#include "../utils/logger.h"

#define ERROR_CODE -1

void check(int operation, char *message);
void check_desc(int operation, char *message, int desc);