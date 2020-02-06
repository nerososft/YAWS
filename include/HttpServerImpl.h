//
// Created by XingfengYang on 2020/1/13.
//

#ifndef RAFT_HTTPSERVER_IMPL_H
#define RAFT_HTTPSERVER_IMPL_H

#include <memory>
#include "IHttpServer.h"
#include "HttpMessage.h"
#include "SocketImpl.h"
#include "TimeKeeper.h"
#include <pthread/pthread.h>

namespace Raft {
    enum HttpResponseStatus {
        CONTINUE = 100,
        SWITCHING_PROTOCOLS = 101,
        PROCESSING = 102,
        CHECKPOINT = 103,
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NON_AUTHORITATIVE_INFORMATION = 203,
        NO_CONTENT = 204,
        RESET_CONTENT = 205,
        PARTIAL_CONTENT = 206,
        MULTI_STATUS = 207,
        ALREADY_REPORTED = 208,
        IM_USED = 226,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        TEMPORARY_REDIRECT = 307,
        PERMANENT_REDIRECT = 308,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        PAYMENT_REQUIRED = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PROXY_AUTHENTICATION_REQUIRED = 407,
        REQUEST_TIMEOUT = 408,
        CONFLICT = 409,
        GONE = 410,
        LENGTH_REQUIRED = 411,
        PRECONDITION_FAILED = 412,
        PAYLOAD_TOO_LARGE = 413,
        URI_TOO_LONG = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        REQUESTED_RANGE_NOT_SATISFIABLE = 416,
        EXPECTATION_FAILED = 417,
        I_AM_A_TEAPOT = 418,
        UNPROCESSABLE_ENTITY = 422,
        LOCKED = 423,
        FAILED_DEPENDENCY = 424,
        UPGRADE_REQUIRED = 426,
        PRECONDITION_REQUIRED = 428,
        TOO_MANY_REQUESTS = 429,
        REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
        UNAVAILABLE_FOR_LEGAL_REASONS = 451,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505,
        VARIANT_ALSO_NEGOTIATES = 506,
        INSUFFICIENT_STORAGE = 507,
        LOOP_DETECTED = 508,
        BANDWIDTH_LIMIT_EXCEEDED = 509,
        NOT_EXTENDED = 510,
        NETWORK_AUTHENTICATION_REQUIRED = 511,
    };

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
        std::map<unsigned int, std::string> reasonMap;
        std::map<Route, std::function<std::string(HttpRequest)>> router;

    private:
        void Handler(char *buffer, int fdc);
    };
}

#endif //RAFT_HTTPSERVER_IMPL_H
