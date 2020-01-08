//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/Server.h"
#include "../include/Message.h"
#include "../include/RaftMessage.h"
#include <thread>
#include <future>
#include <memory>
#include <mutex>
#include <algorithm>

namespace Raft {

    Server::~Server() noexcept = default;

    Server::Server(Raft::Server &&) noexcept = default;

    Server &Server::operator=(Server &&) noexcept = default;

    Server::Server() : raftServer(new RaftCore()) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//        raftServer->sharedProperties->randomGenerator.seed(seed);
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
        raftServer->worker = std::thread(&RaftCore::Worker, raftServer.get());
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
            case RaftMessage::Type::RequestVote: {
                const auto response = Message::CreateMessage();
                response->raftMessage->type = RaftMessage::Type::RequestVoteResults;
                response->raftMessage->requestVoteResultsDetails.term = std::max(
                        message->raftMessage->requestVoteDetails.term,
                        raftServer->sharedProperties->configuration.currentTerm
                );

                if (raftServer->sharedProperties->configuration.currentTerm >
                    message->raftMessage->requestVoteDetails.term) {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = false;
                } else if (raftServer->sharedProperties->configuration.currentTerm ==
                           message->raftMessage->requestVoteDetails.term &&
                           raftServer->sharedProperties->votedThisTerm &&
                           raftServer->sharedProperties->votedFor != senderInstanceNumber) {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = false;
                } else {
                    response->raftMessage->requestVoteResultsDetails.voteGranted = true;
                    raftServer->sharedProperties->votedThisTerm = true;
                    raftServer->sharedProperties->votedFor = senderInstanceNumber;
                    raftServer->sharedProperties->configuration.currentTerm = message->raftMessage->requestVoteDetails.term;
                    raftServer->RevertToFollower();
                }
                raftServer->SendMessage(response, senderInstanceNumber, now);
            }
                break;
            case RaftMessage::Type::RequestVoteResults: {
                auto &instance = raftServer->sharedProperties->instances[senderInstanceNumber];
                if (message->raftMessage->requestVoteResultsDetails.voteGranted) {
                    if (instance.awaitingVote) {
                        ++raftServer->sharedProperties->votesForUs;
                        if (raftServer->sharedProperties->votesForUs >=
                            raftServer->sharedProperties->configuration.instancesNumbers.size() / 2 + 1) {
                            raftServer->sharedProperties->isLeader = true;
                        }
                    }
                }
            }
                break;

            case RaftMessage::Type::HeartBeat: {
                if (raftServer->sharedProperties->configuration.currentTerm <
                    message->raftMessage->requestVoteDetails.term) {
                    raftServer->sharedProperties->configuration.currentTerm = message->raftMessage->heartBeatDetails.term;
                    raftServer->RevertToFollower();
                } else if (raftServer->sharedProperties->configuration.currentTerm ==
                           message->raftMessage->requestVoteDetails.term) {
                    raftServer->ResetElectionTimer();
                }
            }
                break;
            default: {
            }
                break;
        }
    }
}

