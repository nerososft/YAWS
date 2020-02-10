//
// Created by XingfengYang on 2020/2/8.
//

#include "../../include/app/WebApplication.h"

void App::WebApplication::Init() {

}

void App::WebApplication::AddRoute(Http::Route route, std::function<Http::HandlerResponse(Http::HttpRequest)> handler) {
    this->httpServer->AddRoute(route, handler);
}

void App::WebApplication::SetServer(std::shared_ptr<Http::HttpServer> server) {
    this->httpServer = server;
}

