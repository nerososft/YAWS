//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/HttpServer.h"
#include "../include/HttpServerImpl.h"
#include "../include/TimeKeeper.h"
#include <memory>

namespace Raft {

    HttpServer::~HttpServer() noexcept = default;

    HttpServer::HttpServer(Raft::HttpServer &&) noexcept = default;

    HttpServer &HttpServer::operator=(HttpServer &&) noexcept = default;

    HttpServer::HttpServer() :
            httpServer(std::make_shared<HttpServerImpl>()),
            socketOps(std::make_shared<SocketImpl>()) {

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
        this->httpServer->SetSocketOps(this->socketOps);

        this->httpServer->stopWorker = std::promise<void>();
        this->httpServer->worker = std::thread(&HttpServerImpl::ServerWorker, httpServer.get());
    }

    void HttpServer::SetTimeKeeper(std::shared_ptr<TimeKeeper> timeKeeper) {
        this->httpServer->timeKeeper = timeKeeper;
    }
}

