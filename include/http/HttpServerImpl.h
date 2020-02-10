//
// Created by XingfengYang on 2020/1/13.
//

#ifndef RAFT_HTTPSERVER_IMPL_H
#define RAFT_HTTPSERVER_IMPL_H

#include <memory>
#include "IHttpServer.h"
#include "HttpMessage.h"
#include "../connect/SocketImpl.h"
#include "../timer/TimeKeeper.h"
#include <pthread/pthread.h>

namespace Http {
    struct Route {
        std::string uri;
        HttpMethod method;

        bool operator<(const Route &rhs) const;

        bool operator>(const Route &rhs) const;

        bool operator<=(const Route &rhs) const;

        bool operator>=(const Route &rhs) const;
    };

    struct HandlerResponse {
        HttpResponseStatus responseStatus;
        std::string body;
    };

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

    public:
        void SetSocketOps(std::shared_ptr<Connect::SocketImpl> socketOps);

        void ServerWorker();

        void SetRunning(bool running);

    public:
        Http::IHttpServer::Configuration configuration;

        std::shared_ptr<Connect::SocketImpl> socket;
        std::thread worker;
        std::promise<void> stopWorker;

        std::shared_ptr<Timer::TimeKeeper> timeKeeper;

        bool isRunning = false;

        std::map<Route, std::function<HandlerResponse(HttpRequest)>> router;

    private:
        void Handler(char *buffer, int fdc);
    };
}

#endif //RAFT_HTTPSERVER_IMPL_H
