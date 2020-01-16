
//
// Created by XingfengYang on 2020/1/1.
//

#include "../include/HttpMessageImpl.h"
#include <memory>
#include <iostream>

namespace Raft {
    HttpMessageImpl::~HttpMessageImpl() noexcept = default;

    HttpMessageImpl::HttpMessageImpl(Raft::HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl &HttpMessageImpl::operator=(HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl::HttpMessageImpl() {}


    char *HttpMessageImpl::EncodeMessage() {

    }

    HttpMessageImpl HttpMessageImpl::DecodeMessage(char *buf) {
        char *baseLine = (char *) malloc(512 * sizeof(char));
        char *header = (char *) malloc(512 * sizeof(char));
        char *content = (char *) malloc(512 * sizeof(char));
        int processPhase = 0;

        while (*buf) {
            if (*buf == '\r') {
                if (*(buf + 1) == '\n') {
                    if (*(buf + 2) == '\r') {
                        if (*(buf + 3) == '\n') {
                            // change type
                            std::cout << "change line" << std::endl;
                            processPhase++;
                        } else {
                            // wtf? but nothing happened
                            this->SetProtocolPayload(buf, baseLine, header, content, processPhase);
                        }
                    } else {
                        // just change line
                        this->SetProtocolPayload(buf, baseLine, header, content, processPhase);
                    }
                } else {
                    // nothing happened
                    this->SetProtocolPayload(buf, baseLine, header, content, processPhase);
                }
            } else {
                this->SetProtocolPayload(buf, baseLine, header, content, processPhase);
            }
        }
        buf++;
    }

    void HttpMessageImpl::SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const {
        if (processPhase == 0) {
            *baseLine = *buf;
            baseLine++;
        } else if (processPhase == 1) {
            *header = *buf;
            header++;
        } else {
            *content = *buf;
            content++;
        }
    }


    void HttpMessageImpl::WriteMem(char *mem, uint32_t offset, char value) {
        *(mem + offset) = value;
    }

    char HttpMessageImpl::ReadMem(char *mem, uint32_t offset) {
        return *(mem + offset);
    }

    uint32_t HttpMessageImpl::ReadMemU32(char *mem, uint32_t offset) {
        return (
                ((uint32_t)this->ReadMem(mem, offset)) << 24 |
                ((uint32_t)this->ReadMem(mem, offset + 1)) << 16 |
                ((uint32_t)this->ReadMem(mem, offset + 2)) << 8 |
                ((uint32_t)this->ReadMem(mem, offset + 3))
        );
    }

}
