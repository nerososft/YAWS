cmake_minimum_required(VERSION 3.14.5)
project(Raft)

set(CMAKE_CXX_STANDARD 14)

SET(Sources
        src/common/Common.cpp
        src/json/Json.cpp
        src/raft/RaftMessage.cpp
        src/raft/RaftMessageImpl.cpp
        src/timer/TimeKeeper.cpp
        src/timer/TimeKeeperImpl.cpp
        src/connect/ConnectionPool.cpp
        src/connect/SocketImpl.cpp
        src/connect/SocketOps.cpp
        src/raft/RaftServerImpl.cpp
        src/raft/RaftServer.cpp
        src/http/HttpMessageImpl.cpp
        src/http/HttpMessage.cpp
        src/templateEngine/Loader.cpp
        src/templateEngine/TemplateEngine.cpp
        src/http/HttpServerImpl.cpp
        src/http/HttpServer.cpp
        src/app/WebApplication.cpp
        src/app/Dashboard.cpp
        src/app/AppConfig.cpp
        src/Bootstrap.cpp
        main.cpp)

set(SRC_LIST ${SEC_LIST} ${Sources})

add_executable(Raft ${SRC_LIST})
