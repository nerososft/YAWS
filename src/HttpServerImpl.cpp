//
// Created by XingfengYang on 2020/1/2.
//
#include "../include/IHttpServer.h"
#include "../include/HttpServerImpl.h"
#include <unistd.h>

#include <utility>
#include <iostream>

namespace Raft {


    HttpServerImpl::~HttpServerImpl() noexcept = default;

    HttpServerImpl::HttpServerImpl(Raft::HttpServerImpl &&) noexcept = default;

    HttpServerImpl &HttpServerImpl::operator=(HttpServerImpl &&) noexcept = default;

    HttpServerImpl::HttpServerImpl() {

    }

    bool HttpServerImpl::Configure(const IHttpServer::Configuration &configuration) {
        this->configuration = configuration;
    }

    void HttpServerImpl::SetSendMessageDelegate(IHttpServer::SendMessageDelegate sendMessageDelegate) {

    }

    void HttpServerImpl::ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                        unsigned int senderInstanceNumber) {

    }

#define TEST_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Http \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>HTTP Server Status:\n</h1>"\
"<p style='color:green;'>HTTP Server works.\n</p>"

    void HttpServerImpl::Handler(char *buffer, int fdc) {
        char *buf = TEST_HTTP_RESPONSE;
        auto *httpMessage = new HttpMessageImpl();
        httpMessage->DecodeMessage(buffer);
        write(fdc, buf, strlen(buf));
        close(fdc);
    }

    void HttpServerImpl::ServerWorker() {
        while (isRunning) {
            const auto bind = std::bind(&HttpServerImpl::Handler, this, std::placeholders::_1, std::placeholders::_2);
            this->socket->Accept(bind);
        }
    }

    void HttpServerImpl::SetSocketOps(std::shared_ptr<SocketImpl> socketOps) {
        this->socket = std::move(socketOps);
    }

    void HttpServerImpl::SetRunning(bool running) {
        this->isRunning = running;
    }
}
