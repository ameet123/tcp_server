//
// Created by achaubal on 1/18/17.
//
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "sock_util.h"
#include "error_codes.h"

/**
 * extract socket creation here
 * @param port
 * @return
 */
int createSocket(int port) {
    int socketOptVal = 1;
    struct sockaddr_in serverAddr;
    // fill in the values in the struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Cannot create socket, dying.");
        exit(SOCK_CREATE_ERROR);
    }
    // apply socket options to reuse address quickly
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &socketOptVal, sizeof(int));

    // bind a socket
    if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding to socket, exiting...");
        exit(BIND_ERROR);
    }
    return sock;
}