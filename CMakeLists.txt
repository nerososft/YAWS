cmake_minimum_required(VERSION 3.15)
project(Raft)

set(CMAKE_CXX_STANDARD 14)

set(SRC_LIST ${SEC_LIST} src/Message.cpp src/Server.cpp main.cpp)


add_executable(Raft ${SRC_LIST})
