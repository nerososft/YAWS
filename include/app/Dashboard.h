//
// Created by XingfengYang on 2020/2/8.
//

#ifndef RAFT_DASHBOARD_H
#define RAFT_DASHBOARD_H

#include "WebApplication.h"
#include "../http/HttpServerImpl.h"

namespace App {
    class Dashboard : public WebApplication {
    public:
        void Init();
    public:
        Http::HandlerResponse Home(Http::HttpRequest request);
        Http::HandlerResponse About(Http::HttpRequest request);
        Http::HandlerResponse Config(Http::HttpRequest request);
    };
}
#endif //RAFT_DASHBOARD_H
