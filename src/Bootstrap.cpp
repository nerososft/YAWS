//
// Created by XingfengYang on 2020/1/12.
//
#include "../include/Bootstrap.h"
#include <unistd.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <iterator>

#include "../include/Log.h"

namespace Raft {

    RaftBootstrap::~RaftBootstrap() noexcept = default;

    RaftBootstrap::RaftBootstrap(Raft::RaftBootstrap &&) noexcept = default;

    RaftBootstrap &RaftBootstrap::operator=(RaftBootstrap &&) noexcept = default;

    RaftBootstrap::RaftBootstrap() :
            server(std::make_shared<Server>()),
            socketOps(std::make_shared<SocketOps>()),
            timeKeeper(std::make_shared<TimeKeeper>()) {}


    template<class Container>
    void split(const std::string &str, Container &cont, char delim = ' ') {
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delim)) {
            cont.push_back(token);
        }
    }

    void RaftBootstrap::LoadConfigFile() {
        std::ifstream is("config/raft.conf", std::ifstream::binary);
        if (is.is_open()) {
            std::string line;
            while (getline(is, line)) {
                if (line[0] != '#' && line.length() != 0) {
                    std::vector<std::string> configs;
                    split(line, configs, '=');
                    std::cout << configs[0] << ":" << configs[1] << std::endl;
                    // TODO:  nodes to mem
                }
            }
            is.close();
        } else {
            PrintColor2("Config file laod failed. %s \n", "config/raft.conf")
        }
    }

#define TEST_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>Raft Server Status:\n</h1>"\
"<p style='color:green;'>HTTP Server works.\n</p>"

#define DAILED_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>ooops:\n</h1>"\
"<p style='color:red;'>Not A Raft Message.\n</p>"

    void handler(char *buffer, int fdc) {
        try {
            auto *raftMessage = new RaftMessage();
            raftMessage->DecodeMessage(buffer);
            char *buf = TEST_HTTP_RESPONSE;
            write(fdc, buf, strlen(buf));
        } catch (std::logic_error error) {
            PrintColor2("Caught %s\n", error.what())
            char *buf = DAILED_HTTP_RESPONSE;
            write(fdc, buf, strlen(buf));
        }
        close(fdc);
    }

    void RaftBootstrap::Run() {
        PrintSplash();
        LoadConfigFile();

        Raft::Server::Configuration configuration;
        configuration.instancesNumbers = {2, 5, 6, 7, 11};
        configuration.selfInstanceNumber = 2;
        this->server->Configure(configuration);
        this->server->SetTimeKeeper(this->timeKeeper);

        // sleep to accept heart beat
        server->Mobilize();

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

    void RaftBootstrap::PrintSplash() {
        NormalPrintColor2(" __ __    ___  ____    ____  _____  ______ \n")
        NormalPrintColor3("|  |  |  /  _]|    \\  /    ||     ||      |\n")
        NormalPrintColor4("|  |  | /  [_ |  D  )|  o  ||   __||      |\n")
        NormalPrintColor5("|  ~  ||    _]|    / |     ||  |_  |_|  |_|\n")
        NormalPrintColor6("|___, ||   [_ |    \\ |  _  ||   _]   |  |  \n")
        NormalPrintColor7("|     ||     ||  .  \\|  |  ||  |     |  |  \n")
        NormalPrintColor1("|____/ |_____||__|\\_||__|__||__|     |__|  \n")
        NormalPrintColor1("\n")
    }

}
