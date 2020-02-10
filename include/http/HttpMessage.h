//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_HTTPMESSAGE_H
#define RAFT_HTTPMESSAGE_H

#include <memory>
#include <functional>
#include "HttpMessageImpl.h"

namespace Http {

    class HttpMessageImpl;

    class HttpMessage {
    public:
        ~HttpMessage() noexcept;

        HttpMessage(const HttpMessage &) = delete;

        HttpMessage(HttpMessage &&) noexcept;

        HttpMessage &operator=(const HttpMessage &) = delete;

        HttpMessage &operator=(HttpMessage &&) noexcept;

    public:
        static std::function<std::shared_ptr<HttpMessage>()> CreateMessage;

    public:
        HttpMessage();

    public:
        std::shared_ptr<HttpMessageImpl> httpMessage;
    };
}
#endif //RAFT_HTTPMESSAGE_H
