//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_SOCKETIMPL_H
#define RAFT_SOCKETIMPL_H

#include "ISocket.h"
#include <memory>
#include <netinet/in.h>
#include <sys/event.h>

namespace Raft {
    class SocketImpl : public ISocket {
    public:
        ~SocketImpl() noexcept;

        SocketImpl(const SocketImpl &) = delete;

        SocketImpl(SocketImpl &&) noexcept;

        SocketImpl &operator=(const SocketImpl &) = delete;

        SocketImpl &operator=(SocketImpl &&) noexcept;

    public:
        SocketImpl();

        virtual bool Configure(const Configuration &config);

        virtual int SetUp();

        virtual int Bind();

        virtual int Listen();

        virtual int Accept(SocketAcceptEventHandler acceptEventHandler);

        virtual void SetSocketAcceptEventHandler(SocketAcceptEventHandler acceptEventHandler);


    public:
        int fd = 0;
        int kq = 0;

        struct sockaddr_in addr{};

        struct kevent evSet;
        struct kevent evList[32];

        Configuration configuration;

        SocketAcceptEventHandler socketAcceptEventHandler;
    };
}
#endif //RAFT_SOCKETIMPL_H
