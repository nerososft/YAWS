
//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/Message.h"
#include "../include/MessageImpl.h"

namespace {
    std::shared_ptr<Raft::Message> CreateBaseMessage() {
        return std::make_shared<Raft::Message>();
    }
}


namespace Raft {
    Message::~Message() noexcept = default;

    Message::Message(Raft::Message &&) noexcept = default;

    Message &Message::operator=(Message &&) noexcept = default;


    std::function<std::shared_ptr<Message>()> Message::CreateMessage = CreateBaseMessage;

    Message::Message() : impl(new MessageImpl()) {

    }
}
