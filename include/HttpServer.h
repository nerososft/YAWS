//
// Created by XingfengYang on 2020/1/13.
//

#ifndef RAFT_HTTPSERVER_H
#define RAFT_HTTPSERVER_H

#include <memory>
#include "IServer.h"

namespace Raft {
    class HttpServer : public IServer {
    public:
        ~HttpServer() noexcept;

        HttpServer(const HttpServer &) = delete;

        HttpServer(HttpServer &&) noexcept;

        HttpServer &operator=(const HttpServer &) = delete;

        HttpServer &operator=(HttpServer &&) noexcept;

    public:
        HttpServer();

    public:
        virtual bool Configure(const Configuration &configuration) override;

        virtual void SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) override;

        virtual void ReceiveMessage(std::shared_ptr<Message> message,
                                    unsigned int senderInstanceNumber) override;
    };
}

#endif //RAFT_HTTPSERVER_H
