//
// Created by XingfengYang on 2020/1/2.
//
#include "../include/IHttpServer.h"
#include "../include/HttpServerImpl.h"
#include <unistd.h>

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

    void handlerHttp(char *buffer, int fdc) {

        char *buf = TEST_HTTP_RESPONSE;
        write(fdc, buf, strlen(buf));
        close(fdc);
    }

    void HttpServerImpl::ServerWorker() {
        while (isRunning) {
            this->socket->Accept(handlerHttp);
        }
    }

    void HttpServerImpl::SetSocketOps(std::shared_ptr<SocketImpl> socket) {
        this->socket = socket;
    }

    void HttpServerImpl::SetRunning(bool running) {
        this->isRunning = running;
    }
}
