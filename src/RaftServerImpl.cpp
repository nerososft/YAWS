//
// Created by XingfengYang on 2020/1/2.
//
#include <iostream>
#include <utility>
#include "../include/RaftServerImpl.h"
#include "../include/RaftMessageImpl.h"

#include "../include/Log.h"
#include <unistd.h>

namespace Raft {

    RaftServerImpl::~RaftServerImpl() noexcept = default;

    RaftServerImpl::RaftServerImpl() :
            sharedProperties(std::make_shared<ServerSharedProperties>()) {
    }

    void RaftServerImpl::ResetElectionTimer() {
        std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
        sharedProperties->timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
        sharedProperties->currentElectionTimeout = std::uniform_real_distribution<>(
                sharedProperties->configuration.minimumElectionTimeout,
                sharedProperties->configuration.maximumElectionTimeout
        )(sharedProperties->randomGenerator);
    }

    double RaftServerImpl::GetTimeSinceLastLeaderMessage(double now) {
        std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
        return now - sharedProperties->timeOfLastLeaderMessage;
    }

    void RaftServerImpl::SendMessage(const std::shared_ptr<RaftMessage> &message, unsigned int instanceNumber, double now) {
        auto &instance = sharedProperties->instances[instanceNumber];
        instance.timeLastRequestSend = now;
        instance.lastRequest = message;

        sendMessageDelegate(message, instanceNumber);
    }

    void RaftServerImpl::StartElection(double now) {
        ++sharedProperties->configuration.currentTerm;

        sharedProperties->votedThisTerm = true;
        sharedProperties->votedFor = sharedProperties->configuration.selfInstanceNumber;
        sharedProperties->votesForUs = 1;

        const auto message = RaftMessage::CreateMessage();
        message->raftMessage->type = Type::RequestVote;
        message->raftMessage->requestVoteDetails.candidateId = sharedProperties->configuration.selfInstanceNumber;
        message->raftMessage->requestVoteDetails.term = sharedProperties->configuration.currentTerm;

        for (auto &instance:sharedProperties->instances) {
            instance.second.awaitingVote = false;
        }
        for (auto instanceNumber: sharedProperties->configuration.instancesNumbers) {
            if (instanceNumber == sharedProperties->configuration.selfInstanceNumber) {
                continue;
            }
            auto &instance = sharedProperties->instances[instanceNumber];
            instance.awaitingVote = true;

            SendMessage(message, instanceNumber, now);
        }
        sharedProperties->timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
    }

    void RaftServerImpl::SendHeartBeat(double now) {
        std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
        sharedProperties->votesForUs = 1;
        const auto message = RaftMessage::CreateMessage();
        message->raftMessage->type = Type::HeartBeat;
        message->raftMessage->requestVoteDetails.term = sharedProperties->configuration.currentTerm;

        for (auto instanceNumber: sharedProperties->configuration.instancesNumbers) {
            if (instanceNumber == sharedProperties->configuration.selfInstanceNumber) {
                continue;
            }
            SendMessage(message, instanceNumber, now);
        }
        this->sharedProperties->timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
    }

    void RaftServerImpl::DoRetransmission(double now) {
        std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
        for (auto &instanceEntry: sharedProperties->instances) {
            if (instanceEntry.second.awaitingVote &&
                (now - instanceEntry.second.timeLastRequestSend >= sharedProperties->configuration.rpcTimeout)) {
                SendMessage(instanceEntry.second.lastRequest,
                            instanceEntry.first,
                            now);
            }
        }
    }

    void RaftServerImpl::RevertToFollower() {
        for (auto &instanceEntry: sharedProperties->instances) {
            instanceEntry.second.awaitingVote = false;
        }
        sharedProperties->isLeader = false;
        ResetElectionTimer();
    }

    void RaftServerImpl::Worker() {
        ResetElectionTimer();
        int rpcTimeoutMilliseconds = (int) (sharedProperties->configuration.rpcTimeout * 1000.0);
        std::future<void> workerAskedToStop = stopWorker.get_future();
        std::unique_lock<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
        while (workerAskedToStop.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            (void) workerAskedToStopOrWeakUp.wait_for(
                    lock,
                    std::chrono::milliseconds(rpcTimeoutMilliseconds)
            );

            const bool signalWorkerLoopCompletion = (sharedProperties->workerLoopCompletion != nullptr);

            lock.unlock();
            const double now = timeKeeper->GetCurrentTime();
            const double timeSinceLastLeaderMessage = GetTimeSinceLastLeaderMessage(now);
            if (sharedProperties->isLeader) {
                if (timeSinceLastLeaderMessage >= sharedProperties->configuration.minimumElectionTimeout / 2) {
                    SendHeartBeat(now);
                }
            } else {
                if (timeSinceLastLeaderMessage >= sharedProperties->currentElectionTimeout) {
                    StartElection(now);
                }
            }
            DoRetransmission(now);
            lock.lock();
            if (signalWorkerLoopCompletion) {
                sharedProperties->workerLoopCompletion->set_value();
                sharedProperties->workerLoopCompletion = nullptr;
            }
        }
        if (sharedProperties->workerLoopCompletion != nullptr) {
            sharedProperties->workerLoopCompletion->set_value();
            sharedProperties->workerLoopCompletion = nullptr;
        }
    }


#define TEST_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>Raft Server Status:\n</h1>"\
"<p style='color:green;'>Raft Server works.\n</p>"

#define DAILED_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>ooops:\n</h1>"\
"<p style='color:red;'>Not A Raft RaftMessage.\n</p>"

    void RaftServerImpl::Handler(char *buffer, int fdc) {
        try {
            auto *raftMessage = new RaftMessageImpl();
            raftMessage->DecodeMessage(buffer);
            char *buf = TEST_HTTP_RESPONSE;
            write(fdc, buf, strlen(buf));
        } catch (std::logic_error error) {
            PrintColor2("Caught %s\n", error.what())
            char *buf = DAILED_HTTP_RESPONSE;
            write(fdc, buf, strlen(buf));
        }
        close(fdc);
    }

    void RaftServerImpl::ServerWorker() {
        while (isRunning) {
            const auto bind = std::bind(&RaftServerImpl::Handler, this, std::placeholders::_1, std::placeholders::_2);
            this->socket->Accept(bind);
        }
    }


    void RaftServerImpl::SetRunning(bool isRunning) {
        this->isRunning = isRunning;
    }

    void RaftServerImpl::SetSocketOps(std::shared_ptr<SocketImpl> socketOps) {
        this->socket = std::move(socketOps);
    }
}
