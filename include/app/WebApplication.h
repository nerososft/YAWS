//
// Created by XingfengYang on 2020/2/8.
//

#ifndef RAFT_WEBAPPLICATION_H
#define RAFT_WEBAPPLICATION_H

#include <map>
#include "../http/HttpServerImpl.h";
#include "../http/HttpServer.h"

namespace App {
    class WebApplication {
    public:
        virtual void Init();

        void SetServer(std::shared_ptr<Http::HttpServer> server);

    protected:
        std::shared_ptr<Http::HttpServer> httpServer;

        void AddRoute(Http::Route route, std::function<Http::HandlerResponse(Http::HttpRequest)> handler);
    };
}

#endif //RAFT_WEBAPPLICATION_H
