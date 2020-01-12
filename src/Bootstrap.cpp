//
// Created by XingfengYang on 2020/1/12.
//
#include "../include/Bootstrap.h"
#include <unistd.h>
#include <iostream>

namespace Raft {

    RaftBootstrap::~RaftBootstrap() noexcept = default;

    RaftBootstrap::RaftBootstrap(Raft::RaftBootstrap &&) noexcept = default;

    RaftBootstrap &RaftBootstrap::operator=(RaftBootstrap &&) noexcept = default;

    RaftBootstrap::RaftBootstrap() : server(std::make_shared<Server>()), socketOps(std::make_shared<SocketOps>()) {}


    void RaftBootstrap::LoadConfigFile() {

    }


#define TEST_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>Raft Server Status:\n</h1>"\
"<p style='color:green;'>HTTP Server works.\n</p>"

    void handler(char *buffer, int fdc) {
        while (*buffer != '\0') {
            std::cout << *buffer;
            buffer++;
        }

        char *buf = TEST_HTTP_RESPONSE;
        write(fdc, buf, strlen(buf));
        close(fdc);
    }

    void RaftBootstrap::Run() {
        // sleep to accept heart beat
        Raft::Server::Configuration configuration;
        configuration.instancesNumbers = {2, 5, 6, 7, 11};
        configuration.selfInstanceNumber = 2;
        this->server->Configure(configuration);
        // server->Mobilize();

        Raft::ISocket::Configuration socketConfig;
        socketConfig.port = 8899;
        this->socketOps->Configure(socketConfig);
        this->socketOps->SetUp();
        this->socketOps->Bind();
        this->socketOps->Listen();
        while (isRunning) {
            this->socketOps->Accept(handler);
        }
    }

}
