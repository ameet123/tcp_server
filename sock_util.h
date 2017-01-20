//
// Created by achaubal on 1/18/17.
//

#ifndef TCP_SERVER_SOCK_UTIL_H
#define TCP_SERVER_SOCK_UTIL_H

int createListeningSocket(int port);

void processDataIn(fd_set *all_inclusive_set, int i);

void acceptConnection(int *sock, fd_set *all_inclusive_set, int *max_fd);

#endif //TCP_SERVER_SOCK_UTIL_H
