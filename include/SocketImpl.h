//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_SOCKETIMPL_H
#define RAFT_SOCKETIMPL_H

#include "ISocket.h"
#include <memory>
#include <netinet/in.h>

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

        virtual bool Configure(const Configuration &configuration);

        virtual int SetUp();

        virtual int Bind();

        virtual int Listen();

        virtual int Accept(SocketAcceptEventHandler socketAcceptEventHandler);

        virtual void SetSocketAcceptEventHandler(SocketAcceptEventHandler socketAcceptEventHandler);

        struct sockaddr_in addr;


    public:
        int fd = 0;
        int kq = 0;

        Configuration configuration;

        SocketAcceptEventHandler socketAcceptEventHandler;
    };
}
#endif //RAFT_SOCKETIMPL_H
