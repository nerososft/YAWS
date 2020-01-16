
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
    }


    void HttpMessageImpl::WriteMem(char *mem, uint32_t offset, char value) {
        *(mem + offset) = value;
    }

    char HttpMessageImpl::ReadMem(char *mem, uint32_t offset) {
        return *(mem + offset);
    }

    uint32_t HttpMessageImpl::ReadMemU32(char *mem, uint32_t offset) {
        return (
                ((uint32_t) this->ReadMem(mem, offset)) << 24 |
                ((uint32_t) this->ReadMem(mem, offset + 1)) << 16 |
                ((uint32_t) this->ReadMem(mem, offset + 2)) << 8 |
                ((uint32_t) this->ReadMem(mem, offset + 3))
        );
    }
}
