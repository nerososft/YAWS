//
// Created by XingfengYang on 2020/1/2.
//
#include "../include/IHttpServer.h"
#include "../include/HttpServerImpl.h"

namespace Raft {
    HttpServerImpl::~HttpServerImpl() noexcept = default;

    HttpServerImpl::HttpServerImpl(Raft::HttpServerImpl &&) noexcept = default;

    HttpServerImpl &HttpServerImpl::operator=(HttpServerImpl &&) noexcept = default;

    HttpServerImpl::HttpServerImpl() {

    }

    bool HttpServerImpl::Configure(const IHttpServer::Configuration &configuration){

    }

    void HttpServerImpl::SetSendMessageDelegate(IHttpServer::SendMessageDelegate sendMessageDelegate){

    }

    void HttpServerImpl::ReceiveMessage(std::shared_ptr<HttpMessage> message,
                                unsigned int senderInstanceNumber){

    }

}
