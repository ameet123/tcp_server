#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "sock_util.h"
#include "error_codes.h"

#define LISTEN_QUEUE 5
#define LISTEN_PORT 6100
#define TRUE 1


int main() {
    printf("Hello, World!\n");
    // number of file descriptors available with some event
    int i_am_ready_fds;

    // all inclusive fd_set for all file descriptors
    fd_set all_inclusive_set, read_fd_set;

    // for select, we need to know the highest file descriptor to loop over
    int max_fd;

    int sock = createSocket(LISTEN_PORT);

    if (listen(sock, LISTEN_QUEUE) < 0) {
        perror("Error listening on port, exiting");
        exit(LISTEN_ERROR);
    }
    printf("Listening on port->%d\n", LISTEN_PORT);

    // FD SET initialization
    // 1. initialize the all set and add current socket to it.
    FD_ZERO(&all_inclusive_set);
    FD_SET(sock, &all_inclusive_set);

    // this is initialized to the socket and modified as we make connections
    max_fd = sock;

    // define client addr struct
    struct sockaddr_in clientAddr;

    // now start infinite loop
    while (TRUE) {
        // set read fd list to the all inclusive list so we have something to start
        read_fd_set = all_inclusive_set;
        // now we can call select to see if anyone wants to read
        i_am_ready_fds = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);

        // handle various select return scenarios
        // select return-1.) error
        if (i_am_ready_fds < 0) {
            perror("Error in select() call, exiting");
            exit(SELECT_ERROR);
        } else if (i_am_ready_fds == 0) {
            // nothing to read/write/error, just show progress that we are alive
            printf(".");
            fflush(stdout);
        } else {
            // this is the real deal, where kernel is telling us that we got data
            // loop over the fd set, we should have some hit
            for (int i = 0; i < max_fd; ++i) {
                if (FD_ISSET(i, &read_fd_set)) {
                    // something was found at index i
                    if (i == sock) {
                        // this is the listener and we need to handle the connection
                        int client = accept(sock, &clientAddr, sizeof(&clientAddr));
                        if (client < 0) {
                            perror("Accept error");
                            exit(ACCEPT_ERROR);
                        } else {
                            // add this newly connected client to the list of FDs
                            FD_SET(client, &all_inclusive_set);
                            printf("New Connection accepted from host=[%s] port[%hd]\n", clientAddr.sin_addr,
                                   ntohs(clientAddr.sin_port));
                            // also increase the max counter
                            if (client > max_fd) {
                                max_fd = client;
                            }
                        } // new client status loop - END
                    } else {
                        // read data received, process it.

                    } // input on listening socket -> handle new conn - END
                } // some input found by kernel as part of read, don't know what - END
            } // iterating over the list of all inclusive FDs - END
        } // some input found - END


    }


    return 0;
}