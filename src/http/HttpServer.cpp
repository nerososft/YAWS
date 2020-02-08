//
// Created by XingfengYang on 2020/1/1.
//
#include "../../include/http/HttpServer.h"
#include "../../include/http/HttpServerImpl.h"
#include "../../include/timer/TimeKeeper.h"
#include "../../include/log/Log.h"
#include <memory>

namespace Http {

    HttpServer::~HttpServer() noexcept = default;

    HttpServer::HttpServer(Http::HttpServer &&) noexcept = default;

    HttpServer &HttpServer::operator=(HttpServer &&) noexcept = default;

    HttpServer::HttpServer() :
            httpServer(std::make_shared<HttpServerImpl>()),
            socketOps(std::make_shared<Connect::SocketImpl>()) {

    }

    bool HttpServer::Configure(const Configuration &configuration) {
        this->httpServer->Configure(configuration);
    }

    void HttpServer::SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) {
        this->httpServer->SetSendMessageDelegate(sendMessageDelegate);
    }

    void HttpServer::ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                    unsigned int senderInstanceNumber) {
        this->httpServer->ReceiveMessage(message, senderInstanceNumber);
    }

    void HttpServer::Mobilize() {
        this->httpServer->SetSocketOps(this->socketOps);
        if (this->httpServer->worker.joinable()) {
            return;
        }
        this->httpServer->SetRunning(isRunning);
        this->socketOps->Configure(this->httpServer->configuration.socketConfiguration);
        this->socketOps->SetUp();
        this->socketOps->Bind();
        this->socketOps->Listen();
        LogInfo("[HttpServer] Http Protocol listen at port: %d\n", httpServer->configuration.socketConfiguration.port)
        this->httpServer->SetSocketOps(this->socketOps);

        this->httpServer->stopWorker = std::promise<void>();
        this->httpServer->worker = std::thread(&HttpServerImpl::ServerWorker, httpServer.get());
    }

    void HttpServer::SetTimeKeeper(std::shared_ptr<Timer::TimeKeeper> timeKeeper) {
        this->httpServer->timeKeeper = timeKeeper;
    }

    void HttpServer::AddRoute(Http::Route route, std::function<Http::HandlerResponse(Http::HttpRequest)> handler) {
        this->httpServer->router.insert(std::pair<Route, std::function<Http::HandlerResponse(Http::HttpRequest)>>(route, handler));
    }
}

