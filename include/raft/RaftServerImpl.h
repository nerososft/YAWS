//
// Created by XingfengYang on 2020/1/2.
//

#ifndef RAFT_RAFTSERVERIMPL_H
#define RAFT_RAFTSERVERIMPL_H

#include <memory>
#include <thread>
#include <future>
#include <condition_variable>
#include <random>
#include <mutex>
#include <map>
#include "IRaftServer.h"
#include "../timer/TimeKeeper.h"
#include "../connect/SocketImpl.h"

namespace {

    struct InstanceInfo {

        bool awaitingVote = false;

        double timeLastRequestSend = 0.0;

        std::shared_ptr<Raft::RaftMessage> lastRequest;
    };

    struct ServerSharedProperties {

        Raft::IRaftServer::Configuration configuration;

        std::mt19937 randomGenerator;

        std::mutex mutex;

        std::shared_ptr<std::promise<void>> workerLoopCompletion;

        double currentElectionTimeout = 0.0;

        double timeOfLastLeaderMessage = 0.0;

        bool isLeader = false;

        size_t votesForUs = 0;

        bool votedThisTerm = false;

        unsigned int votedFor = 0;

        std::map<unsigned int, InstanceInfo> instances;
    };
}

namespace Raft {
    class RaftServerImpl {

    public:
        ~RaftServerImpl() noexcept;

    public:
        std::shared_ptr<ServerSharedProperties> sharedProperties;
        std::shared_ptr<TimeKeeper> timeKeeper;

        IRaftServer::SendMessageDelegate sendMessageDelegate;

        std::thread worker;
        std::thread serverWorker;
        std::promise<void> stopWorker;

        std::shared_ptr<SocketImpl> socket;

        std::condition_variable workerAskedToStopOrWeakUp;

        bool isRunning = false;

    public:
        RaftServerImpl();

        void ResetElectionTimer();

        double GetTimeSinceLastLeaderMessage(double now);

        void SendMessage(const std::shared_ptr<RaftMessage> &message, unsigned int instanceNumber, double now);

        void ReceiveMessage(std::shared_ptr<RaftMessage> message, unsigned int senderInstanceNumber);

        void StartElection(double now);

        void SendHeartBeat(double now);

        void DoRetransmission(double now);

        void RevertToFollower();

        void Worker();

        void ServerWorker();

        void SetRunning(bool running);

        void SetSocketOps(std::shared_ptr<SocketImpl> socketOps);

        void SendMessageImpl(std::shared_ptr<RaftMessage> message, unsigned int receivedInstanceNumber);

    private:
        void Handler(char *buffer, int fdc);
    };
}
#endif //RAFT_RAFTSERVERIMPL_H
