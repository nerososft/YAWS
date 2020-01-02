//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_SERVER_H
#define RAFT_SERVER_H

#include <memory>

#include "IServer.h"
#include "TimeKeeper.h"
#include "../include/RaftCore.h"

namespace Raft {
    class Server : public IServer {
    public:
        ~Server() noexcept;

        Server(const Server &) = delete;

        Server(Server &&) noexcept;

        Server &operator=(const Server &) = delete;

        Server &operator=(Server &&) noexcept;

    public:
        Server();

        void SetTimeKeeper(std::shared_ptr<TimeKeeper> timeKeeper);

        void Mobilize();

        void Demobilize();

        void WaitForAtLeastOneWorkerLoop();

    public:
        virtual bool Configure(const Configuration &configuration) override;

        virtual void SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) override;

        virtual void ReceiveMessage(std::shared_ptr<Message> message,
                                    unsigned int senderInstanceNumber) override;

        virtual bool IsLeader() override;

    private:
        std::shared_ptr<RaftCore> raftServer;
    };
}
#endif //RAFT_SERVER_H
