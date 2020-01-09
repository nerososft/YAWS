//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_MESSAGE_H
#define RAFT_MESSAGE_H

#include <memory>
#include <functional>
#include "RaftMessage.h"

namespace Raft {
    class Message {
    public:
        ~Message() noexcept;

        Message(const Message &) = delete;

        Message(Message &&) noexcept;

        Message &operator=(const Message &) = delete;

        Message &operator=(Message &&) noexcept;

    public:
        static std::function<std::shared_ptr<Message>()> CreateMessage;

    public:
        Message();

    public:
        std::shared_ptr<RaftMessage> raftMessage;
    };
}
#endif //RAFT_MESSAGE_H
