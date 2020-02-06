//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_IRAFTSERVER_H
#define RAFT_IRAFTSERVER_H

#include <functional>
#include <vector>
#include <memory>
#include "RaftMessage.h"
#include "ISocket.h"
#include <mutex>
#include <future>
#include "Config.h"

namespace Raft {

    class IRaftServer {
    public:
        struct Configuration {
            std::vector<unsigned int> instancesNumbers;
            unsigned int selfInstanceNumber = 0;
            unsigned int currentTerm = 0;
            double minimumElectionTimeout = 0.15;
            double maximumElectionTimeout = 0.3;
            double rpcTimeout = 0.015;
            ISocket::SocketConfiguration socketConfiguration;

            std::map<unsigned int, EndPoint> endPoints;
        };

        using SendMessageDelegate = std::function<void(std::shared_ptr<RaftMessage> message,
                                                       unsigned int receivedInstanceNumber)>;

    public:

        virtual bool Configure(const Configuration &configuration) = 0;

        virtual void SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) = 0;

        virtual void ReceiveMessage(std::shared_ptr<RaftMessage> message,
                                    unsigned int senderInstanceNumber) = 0;

    private:
    };
}

#endif //RAFT_IRAFTSERVER_H
