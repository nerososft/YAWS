//
// Created by XingfengYang on 2020/1/1.
//

#ifndef HTTP_MESSAGEIMPL_H
#define HTTP_MESSAGEIMPL_H

#include <memory>
#include <map>
#include <string>

namespace Raft {
    class HttpMessageImpl {
    public:
        ~HttpMessageImpl() noexcept;

        HttpMessageImpl(const HttpMessageImpl &) = delete;

        HttpMessageImpl(HttpMessageImpl &&) noexcept;

        HttpMessageImpl &operator=(const HttpMessageImpl &) = delete;

        HttpMessageImpl &operator=(HttpMessageImpl &&) noexcept;

        std::map<std::string, std::string> httpRequestHeader;
    private:
        void WriteMem(char *mem, uint32_t offset, char value);

        char ReadMem(char *mem, uint32_t offset);

        uint32_t ReadMemU32(char *mem, uint32_t offset);

    public:
        char *EncodeMessage();

        std::shared_ptr<HttpMessageImpl> DecodeMessage(char *buf);


    public:
        HttpMessageImpl();

    private:
        void SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const;

        std::map<std::string, std::string> ParseHeader(const char *buf);

    private:


    };
}

#endif //HTTP_MESSAGEIMPL_H
