//
// Created by XingfengYang on 2020/1/7.
//

#include "../include/SocketOps.h"

namespace Raft {

    SocketOps::~SocketOps() noexcept = default;

    SocketOps::SocketOps(Raft::SocketOps &&) noexcept = default;

    SocketOps &SocketOps::operator=(SocketOps &&) noexcept = default;

    SocketOps::SocketOps() : impl(new SocketImpl()) {

    }

    bool SocketOps::Configure(const SocketConfiguration &configuration) {
        impl->configuration = configuration;
    }

    int SocketOps::SetUp() {
        impl->SetUp();
    }

    int SocketOps::Bind() {
        impl->Bind();
    }

    int SocketOps::Listen() {
        impl->Listen();
    }

    void SocketOps::SetSocketAcceptEventHandler(SocketAcceptEventHandler socketAcceptEventHandler) {
        impl->socketAcceptEventHandler = socketAcceptEventHandler;
    }

    int SocketOps::Accept(SocketAcceptEventHandler socketAcceptEventHandler) {
        impl->Accept(socketAcceptEventHandler);
    }

    int SocketOps::Connect(struct sockaddr_in sockaddrIn) {
        impl->Connect(sockaddrIn);
    }

    int SocketOps::Send(char *buf) {
        impl->Send(buf);
    }
}
