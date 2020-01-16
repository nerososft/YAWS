//
// Created by XingfengYang on 2020/1/1.
//

#ifndef HTTP_MESSAGEIMPL_H
#define HTTP_MESSAGEIMPL_H

#include <memory>

namespace Raft {
    class HttpMessageImpl {
    public:
        ~HttpMessageImpl() noexcept;

        HttpMessageImpl(const HttpMessageImpl &) = delete;

        HttpMessageImpl(HttpMessageImpl &&) noexcept;

        HttpMessageImpl &operator=(const HttpMessageImpl &) = delete;

        HttpMessageImpl &operator=(HttpMessageImpl &&) noexcept;

    private:
        void WriteMem(char *mem, uint32_t offset, char value);

        char ReadMem(char *mem, uint32_t offset);

        uint32_t ReadMemU32(char *mem, uint32_t offset);

    public:
        char *EncodeMessage();

        HttpMessageImpl DecodeMessage(char *buf);


    public:
        HttpMessageImpl();

    private:
        void SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const;
    };
}

#endif //HTTP_MESSAGEIMPL_H
