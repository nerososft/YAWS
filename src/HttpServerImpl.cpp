//
// Created by XingfengYang on 2020/1/2.
//
#include "../include/IHttpServer.h"
#include "../include/HttpServerImpl.h"
#include "../include/Log.h"
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

        InitRouter();
    }

    bool HttpServerImpl::Configure(const IHttpServer::Configuration &configuration) {
        this->configuration = configuration;
    }

    void HttpServerImpl::SetSendMessageDelegate(IHttpServer::SendMessageDelegate sendMessageDelegate) {

    }

#define HTTP_RESPONSE_404 "<h1>404 Not Found:</h1><p>What you are looking for is missed.</p>"

    void HttpServerImpl::ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                        unsigned int fdc) {
        std::string uri = message->httpMessage->httpRequestHeader["Path"];
        HttpMethod method = httpMethodMap[message->httpMessage->httpRequestHeader["Type"]];
        LogInfo("[HttpServer] Request %s %s\n", message->httpMessage->httpRequestHeader["Type"].c_str(), message->httpMessage->httpRequestHeader["Path"].c_str())
        Route route{uri, method};
        HandlerResponse responseBody;
        if (router.count(route)) {
            std::function<HandlerResponse(HttpRequest)> &handler = this->router.find(route)->second;
            HttpRequest httpRequest;
            httpRequest.uri = uri;
            httpRequest.httpMethod = method;
            httpRequest.header = message->httpMessage->httpRequestHeader;
//            httpRequest.body = message->httpMessage.body; //TODO : decode body from http request payload
            responseBody = handler(httpRequest);
        } else { // 404 Not Found
            responseBody = {NOT_FOUND, HTTP_RESPONSE_404};
        }

        HttpMessageImpl httpMessage;
        const std::string &response = httpMessage.EncodeMessage(responseBody.responseStatus, responseBody.body);

        write(fdc, response.c_str(), strlen(response.c_str()));
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

    void HttpServerImpl::InitRouter() {
        router.insert(std::pair<Route, std::function<HandlerResponse(HttpRequest)>>({"/dashboard", GET}, std::bind(&HttpServerImpl::Dashboard, this, std::placeholders::_1)));
    }

    HandlerResponse HttpServerImpl::Dashboard(HttpRequest request) {
        // TODO : render response body by template engine
        return {OK, "<h1>Dashboard:</h1>"
                    "<ul>"
                    "<li><a>nodes</a></li>"
                    "<li><a>about</a></li>"
                    "<li><a>config</a></li>"
                    "</ul>"
                    "<table>"
                    "<tr><td>status</td><td>role</td><td>id</td><td>host</td><td>port</td><td>term</td><td>logEntryCommitId</td></tr>"
                    "<tr><td>ok</td><td>follower</td><td>1</td><td>127.0.0.1</td><td>8898</td><td>2</td><td>4</td></tr>"
                    "<tr><td>ok</td><td>follower</td><td>2</td><td>127.0.0.1</td><td>8897</td><td>2</td><td>4</td></tr>"
                    "</table>"};
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
