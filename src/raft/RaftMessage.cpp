
//
// Created by XingfengYang on 2020/1/1.
//
#include "../../include/raft/RaftMessage.h"
#include "../../include/raft/RaftMessageImpl.h"
#include <map>
#include <string>

namespace {
    std::shared_ptr<Raft::RaftMessage> CreateBaseMessage() {
        return std::make_shared<Raft::RaftMessage>();
    }
}


namespace Raft {
    RaftMessage::~RaftMessage() noexcept = default;

    RaftMessage::RaftMessage(Raft::RaftMessage &&) noexcept = default;

    RaftMessage &RaftMessage::operator=(RaftMessage &&) noexcept = default;


    std::function<std::shared_ptr<RaftMessage>()> RaftMessage::CreateMessage = CreateBaseMessage;

    RaftMessage::RaftMessage() : raftMessage(new RaftMessageImpl()) {

    }

    char* RaftMessage::getMessageType() const {
        return raftMessage->getMessageType();
    }
}
