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

        std::condition_variable workerAskedToStopOrWeakUp;


        void UpdateTimeOfLastLeaderMessage() {
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

        void Worker() {
            UpdateTimeOfLastLeaderMessage();
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
                if (timeSinceLastLeaderMessage >= sharedProperties->currentElectionTimeout) {
                    StartElection(now);
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

    Server::Server() : impl(new Impl()) {
        impl->sharedProperties->randomGenerator.seed((int) time(NULL));
    }

    void Server::SetTimeKeeper(std::shared_ptr<TimeKeeper> timeKeeper) {
        impl->timeKeeper = timeKeeper;
    }

    void Server::Mobilize() {
        if (impl->worker.joinable()) {
            return;
        }
        impl->sharedProperties->instances.clear();
        impl->sharedProperties->isLeader = false;
        impl->sharedProperties->timeOfLastLeaderMessage = 0.0;
        impl->sharedProperties->votesForUs = 0;
        impl->stopWorker = std::promise<void>();
        impl->worker = std::thread(&Impl::Worker, impl.get());
    }

    void Server::Demobilize() {
        if (!impl->worker.joinable()) {
            return;
        }

        std::unique_lock<decltype(impl->sharedProperties->mutex)> lock(impl->sharedProperties->mutex);
        impl->stopWorker.set_value();
        impl->workerAskedToStopOrWeakUp.notify_one();
        lock.unlock();
        impl->worker.join();
    }

    void Server::WaitForAtLeastOneWorkerLoop() {
        std::unique_lock<decltype(impl->sharedProperties->mutex)> lock(impl->sharedProperties->mutex);
        impl->sharedProperties->workerLoopCompletion = std::make_shared<std::promise<void>>();
        auto workerLoopCompletion = impl->sharedProperties->workerLoopCompletion->get_future();
        impl->workerAskedToStopOrWeakUp.notify_one();
        lock.unlock();
        workerLoopCompletion.wait();
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
                if (message->impl->requestVoteResultsDetails.voteGranted) {
                    ++impl->sharedProperties->votesForUs;
                    if (impl->sharedProperties->votesForUs >=
                        impl->sharedProperties->configuration.instancesNumbers.size() / 2 + 1) {
                        impl->sharedProperties->isLeader = true;
                    }
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

