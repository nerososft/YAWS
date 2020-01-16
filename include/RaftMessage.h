//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_RAFTMESSAGE_H
#define RAFT_RAFTMESSAGE_H

#include <memory>
#include <functional>
#include "RaftMessageImpl.h"

namespace Raft {
    class RaftMessage {
    public:
        ~RaftMessage() noexcept;

        RaftMessage(const RaftMessage &) = delete;

        RaftMessage(RaftMessage &&) noexcept;

        RaftMessage &operator=(const RaftMessage &) = delete;

        RaftMessage &operator=(RaftMessage &&) noexcept;

    public:
        static std::function<std::shared_ptr<RaftMessage>()> CreateMessage;

    public:
        RaftMessage();

    public:
        std::shared_ptr<RaftMessageImpl> raftMessage;
    };
}
#endif //RAFT_RAFTMESSAGE_H
