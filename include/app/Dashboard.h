//
// Created by XingfengYang on 2020/2/8.
//

#ifndef RAFT_DASHBOARD_H
#define RAFT_DASHBOARD_H

#include <string>

#include "WebApplication.h"
#include "../http/HttpServerImpl.h"
#include "AppConfig.h"

namespace App {
    class Dashboard : public WebApplication {
    public:
        struct NodeModel {
            std::string statusClass;
            std::string status;
            std::string role;
            std::string id;
            std::string host;
            std::string port;
            std::string term;
            std::string commissionId;
        };

        void Init();

    public:
        Http::HandlerResponse Home(Http::HttpRequest request);

        Http::HandlerResponse Config(Http::HttpRequest request);

        Http::HandlerResponse LogEntry(Http::HttpRequest request);

        Http::HandlerResponse About(Http::HttpRequest request);
    };
}
#endif //RAFT_DASHBOARD_H
