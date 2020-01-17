
//
// Created by XingfengYang on 2020/1/1.
//

#include "../include/HttpMessageImpl.h"
#include <memory>
#include <iostream>
#include <vector>

namespace Raft {
    HttpMessageImpl::~HttpMessageImpl() noexcept = default;

    HttpMessageImpl::HttpMessageImpl(Raft::HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl &HttpMessageImpl::operator=(HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl::HttpMessageImpl() {}


    char *HttpMessageImpl::EncodeMessage() {

    }

    HttpMessageImpl HttpMessageImpl::DecodeMessage(char *buf) {
        int len = 0;
        while (*buf) {
            if (*buf == '\r') {
                if (*(buf + 1) == '\n') {
                    if (*(buf + 2) == '\r') {
                        if (*(buf + 3) == '\n') {
                            break;
                        }
                    }
                }
            }
            len++;
            buf++;
        }
        char* header = (char*)malloc(len*sizeof(char));
        buf-=len;
        memcpy(header,buf,len);
        buf+=len;
        char* body = buf+4;

        std::vector<char *> lines;
        int headerLen = 0;
        while (*header) {
            if (*header == '\r') {
                if (*(header + 1) == '\n') {
                    char *line = (char *) malloc(headerLen * sizeof(char));
                    for (int i = 0; i < headerLen; i++) {
                        std::cout << *(header - (headerLen - i));
                        line[i] = *(header - (headerLen - i));
                    }
                    lines.push_back(line);
                    headerLen = 0;
                }
            }
            header++;
            headerLen++;
        }
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
