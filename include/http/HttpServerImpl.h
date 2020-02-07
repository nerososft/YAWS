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

namespace Raft {
    enum HttpMethod {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH,
    };

    struct HttpRequest {
        std::string uri;
        HttpMethod httpMethod;
        std::map<std::string, std::string> header;
        std::map<std::string, std::string> params;
        std::string body;
    };

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
        void SetSocketOps(std::shared_ptr<SocketImpl> socketOps);

        void ServerWorker();

        void SetRunning(bool running);

    public:
        Raft::IHttpServer::Configuration configuration;

        std::shared_ptr<SocketImpl> socket;
        std::thread worker;
        std::promise<void> stopWorker;

        std::shared_ptr<TimeKeeper> timeKeeper;

        bool isRunning = false;

    private:
        std::map<std::string, HttpMethod> httpMethodMap;
        std::map<Route, std::function<HandlerResponse(HttpRequest)>> router;

    private:
        void Handler(char *buffer, int fdc);

        void InitRouter();

    private:
        HandlerResponse Dashboard(HttpRequest request);
    };
}

#endif //RAFT_HTTPSERVER_IMPL_H
