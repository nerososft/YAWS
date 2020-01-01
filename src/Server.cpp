//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/Server.h"
#include "../include/Message.h"
#include "../include/RaftMessageImpl.h"
#include <thread>
#include <future>
#include <memory>
#include <mutex>
#include <map>
#include <random>
#include <time.h>

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

    struct Server::RaftServerImpl {
        std::shared_ptr<ServerSharedProperties> sharedProperties;
        std::shared_ptr<TimeKeeper> timeKeeper;

        SendMessageDelegate sendMessageDelegate;

        std::thread worker;
        std::promise<void> stopWorker;

        std::condition_variable workerAskedToStopOrWeakUp;

        void ResetElectionTimer() {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            sharedProperties->timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
            sharedProperties->currentElectionTimeout = std::uniform_real_distribution<>(
                    sharedProperties->configuration.minimumElectionTimeout,
                    sharedProperties->configuration.maximumElectionTimeout
            )(sharedProperties->randomGenerator);
        }

        double GetTimeSinceLastLeaderMessage(double now) {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            return now - sharedProperties->timeOfLastLeaderMessage;
        }

        void SendMessage(std::shared_ptr<Message> message, unsigned int instanceNumber, double now) {
            auto &instance = sharedProperties->instances[instanceNumber];
            instance.timeLastRequestSend = now;
            instance.lastRequest = message;

            sendMessageDelegate(message, instanceNumber);
        }

        void StartElection(double now) {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            sharedProperties->votesForUs = 1;
            const auto message = Message::CreateMessage();
            message->raftMessage->type = RaftMessageImpl::Type::RequestVote;
            message->raftMessage->requestVoteDetails.candidateId = sharedProperties->configuration.selfInstanceNumber;
            message->raftMessage->requestVoteDetails.term = ++sharedProperties->configuration.currentTerm;

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
            double timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
        }

        void SendHeartBeat(double now) {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            sharedProperties->votesForUs = 1;
            const auto message = Message::CreateMessage();
            message->raftMessage->type = RaftMessageImpl::Type::HeartBeat;
            message->raftMessage->requestVoteDetails.term = sharedProperties->configuration.currentTerm;

            for (auto instanceNumber: sharedProperties->configuration.instancesNumbers) {
                if (instanceNumber == sharedProperties->configuration.selfInstanceNumber) {
                    continue;
                }
                SendMessage(message, instanceNumber, now);
            }
            double timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
        }

        void DoRetransmission(double now) {
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

        void RevertToFollower() {
            for (auto &instanceEntry: sharedProperties->instances) {
                instanceEntry.second.awaitingVote = false;
            }
            sharedProperties->isLeader = false;
            ResetElectionTimer();
        }

        void Worker() {
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
    };

    Server::~Server() noexcept = default;

    Server::Server(Raft::Server &&) noexcept = default;

    Server &Server::operator=(Server &&) noexcept = default;

    Server::Server() : raftServer(new RaftServerImpl()) {
        raftServer->sharedProperties->randomGenerator.seed((int) time(NULL));
    }

    void Server::SetTimeKeeper(std::shared_ptr<TimeKeeper> timeKeeper) {
        raftServer->timeKeeper = timeKeeper;
    }

    void Server::Mobilize() {
        if (raftServer->worker.joinable()) {
            return;
        }
        raftServer->sharedProperties->instances.clear();
        raftServer->sharedProperties->isLeader = false;
        raftServer->sharedProperties->timeOfLastLeaderMessage = 0.0;
        raftServer->sharedProperties->votesForUs = 0;
        raftServer->stopWorker = std::promise<void>();
        raftServer->worker = std::thread(&RaftServerImpl::Worker, raftServer.get());
    }

    void Server::Demobilize() {
        if (!raftServer->worker.joinable()) {
            return;
        }

        std::unique_lock<decltype(raftServer->sharedProperties->mutex)> lock(raftServer->sharedProperties->mutex);
        raftServer->stopWorker.set_value();
        raftServer->workerAskedToStopOrWeakUp.notify_one();
        lock.unlock();
        raftServer->worker.join();
    }

    void Server::WaitForAtLeastOneWorkerLoop() {
        std::unique_lock<decltype(raftServer->sharedProperties->mutex)> lock(raftServer->sharedProperties->mutex);
        raftServer->sharedProperties->workerLoopCompletion = std::make_shared<std::promise<void>>();
        auto workerLoopCompletion = raftServer->sharedProperties->workerLoopCompletion->get_future();
        raftServer->workerAskedToStopOrWeakUp.notify_one();
        lock.unlock();
        workerLoopCompletion.wait();
    }


    bool Server::Configure(const Configuration &configuration) {
        raftServer->sharedProperties->configuration = configuration;
        return true;
    }

    void Server::SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) {
        raftServer->sendMessageDelegate = sendMessageDelegate;
    }

    void Server::ReceiveMessage(std::shared_ptr<Message> message,
                                unsigned int senderInstanceNumber) {
        const double now = raftServer->timeKeeper->GetCurrentTime();
        switch (message->raftMessage->type) {
            case RaftMessageImpl::Type::RequestVote: {
                if (raftServer->sharedProperties->configuration.currentTerm < message->raftMessage->requestVoteDetails.term) {
                    raftServer->sharedProperties->configuration.currentTerm = message->raftMessage->requestVoteDetails.term;
                }

                const auto response = Message::CreateMessage();
                response->raftMessage->type = RaftMessageImpl::Type::RequestVoteResults;
                response->raftMessage->requestVoteResultsDetails.term = message->raftMessage->requestVoteDetails.term;

                if (raftServer->sharedProperties->configuration.currentTerm > message->raftMessage->requestVoteDetails.term) {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = false;
                } else if (raftServer->sharedProperties->votedThisTerm &&
                           raftServer->sharedProperties->votedFor != senderInstanceNumber) {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = false;
                } else {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = true;
                    raftServer->sharedProperties->votedThisTerm = true;
                    raftServer->sharedProperties->votedFor = senderInstanceNumber;
                    raftServer->RevertToFollower();
                }
                raftServer->SendMessage(message, senderInstanceNumber, now);
            }
                break;
            case RaftMessageImpl::Type::RequestVoteResults: {

                auto &instance = raftServer->sharedProperties->instances[senderInstanceNumber];
                instance.awaitingVote = false;
                if (message->raftMessage->requestVoteResultsDetails.voteGranted) {
                    ++raftServer->sharedProperties->votesForUs;
                    if (raftServer->sharedProperties->votesForUs >=
                        raftServer->sharedProperties->configuration.instancesNumbers.size() / 2 + 1) {
                        raftServer->sharedProperties->isLeader = true;
                    }
                }
            }
                break;

            case RaftMessageImpl::Type::HeartBeat: {
                if (raftServer->sharedProperties->configuration.currentTerm < message->raftMessage->requestVoteDetails.term) {
                    raftServer->RevertToFollower();
                }
            }
                break;
            default: {
            }
                break;
        }
    }

    bool Server::IsLeader() {
        return raftServer->sharedProperties->isLeader;
    }


}

