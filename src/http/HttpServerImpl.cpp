//
// Created by XingfengYang on 2020/1/2.
//
#include "../../include/http/IHttpServer.h"
#include "../../include/http/HttpServerImpl.h"
#include "../../include/log/Log.h"
#include "../../include/templateEngine/Loader.h"
#include "../../include/templateEngine/TemplateEngine.h"
#include <unistd.h>

#include <utility>
#include <iostream>
#include <memory>
#include <sstream>

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
        // TODO : may be can use micro to simplify,Like GET("/dashboard",HttpServerImpl::Dashboard)
        router.insert(std::pair<Route, std::function<HandlerResponse(HttpRequest)>>({"/dashboard", GET}, std::bind(&HttpServerImpl::Dashboard, this, std::placeholders::_1)));
    }

    HandlerResponse HttpServerImpl::Dashboard(HttpRequest request) {
        Raft::FileLoader fileLoader;
        Raft::Template::TemplateEngine templateEngine(fileLoader);

        try {
            templateEngine.Load("www/html/dashboard.html");
            templateEngine.SetBlock("configs").Repeat(5);

            for (int i = 0; i < 5; i++) {
                templateEngine.SetBlock("configs")[i].Set("status", "OK");
                templateEngine.SetBlock("configs")[i].Set("role", "Follower");
                templateEngine.SetBlock("configs")[i].Set("id", "Follower");
                templateEngine.SetBlock("configs")[i].Set("host", "Follower");
                templateEngine.SetBlock("configs")[i].Set("port", "Follower");
                templateEngine.SetBlock("configs")[i].Set("term", "Follower");
                templateEngine.SetBlock("configs")[i].Set("commissionId", "Follower");
            }

            std::stringbuf buf;
            std::ostream sout(&buf);
            templateEngine.Render(sout);
            return {OK, buf.str()};
        } catch (std::logic_error error) {
            return {OK, error.what()};
        }
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
