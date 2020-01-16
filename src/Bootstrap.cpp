//
// Created by XingfengYang on 2020/1/12.
//
#include "../include/Bootstrap.h"
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
            raftServer(std::make_shared<RaftServer>()),
            httpServer(std::make_shared<HttpServer>()),
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

    void RaftBootstrap::Run() {
        PrintSplash();
        LoadConfigFile();

        Raft::RaftServer::Configuration configuration;
        configuration.instancesNumbers = {2, 5, 6, 7, 11};
        configuration.selfInstanceNumber = 2;
        configuration.socketConfiguration.port = 8898;
        this->raftServer->Configure(configuration);
        this->raftServer->SetTimeKeeper(this->timeKeeper);
        this->raftServer->SetRunning();
        this->raftServer->Mobilize();

        Raft::IHttpServer::Configuration httpServerConfiguration;
        httpServerConfiguration.socketConfiguration.port = 8899;
        this->httpServer->Configure(httpServerConfiguration);
        this->httpServer->SetTimeKeeper(this->timeKeeper);
        this->httpServer->SetRunning();
        this->httpServer->Mobilize();
        while (isRunning) {}
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
