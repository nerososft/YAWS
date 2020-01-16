//
// Created by XingfengYang on 2020/1/13.
//

#ifndef RAFT_HTTPSERVER_IMPL_H
#define RAFT_HTTPSERVER_IMPL_H

#include <memory>
#include "IHttpServer.h"
#include "HttpMessage.h"

namespace Raft {
    class HttpServerImpl {
    public:
        ~HttpServerImpl() noexcept;

        HttpServerImpl(const HttpServerImpl &) = delete;

        HttpServerImpl(HttpServerImpl &&) noexcept;

        HttpServerImpl &operator=(const HttpServerImpl &) = delete;

        HttpServerImpl &operator=(HttpServerImpl &&) noexcept;

    public:
        HttpServerImpl();

    public:
        bool Configure(const IHttpServer::Configuration &configuration);

        void SetSendMessageDelegate(IHttpServer::SendMessageDelegate sendMessageDelegate);

        void ReceiveMessage(std::shared_ptr<HttpMessage> message,
                            unsigned int senderInstanceNumber);
    };
}

#endif //RAFT_HTTPSERVER_IMPL_H
