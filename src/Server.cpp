//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/Server.h"
#include "../include/Message.h"
#include "../include/MessageImpl.h"
#include <thread>
#include <future>
#include <memory>
#include <mutex>
#include <map>

namespace {

    struct InstanceInfo {
        bool awaitingVote = false;
    };

    struct ServerSharedProperties {
        Raft::IServer::Configuration configuration;

        std::mutex mutex;
        std::shared_ptr<std::promise<void>> workerLoopCompletion;

        double timeOfLastLeaderMessage;

        bool isLeader = false;

        size_t votesForUs = 0;

        std::map<unsigned int, InstanceInfo> instances;
    };
}


namespace Raft {

    struct Server::Impl {
        std::shared_ptr<ServerSharedProperties> sharedProperties;
        std::shared_ptr<TimeKeeper> timeKeeper;

        SendMessageDelegate sendMessageDelegate;

        std::thread worker;
        std::promise<void> stopWorker;


        void UpdateTimeOfLastLeaderMessage() {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            sharedProperties->timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
        }

        double GetTimeSinceLastLeaderMessage(double now) {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            return now - sharedProperties->timeOfLastLeaderMessage;
        }

        bool MakeWorkerThreadLoopPromiseIfNeeded() {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            return sharedProperties->workerLoopCompletion != nullptr;
        }

        void StartElection(double now) {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);

            sharedProperties->votesForUs = 1;
            const auto message = Message::CreateMessage();
            message->impl->type = MessageImpl::Type::RequestVote;
            message->impl->requestVoteDetails.candidateId = sharedProperties->configuration.selfInstanceNumber;
            message->impl->requestVoteDetails.term = ++sharedProperties->configuration.currentTerm;


            for (auto &instance:sharedProperties->instances) {
                instance.second.awaitingVote = false;
            }
            for (auto instanceNumber: sharedProperties->configuration.instancesNumbers) {
                if (instanceNumber == sharedProperties->configuration.selfInstanceNumber) {
                    continue;
                }
                auto &instance = sharedProperties->instances[instanceNumber];
                instance.awaitingVote = true;
                sendMessageDelegate(message, instanceNumber);
            }
            double timeOfLastLeaderMessage = timeKeeper->GetCurrentTime();
        }

        void DoRetransmission(double now) {
            std::lock_guard<decltype(sharedProperties->mutex)> lock(sharedProperties->mutex);
            for (auto instance: sharedProperties->configuration.instancesNumbers) {
                if (instance == sharedProperties->configuration.selfInstanceNumber) {
                    continue;
                }
            }
        }

        void Worker() {
            UpdateTimeOfLastLeaderMessage();
            auto workerAskedToStop = stopWorker.get_future();

            int rpcTimeoutMilliseconds = (int) (sharedProperties->configuration.rpcTimeout * 1000.0);

            while (workerAskedToStop.wait_for(
                    std::chrono::milliseconds(rpcTimeoutMilliseconds)
            ) != std::future_status::ready) {
                const bool signalWorkerLoopCompletion = MakeWorkerThreadLoopPromiseIfNeeded();
                const double now = timeKeeper->GetCurrentTime();
                const double timeSinceLastLeaderMessage = GetTimeSinceLastLeaderMessage(now);

                if (timeSinceLastLeaderMessage >= sharedProperties->configuration.minimumElectionTimeout) {
                    StartElection(now);
                }
                DoRetransmission(now);
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

    Server::Server() : impl(new Impl()) {

    }

    void Server::SetTimeKeeper(std::shared_ptr<TimeKeeper> timeKeeper) {
        impl->timeKeeper = timeKeeper;
    }

    void Server::Mobilize() {
        impl->worker = std::thread(&Impl::Worker, impl.get());
    }

    void Server::Demobilize() {
        if (!impl->worker.joinable()) {
            return;
        }
        impl->stopWorker.set_value();
        impl->worker.join();
    }

    bool Server::Configure(const Configuration &configuration) {
        impl->sharedProperties->configuration = configuration;
        return true;
    }

    void Server::SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) {
        impl->sendMessageDelegate = sendMessageDelegate;
    }

    void Server::ReceiveMessage(std::shared_ptr<Message> message,
                                unsigned int senderInstanceNumber) {
        switch (message->impl->type) {
            case MessageImpl::Type::RequestVoteResults: {

                auto &instance = impl->sharedProperties->instances[senderInstanceNumber];
                instance.awaitingVote = false;

                ++impl->sharedProperties->votesForUs;
                if (impl->sharedProperties->votesForUs >=
                    impl->sharedProperties->configuration.instancesNumbers.size() / 2 + 1) {
                    impl->sharedProperties->isLeader = true;
                }
            }
                break;
            default: {
            }
                break;
        }
    }

    bool Server::IsLeader() {
        return impl->sharedProperties->isLeader;
    }


}

