//
// Created by XingfengYang on 2020/2/10.
//

#ifndef RAFT_APPCONFIG_H
#define RAFT_APPCONFIG_H

#include "../http/HttpServer.h"

namespace App {
    class AppConfig {
    public:
        void AttachApps(std::shared_ptr<Http::HttpServer> httpServer);
    };
}
#endif //RAFT_APPCONFIG_H
