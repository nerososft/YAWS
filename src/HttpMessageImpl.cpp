
//
// Created by XingfengYang on 2020/1/1.
//

#include "../include/HttpMessageImpl.h"
#include <memory>
#include <iostream>
#include <map>

#define CR '\r'
#define LF '\n'
#define CR_LF '\r\n'

namespace Raft {
    HttpMessageImpl::~HttpMessageImpl() noexcept = default;

    HttpMessageImpl::HttpMessageImpl(Raft::HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl &HttpMessageImpl::operator=(HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl::HttpMessageImpl() {}


    char *HttpMessageImpl::EncodeMessage() {

    }

    HttpMessageImpl HttpMessageImpl::DecodeMessage(char *buf) {
        HttpMessageImpl httpMessage;
        httpMessage.httpRequestHeader = this->ParseHeader(buf);
        return httpMessage;
    }

    std::map<std::string, std::string> HttpMessageImpl::ParseHeader(const char *msg) {
        std::map<std::string, std::string> httpRequestHeader;

        char *head = (char *) msg;
        char *mid;
        char *tail = head;

        // Find request type
        while (*head++ != ' ');
        httpRequestHeader["Type"] = std::string((char *) msg).substr(0, (head - 1) - tail);

        // Find path
        tail = head;
        while (*head++ != ' ');
        httpRequestHeader["Path"] = std::string((char *) msg).substr(tail - (char *) msg, (head - 1) - tail);

        // Find HTTP version
        tail = head;
        while (*head++ != '\r');
        httpRequestHeader["Version"] = std::string((char *) msg).substr(tail - (char *) msg, (head - 1) - tail);

        // Map all headers from a key to a value
        while (true) {
            tail = head + 1;
            while (*head++ != '\r');
            mid = strstr(tail, ":");

            // Look for the failed strstr
            if (tail > mid)
                break;

            httpRequestHeader[std::string((char *) msg).substr(tail - (char *) msg, (mid) - tail)] = std::string((char *) msg).substr(mid + 2 - (char *) msg, (head - 3) - mid);
        }

        return httpRequestHeader;
    }

    void HttpMessageImpl::SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const {
    }


    void HttpMessageImpl::WriteMem(char *mem, uint32_t offset, char value) {
        *(mem + offset) = value;
    }

    char HttpMessageImpl::ReadMem(char *mem, uint32_t offset) {
        return *(mem + offset);
    }

    uint32_t HttpMessageImpl::ReadMemU32(char *mem, uint32_t offset) {
        return (
                ((uint32_t)
                        this->ReadMem(mem, offset)) << 24 |
                ((uint32_t)
                        this->ReadMem(mem, offset + 1)) << 16 |
                ((uint32_t)
                        this->ReadMem(mem, offset + 2)) << 8 |
                ((uint32_t)
                        this->ReadMem(mem, offset + 3))
        );
    }

}
