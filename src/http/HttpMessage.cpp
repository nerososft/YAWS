
//
// Created by XingfengYang on 2020/1/1.
//
#include "../../include/http/HttpMessage.h"

namespace {
    std::shared_ptr<Http::HttpMessage> CreateBaseMessage() {
        return std::make_shared<Http::HttpMessage>();
    }
}


namespace Http {
    HttpMessage::~HttpMessage() noexcept = default;

    HttpMessage::HttpMessage(Http::HttpMessage &&) noexcept = default;

    HttpMessage &HttpMessage::operator=(HttpMessage &&) noexcept = default;


    std::function<std::shared_ptr<HttpMessage>()> HttpMessage::CreateMessage = CreateBaseMessage;

    HttpMessage::HttpMessage() : httpMessage(new HttpMessageImpl()) {

    }
}
