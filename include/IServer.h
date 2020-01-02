//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_ISERVER_H
#define RAFT_ISERVER_H

#include <functional>
#include <vector>
#include <memory>
#include "Message.h"
#include <mutex>
#include <future>

namespace Raft {

    class IServer {
    public:
        struct Configuration {
            std::vector<unsigned int> instancesNumbers;
            unsigned int selfInstanceNumber = 0;

            unsigned int currentTerm = 0;


            double minimumElectionTimeout = 0.15;
            double maximumElectionTimeout = 0.3;
            double rpcTimeout = 0.015;
        };

        using SendMessageDelegate = std::function<void(std::shared_ptr<Message> message,
                                                       unsigned int receivedInstanceNumber)>;

    public:
        virtual bool Configure(const Configuration &configuration) = 0;

        virtual void SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) = 0;

        virtual void ReceiveMessage(std::shared_ptr<Message> message,
                                    unsigned int senderInstanceNumber) = 0;

    private:
    };
}

#endif //RAFT_ISERVER_H
