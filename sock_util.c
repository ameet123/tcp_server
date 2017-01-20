//
// Created by achaubal on 1/18/17.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include "sock_util.h"
#include "error_codes.h"
#include "constants.h"

/**
 * create a listening socket, pass the socket fd # or -1 to indicate error
 * @param port
 * @return
 */
int createListeningSocket(int port) {
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
    if (listen(sock, LISTEN_QUEUE) < 0) {
        perror("Error listening on port, exiting");
        exit(LISTEN_ERROR);
    }
    printf("Listening on port->%d\n", port);
    return sock;
}

/**
 * accept new connection and add it to the list of all inclusive currently active descriptors of clients.
 * also update the max file descriptor if appropriate
 * @param sock
 * @param all_inclusive_set
 * @param max_fd
 */
void acceptConnection(int *sock, fd_set *all_inclusive_set, int *max_fd) {
    struct sockaddr_in clientAddr;
    // without this, there is no space allocated to store client address, you will see conn from 0.0.0.0
    socklen_t clientAddr_len = sizeof(clientAddr);

    int client = accept(*sock, (struct sockaddr *) &clientAddr, &clientAddr_len);
    if (client < 0) {
        perror("Accept error");
    } else {
        // add this newly connected client to the list of FDs
        FD_SET(client, all_inclusive_set);
        printf("New Connection accepted from host=[%s] port[%hd]\n", inet_ntoa(clientAddr.sin_addr),
               ntohs(clientAddr.sin_port));
        // also increase the max counter
        if (client > (*max_fd)) {
            (*max_fd) = client;
        }
    }
}

void processDataIn(fd_set *all_inclusive_set, int i) {
    int readBytes = recv(i, sockBuf, BUF_SIZE, 0);
    if (readBytes <= 0) {
        if (readBytes < 0) {
            perror("Error reading from client socket");
        } else if (readBytes == 0) {
            printf("Connection closed\n");
        }
        // close socket and remove from actively monitored list
        close(i);
        FD_CLR(i, all_inclusive_set);
    } else {
        // data received, say something back
        if (send(i, response,strlen(response)* sizeof(char), 0) < 0) {
            perror("Error sending response");
        }
    }
}