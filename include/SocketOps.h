//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_SOCKETOPS_H
#define RAFT_SOCKETOPS_H

#include "ISocket.h"
#include "SocketImpl.h"
#include <memory>
#include <netinet/in.h>

namespace Raft {
    class SocketOps : public ISocket {
    public:
        ~SocketOps() noexcept;

        SocketOps(const SocketOps &) = delete;

        SocketOps(SocketOps &&) noexcept;

        SocketOps &operator=(const SocketOps &) = delete;

        SocketOps &operator=(SocketOps &&) noexcept;

    public:
        SocketOps();

        virtual bool Configure(const Configuration &configuration) override;

        virtual int SetUp() override;

        virtual int Bind() override;

        virtual int Listen() override;

        virtual int Accept(SocketAcceptEventHandler socketAcceptEventHandler) override;

        virtual void SetSocketAcceptEventHandler(SocketAcceptEventHandler socketAcceptEventHandler) override;



    private:
        std::shared_ptr<SocketImpl> impl;
    };
}
#endif //RAFT_SOCKETOPS_H
