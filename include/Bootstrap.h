//
// Created by XingfengYang on 2020/1/12.
//
#ifndef RAFT_BOOTSTRAP_H
#define RAFT_BOOTSTRAP_H

#include <memory>
#include "raft/RaftServer.h"
#include "connect/SocketOps.h"
#include "http/HttpServer.h"
#include <fstream>

namespace Bootstrap {
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
        std::shared_ptr<Timer::TimeKeeper> timeKeeper;
        std::shared_ptr<Raft::RaftServer> raftServer;
        std::shared_ptr<Http::HttpServer> httpServer;

        Connect::Config config;

    private:
        std::vector<std::string> args;

    public:
        void Run(int argc, char *argv[]);

        static void PrintSplash();

        void BootstrapRaftServer() const;

        void BootstrapHttpServer() const;

        void ConfigHttpServer() const;

        void AttachApps() const;
    };

}
#endif //RAFT_BOOTSTRAP_H
