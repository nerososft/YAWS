//
// Created by XingfengYang on 2020/1/12.
//
#ifndef RAFT_BOOTSTRAP_H
#define RAFT_BOOTSTRAP_H

#include <memory>
#include "Server.h"
#include "SocketOps.h"

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
        std::shared_ptr<Server> server;
        std::shared_ptr<SocketOps> socketOps;

    public:
        void Run();

        void PrintSplash();
    };

}
#endif //RAFT_BOOTSTRAP_H
