//
// Created by XingfengYang on 2020/1/13.
//

#ifndef RAFT_HTTPSERVER_H
#define RAFT_HTTPSERVER_H

#include "IHttpServer.h"
#include "HttpServerImpl.h"
#include "HttpMessage.h"
#include "../connect/SocketImpl.h"
#include "../timer/TimeKeeper.h"
#include <memory>


namespace Raft {
    class HttpServer : public IHttpServer {
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

        virtual void ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                    unsigned int senderInstanceNumber) override;

    public:
        void Mobilize();

        void SetTimeKeeper(std::shared_ptr<TimeKeeper> timeKeeper);

        void SetRunning(){this->isRunning = true;}

    private:
        bool isRunning = false;
        std::shared_ptr<HttpServerImpl> httpServer;
        std::shared_ptr<SocketImpl> socketOps;
    };
}

#endif //RAFT_HTTPSERVER_H
