//
// Created by XingfengYang on 2020/2/10.
//

#include "../../include/app/AppConfig.h"
#include "../../include/app/Dashboard.h"

void App::AppConfig::AttachApps(std::shared_ptr<Http::HttpServer> httpServer) {
    App::Dashboard dashboard;
    dashboard.SetServer(httpServer);
    dashboard.Init();
}
