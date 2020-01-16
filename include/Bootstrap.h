//
// Created by XingfengYang on 2020/1/12.
//
#ifndef RAFT_BOOTSTRAP_H
#define RAFT_BOOTSTRAP_H

#include <memory>
#include "RaftServer.h"
#include "SocketOps.h"
#include "HttpServer.h"
#include <fstream>

namespace Raft {
    class RaftBootstrap {
    public:
        ~RaftBootstrap() noexcept;

        RaftBootstrap(const RaftBootstrap &) = delete;

        RaftBootstrap(RaftBootstrap &&) noexcept;

        RaftBootstrap &operator=(const RaftBootstrap &) = delete;

        RaftBootstrap &operator=(RaftBootstrap &&) noexcept;

    public:
        RaftBootstrap();

    private:
        void LoadConfigFile();

    private:
        bool isRunning = true;
        std::shared_ptr<TimeKeeper> timeKeeper;
        std::shared_ptr<RaftServer> raftServer;
        std::shared_ptr<HttpServer> httpServer;

    public:
        void Run();

        static void PrintSplash();
    };

}
#endif //RAFT_BOOTSTRAP_H
