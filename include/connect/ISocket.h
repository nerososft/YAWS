//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_ISOCKET_H
#define RAFT_ISOCKET_H

#include <memory>
#include <functional>
#include <netinet/in.h>
#include "../config/Config.h"

namespace Raft {
    class ISocket {

    public:
        struct SocketConfiguration {
            unsigned int port = 8899;
        };

    public:
        virtual bool Configure(const SocketConfiguration &configuration) = 0;

        virtual int SetUp() = 0;

        virtual int Bind() = 0;

        virtual int Listen() = 0;

        using SocketAcceptEventHandler = std::function<void(char *buffer, int fdc)>;

        virtual int Accept(SocketAcceptEventHandler socketAcceptEventHandler) = 0;

        virtual void SetSocketAcceptEventHandler(SocketAcceptEventHandler socketAcceptEventHandler) = 0;

        virtual int Connect(std::string host, int port) = 0;

        virtual int Send(EndPoint endPoint, char *buf) = 0;

    private:
    };
}

#endif //RAFT_ISOCKET_H
