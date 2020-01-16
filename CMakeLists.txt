cmake_minimum_required(VERSION 3.14.5)
project(Raft)

set(CMAKE_CXX_STANDARD 14)

set(SRC_LIST ${SEC_LIST} src/RaftMessage.cpp
        src/RaftMessageImpl.cpp
        src/TimeKeeper.cpp
        src/TimeKeeperImpl.cpp
        src/SocketImpl.cpp
        src/SocketOps.cpp
        src/RaftServerImpl.cpp
        src/RaftServer.cpp
        src/HttpMessage.cpp
        src/HttpMessageImpl.cpp
        src/HttpServerImpl.cpp
        src/HttpServer.cpp
        src/Bootstrap.cpp
        main.cpp
        )

add_executable(Raft ${SRC_LIST})
