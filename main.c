#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "sock_util.h"
#include "error_codes.h"

#define LISTEN_PORT 6100
#define TRUE 1

int main() {
    printf("Hello, World!\n");
    // number of file descriptors available with some event
    int fds_with_input;
    // all inclusive fd_set for all file descriptors
    fd_set all_inclusive_set, read_fd_set;

//    char response[] = "Hello! I received some data";

    int sock = createListeningSocket(LISTEN_PORT);
    /**
     * BEGIN this section is for select() call to organize the descriptors to store and loop over
     */
    // for select, we need to know the highest file descriptor to loop over
    int max_fd;
    // FD SET initialization
    // 1. initialize the all set and add current socket to it.
    FD_ZERO(&all_inclusive_set);
    FD_ZERO(&read_fd_set);
    FD_SET(sock, &all_inclusive_set);
    // this is initialized to the socket and modified as we make connections
    max_fd = sock;
    /**
     * END of select() call descriptor section
     */
    // now start infinite loop
    while (TRUE) {
        // set read fd list to the all inclusive list so we have something to start
        read_fd_set = all_inclusive_set;
        // now we can call select to see if anyone wants to read
        fds_with_input = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);

        // handle various select return scenarios
        // select return-1.) error
        if (fds_with_input < 0) {
            perror("Error in select() call, exiting");
            exit(SELECT_ERROR);
        } else if (fds_with_input == 0) {
            // nothing to read/write/error, just show progress that we are alive
            printf(".");
            fflush(stdout);
        } else {
            // this is the real deal, where kernel is telling us that we got data
            // loop over the fd set, we should have some hit
            // Note: read_fd_set has been modified
            for (int i = 0; i <= max_fd; ++i) {
                if (FD_ISSET(i, &read_fd_set)) { // does "i" socket have activity indicated by inclusion in read_fd_set?
                    // something was found at index i
                    if (i == sock) {
                        acceptConnection(&sock, &all_inclusive_set, &max_fd);
                    } else {
                        processDataIn(&all_inclusive_set, i);
                    } // input on listening socket -> handle new conn - END
                } // some input found by kernel as part of read, don't know what - END
            } // iterating over the list of all inclusive FDs - END
        } // some input found - END
    }
    return 0;
}