//
// Created by XingfengYang on 2020/1/2.
//
#include "../include/IHttpServer.h"
#include "../include/HttpServerImpl.h"
#include <unistd.h>

#include <utility>
#include <iostream>
#include <memory>

namespace Raft {


    HttpServerImpl::~HttpServerImpl() noexcept = default;

    HttpServerImpl::HttpServerImpl(Raft::HttpServerImpl &&) noexcept = default;

    HttpServerImpl &HttpServerImpl::operator=(HttpServerImpl &&) noexcept = default;

    HttpServerImpl::HttpServerImpl() {
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("GET", GET));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("HEAD", HEAD));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("POST", POST));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("PUT", PUT));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("DELETE", DELETE));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("CONNECT", CONNECT));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("OPTIONS", OPTIONS));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("TRACE", TRACE));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("PATCH", PATCH));
    }

    bool HttpServerImpl::Configure(const IHttpServer::Configuration &configuration) {
        this->configuration = configuration;
    }

    void HttpServerImpl::SetSendMessageDelegate(IHttpServer::SendMessageDelegate sendMessageDelegate) {

    }

#define HTTP_RESPONSE_404 "HTTP/1.1 404 Not Found\r\nServer: Http \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>404 Not Found:\n</h1>"\
"<p style='color:green;'>What you are looking for is missed.\n</p>"

    void HttpServerImpl::ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                        unsigned int fdc) {
        std::string uri = message->httpMessage->httpRequestHeader["Path"];
        HttpMethod method = httpMethodMap[message->httpMessage->httpRequestHeader["Type"]];
        Route route{uri, method};

        std::string result;
        if (router.count(route)) {
            std::function<std::string(HttpRequest)> &handler = this->router.find(route)->second;
            HttpRequest httpRequest;
            httpRequest.uri = uri;
            httpRequest.httpMethod = method;
            httpRequest.header = message->httpMessage->httpRequestHeader;
//            httpRequest.body = message->httpMessage.body; //TODO : decode body from http request payload
            result = handler(httpRequest);
        } else { // 404 Not Found
            result = HTTP_RESPONSE_404;
        }

        write(fdc, result.c_str(), strlen(result.c_str()));
        close(fdc);
    }

    void HttpServerImpl::Handler(char *buffer, int fdc) {

        auto *httpMessage = new HttpMessageImpl();
        std::shared_ptr<HttpMessage> message = std::make_shared<HttpMessage>();
        message->httpMessage = httpMessage->DecodeMessage(buffer);
        ReceiveMessage(message, fdc);
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


    bool Route::operator<(const Route &rhs) const {
        if (uri < rhs.uri)
            return true;
        if (rhs.uri < uri)
            return false;
        return method < rhs.method;
    }

    bool Route::operator>(const Route &rhs) const {
        return rhs < *this;
    }

    bool Route::operator<=(const Route &rhs) const {
        return !(rhs < *this);
    }

    bool Route::operator>=(const Route &rhs) const {
        return !(*this < rhs);
    }
}
