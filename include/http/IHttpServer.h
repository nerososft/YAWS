//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_IHTTPSERVER_H
#define RAFT_IHTTPSERVER_H

#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include "HttpMessage.h"
#include "../connect/ISocket.h"

namespace Http {

    class IHttpServer {
    public:
        struct Configuration {
            Connect::ISocket::SocketConfiguration socketConfiguration;
        };

        using SendMessageDelegate = std::function<void(std::shared_ptr<HttpMessage> message,
                                                       unsigned int receivedInstanceNumber)>;

    public:

        virtual bool Configure(const Configuration &configuration) = 0;

        virtual void SetSendMessageDelegate(SendMessageDelegate sendMessageDelegate) = 0;

        virtual void ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                    unsigned int senderInstanceNumber) = 0;

    private:
    };
}

#endif //RAFT_IHTTPSERVER_H
