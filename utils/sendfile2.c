#include "sendfile2.h"

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
