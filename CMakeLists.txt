cmake_minimum_required(VERSION 3.15)
project(http_client)

set(CMAKE_CXX_STANDARD 14)

add_executable(http_client main.cpp functions.h functions.cpp url.h url.cpp SocketWrapper.cpp SocketWrapper.h HttpClient.cpp HttpClient.h)