//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_SOCKETIMPL_H
#define RAFT_SOCKETIMPL_H

#include "ISocket.h"
#include "ConnectionPool.h"
#include <memory>
#include <netinet/in.h>
#include <sys/event.h>
#include <map>

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

        bool Configure(const SocketConfiguration &config) override;

        int SetUp() override;

        int Bind() override;

        int Listen() override;

        int Accept(SocketAcceptEventHandler acceptEventHandler) override;

        void SetSocketAcceptEventHandler(SocketAcceptEventHandler acceptEventHandler) override;

        int Connect(std::string host, int port) override;

        int Send(EndPoint endPoint, char *buf) override;


    public:
        int fd = 0;
        int kq = 0;

        struct sockaddr_in addr{};

        // TODO : kqueue just work on osx, should replace with epoll on linux
        struct kevent eventSet{};
        struct kevent eventList[32]{};

        SocketConfiguration configuration;

        SocketAcceptEventHandler socketAcceptEventHandler;

    private:
        std::shared_ptr<ConnectionPool> connectionPool;
    };
}
#endif //RAFT_SOCKETIMPL_H
