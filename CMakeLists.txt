cmake_minimum_required(VERSION 3.14.5)
project(Raft)

set(CMAKE_CXX_STANDARD 14)

set(SRC_LIST ${SEC_LIST} src/Message.cpp
        src/RaftMessage.cpp
        src/TimeKeeper.cpp
        src/TimeKeeperImpl.cpp
        src/SocketImpl.cpp
        src/SocketOps.cpp
        src/RaftServerImpl.cpp
        src/RaftServer.cpp
        src/Bootstrap.cpp
        main.cpp
        )

add_executable(Raft ${SRC_LIST})
