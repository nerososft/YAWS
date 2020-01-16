
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
        int length = strlen(buf);
        char *newBuf = (char *) malloc((length + 5) * sizeof(char));
        memcpy(newBuf, buf, length);
        *(newBuf + length) = '\0';
        *(newBuf + length + 1) = '\0';
        *(newBuf + length + 2) = '\0';
        *(newBuf + length + 3) = '\0';
        *(newBuf + length + 4) = '\0';
        char *content = (char *) malloc(512 * sizeof(char));
        bool contentFound = false;

        std::vector<char *> lines;

        int len = 0;
        while (*newBuf != '\0') {
            if (contentFound) {
                *content = *newBuf;
                content++;
                newBuf++;
                continue;
            }
            std::cout << *newBuf;
            if (*newBuf == '\r') {
                if (*(newBuf + 1) == '\n') {
                    if (*(newBuf + 2) == '\r') {
                        if (*(newBuf + 3) == '\n') {
                            std::cout << "change line" << std::endl;
                            contentFound = true;
                            newBuf += 4;
                            continue;
                        }
                    } else {
                        char *line = (char *) malloc(len * sizeof(char));
                        std::cout << "HEADER:";
                        for (int i = 0; i < len; i++) {
                            std::cout << *(newBuf - (len - i));
                            line[i] = *(newBuf - (len - i));
                        }
                        lines.push_back(line);
                        len = 0;
                        newBuf += 2;
                        continue;
                    }
                }
            }
            len++;
            newBuf++;
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
