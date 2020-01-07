//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_ISOCKET_H
#define RAFT_ISOCKET_H

#include <memory>
#include <functional>

namespace Raft {
    class ISocket {

    public:
        struct Configuration {
            unsigned int port = 8899;
        };

    public:
        virtual bool Configure(const Configuration &configuration) = 0;

        virtual int SetUp() = 0;

        virtual int Bind() = 0;

        virtual int Listen() = 0;

        using EventHandler = std::function<void(char *buffer, unsigned int receivedInstanceNumber)>;

        virtual int Accept(EventHandler eventHandler) = 0;

    private:
    };
}

#endif //RAFT_ISOCKET_H
