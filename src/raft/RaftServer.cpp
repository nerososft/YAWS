//
// Created by XingfengYang on 2020/1/1.
//
#include "../../include/raft/RaftServer.h"
#include "../../include/raft/RaftMessage.h"
#include "../../include/log/Log.h"
#include <thread>
#include <future>
#include <memory>
#include <mutex>

namespace Raft {

    RaftServer::~RaftServer() noexcept = default;

    RaftServer::RaftServer(Raft::RaftServer &&) noexcept = default;

    RaftServer &RaftServer::operator=(RaftServer &&) noexcept = default;

    RaftServer::RaftServer() :
            raftServer(std::make_shared<RaftServerImpl>()),
            socketOps(std::make_shared<Connect::SocketImpl>()) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//        raftServer->sharedProperties->randomGenerator.seed(seed);
    }

    void RaftServer::SetTimeKeeper(std::shared_ptr<Timer::TimeKeeper> timeKeeper) {
        raftServer->timeKeeper = timeKeeper;
    }

    void RaftServer::Mobilize() {
        if (raftServer->serverWorker.joinable()) {
            return;
        }
        raftServer->SetRunning(isRunning);
        this->socketOps->Configure(raftServer->sharedProperties->configuration.socketConfiguration);
        this->socketOps->SetUp();
        this->socketOps->Bind();
        this->socketOps->Listen();
        LogInfo("[RaftServer] Raft Protocol listen at port: %d\n", raftServer->sharedProperties->configuration.socketConfiguration.port)
        raftServer->SetSocketOps(this->socketOps);
        raftServer->SetRunning(true);
        raftServer->serverWorker = std::thread(&RaftServerImpl::ServerWorker, raftServer.get());

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

    void RaftServer::Demobilize() {
        if (!raftServer->worker.joinable()) {
            return;
        }

        std::unique_lock<decltype(raftServer->sharedProperties->mutex)> lock(raftServer->sharedProperties->mutex);
        raftServer->stopWorker.set_value();
        raftServer->workerAskedToStopOrWeakUp.notify_one();
        lock.unlock();
        raftServer->worker.join();
    }

    void RaftServer::WaitForAtLeastOneWorkerLoop() {
        std::unique_lock<decltype(raftServer->sharedProperties->mutex)> lock(raftServer->sharedProperties->mutex);
        raftServer->sharedProperties->workerLoopCompletion = std::make_shared<std::promise<void>>();
        auto workerLoopCompletion = raftServer->sharedProperties->workerLoopCompletion->get_future();
        raftServer->workerAskedToStopOrWeakUp.notify_one();
        lock.unlock();
        workerLoopCompletion.wait();
    }


    bool RaftServer::Configure(const Configuration &configuration) {
        raftServer->sharedProperties->configuration = configuration;
        return true;
    }

    void RaftServer::SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) {
        raftServer->sendMessageDelegate = sendMessageDelegate;
    }

    void RaftServer::ReceiveMessage(std::shared_ptr<RaftMessage> message,
                                    unsigned int senderInstanceNumber) {
        raftServer->ReceiveMessage(message, senderInstanceNumber);
    }
}

