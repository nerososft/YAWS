
//
// Created by XingfengYang on 2020/1/1.
//
#include "../include/HttpMessage.h"

namespace {
    std::shared_ptr<Raft::HttpMessage> CreateBaseMessage() {
        return std::make_shared<Raft::HttpMessage>();
    }
}


namespace Raft {
    HttpMessage::~HttpMessage() noexcept = default;

    HttpMessage::HttpMessage(Raft::HttpMessage &&) noexcept = default;

    HttpMessage &HttpMessage::operator=(HttpMessage &&) noexcept = default;


    std::function<std::shared_ptr<HttpMessage>()> HttpMessage::CreateMessage = CreateBaseMessage;

    HttpMessage::HttpMessage() : httpMessage(new HttpMessageImpl()) {

    }
}
