#include "logger.h"

void print_time() {
    char buffer[26];
    struct tm* tm_info;
    int millisec;
    time_t timer;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
        millisec -=1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%H:%M:%S", tm_info);
    printf("%s.%03d | ", buffer, millisec);

}
