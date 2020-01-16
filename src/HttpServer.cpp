//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/HttpServer.h"
#include "../include/HttpServerImpl.h"
#include <memory>

namespace Raft {

    HttpServer::~HttpServer() noexcept = default;

    HttpServer::HttpServer(Raft::HttpServer &&) noexcept = default;

    HttpServer &HttpServer::operator=(HttpServer &&) noexcept = default;

    HttpServer::HttpServer() : impl(new HttpServerImpl()) {

    }

    bool HttpServer::Configure(const Configuration &configuration) {
        this->impl->Configure(configuration);
    }

    void HttpServer::SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) {
        this->impl->SetSendMessageDelegate(sendMessageDelegate);
    }

    void HttpServer::ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                    unsigned int senderInstanceNumber) {
        this->impl->ReceiveMessage(message, senderInstanceNumber);
    }
}

