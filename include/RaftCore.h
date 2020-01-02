//
// Created by XingfengYang on 2020/1/2.
//

#ifndef RAFT_RAFTCORE_H
#define RAFT_RAFTCORE_H

#include <memory>
#include <thread>
#include <future>
#include <condition_variable>
#include <random>
#include <mutex>
#include <map>
#include "IServer.h"
#include "TimeKeeper.h"

namespace {

    struct InstanceInfo {

        bool awaitingVote = false;

        double timeLastRequestSend = 0.0;

        std::shared_ptr<Raft::Message> lastRequest;
    };

    struct ServerSharedProperties {

        Raft::IServer::Configuration configuration;

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

    class RaftCore {

    public:
        ~RaftCore() noexcept;

    public:
        std::shared_ptr<ServerSharedProperties> sharedProperties;
        std::shared_ptr<TimeKeeper> timeKeeper;

        IServer::SendMessageDelegate sendMessageDelegate;

        std::thread worker;
        std::promise<void> stopWorker;

        std::condition_variable workerAskedToStopOrWeakUp;

    public:
        RaftCore();

        void ResetElectionTimer();

        double GetTimeSinceLastLeaderMessage(double now);

        void SendMessage(std::shared_ptr<Message> message, unsigned int instanceNumber, double now);

        void StartElection(double now);

        void SendHeartBeat(double now);

        void DoRetransmission(double now);

        void RevertToFollower();

        void Worker();

        bool IsLeader();
    };
}
#endif //RAFT_RAFTCORE_H
