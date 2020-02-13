//
// Created by XingfengYang on 2020/1/2.
//
#include <iostream>
#include <utility>
#include "../../include/raft/RaftServerImpl.h"
#include "../../include/raft/RaftMessageImpl.h"

#include "../../include/log/Log.h"
#include <unistd.h>

#include <map>

namespace Raft {

    RaftServerImpl::~RaftServerImpl() noexcept = default;

    RaftServerImpl::RaftServerImpl() :
            sharedProperties(std::make_shared<ServerSharedProperties>()) {
        this->sendMessageDelegate = std::bind(&RaftServerImpl::SendMessageImpl, this, std::placeholders::_1, std::placeholders::_2);
    }

    void RaftServerImpl::SendMessageImpl(std::shared_ptr<RaftMessage> message, unsigned int receivedInstanceNumber) {
        char *encodedMessage = message->raftMessage->EncodeMessage();
        // get EndPoint from config
        if (this->sharedProperties->configuration.endPoints.count(receivedInstanceNumber)) {
            Connect::EndPoint endPoint = this->sharedProperties->configuration.endPoints.find(receivedInstanceNumber)->second;
            socket->Send(endPoint, encodedMessage);
        }
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

    void RaftServerImpl::ReceiveMessage(std::shared_ptr<RaftMessage> message,
                                        unsigned int senderInstanceNumber) {

        LogInfo("[Raft] Received Message, type %d [%s]\n", message->raftMessage->type, message->getMessageType())

        const double now = timeKeeper->GetCurrentTime();
        switch (message->raftMessage->type) {
            case Type::RequestVote: {
                const auto response = RaftMessage::CreateMessage();
                response->raftMessage->type = Type::RequestVoteResults;
                response->raftMessage->requestVoteResultsDetails.term = std::max(
                        message->raftMessage->requestVoteDetails.term,
                        sharedProperties->configuration.currentTerm
                );

                if (sharedProperties->configuration.currentTerm > message->raftMessage->requestVoteDetails.term) {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = false;
                } else if (sharedProperties->configuration.currentTerm ==
                           message->raftMessage->requestVoteDetails.term &&
                           sharedProperties->votedThisTerm &&
                           sharedProperties->votedFor != senderInstanceNumber) {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = false;
                } else {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = true;
                    sharedProperties->votedThisTerm = true;
                    sharedProperties->votedFor = senderInstanceNumber;
                    sharedProperties->configuration.currentTerm = message->raftMessage->requestVoteDetails.term;
                    RevertToFollower();
                }
                SendMessage(response, senderInstanceNumber, now);
            }
                break;
            case Type::RequestVoteResults: {
                auto &instance = sharedProperties->instances[senderInstanceNumber];
                if (message->raftMessage->requestVoteResultsDetails.voteGranted) {
                    if (instance.awaitingVote) {
                        ++sharedProperties->votesForUs;
                        if (sharedProperties->votesForUs >=
                            sharedProperties->configuration.instancesNumbers.size() / 2 + 1) {
                            sharedProperties->isLeader = true;
                        }
                    }
                }
            }
                break;

            case Type::HeartBeat: {
                if (sharedProperties->configuration.currentTerm <
                    message->raftMessage->requestVoteDetails.term) {
                    sharedProperties->configuration.currentTerm = message->raftMessage->heartBeatDetails.term;
                    RevertToFollower();
                } else if (sharedProperties->configuration.currentTerm ==
                           message->raftMessage->requestVoteDetails.term) {
                    ResetElectionTimer();
                }
            }
                break;
            default: {
            }
                break;
        }
    }

    void RaftServerImpl::StartElection(double now) {
        LogInfo("[Raft] start Election, now: %f\n", now)
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
        while (workerAskedToStop.wait_for(std::chrono::seconds(0)) == std::future_status::ready ||
               workerAskedToStop.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {

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

    void RaftServerImpl::Handler(char *buffer, int fdc) {
        try {
            std::shared_ptr<RaftMessageImpl> raftMessageImpl = std::make_shared<RaftMessageImpl>();
            std::shared_ptr<RaftMessage> raftMessage = std::make_shared<RaftMessage>();
            raftMessage->raftMessage = raftMessageImpl->DecodeMessage(buffer);

            ReceiveMessage(raftMessage, 0);
        } catch (std::logic_error error) {
            LogError("Caught %s\n", error.what())
            char *buf = "Not A Raft RaftMessage.";
            write(fdc, buf, strlen(buf));
            close(fdc);
        }
    }

    void RaftServerImpl::ServerWorker() {
        while (isRunning) {
            const auto bind = std::bind(&RaftServerImpl::Handler, this, std::placeholders::_1, std::placeholders::_2);
            this->socket->Accept(bind);
        }
    }


    void RaftServerImpl::SetRunning(bool running) {
        this->isRunning = running;
    }

    void RaftServerImpl::SetSocketOps(std::shared_ptr<Connect::SocketImpl> socketOps) {
        this->socket = std::move(socketOps);
    }
}
