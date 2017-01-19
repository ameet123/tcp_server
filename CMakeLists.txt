cmake_minimum_required(VERSION 3.6)
project(tcp_server)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c sock_util.c sock_util.h error_codes.h)
add_executable(tcp_server ${SOURCE_FILES})