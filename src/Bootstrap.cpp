//
// Created by XingfengYang on 2020/1/12.
//
#include "../include/Bootstrap.h"
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <ostream>

#include "../include/Log.h"
#include "../include/Common.h"

namespace Raft {

    RaftBootstrap::~RaftBootstrap() noexcept = default;

    RaftBootstrap::RaftBootstrap(Raft::RaftBootstrap &&) noexcept = default;

    RaftBootstrap &RaftBootstrap::operator=(RaftBootstrap &&) noexcept = default;

    RaftBootstrap::RaftBootstrap() :
            raftServer(std::make_shared<RaftServer>()),
            httpServer(std::make_shared<HttpServer>()),
            timeKeeper(std::make_shared<TimeKeeper>()) {}

    void RaftBootstrap::LoadConfigFile() {
        std::ifstream is("config/raft.conf", std::ifstream::binary);
        if (is.is_open()) {
            std::string line;
            while (getline(is, line)) {
                if (line[0] != '#' && line.length() != 0) {
                    std::vector<std::string> configs;
                    Common::split(line, configs, '=');

                    if (strcmp(Common::trim(configs[0]).c_str(), "id") == 0) {
                        config.id = configs[1];
                    }
                    if (strcmp(Common::trim(configs[0]).c_str(), "raft_port") == 0) {
                        char *pEnd;
                        config.raftPort = (int) std::strtol(configs[1].c_str(), &pEnd, 10);
                    }
                    if (strcmp(Common::trim(configs[0]).c_str(), "http_port") == 0) {
                        char *pEnd;
                        config.httpPort = (int) std::strtol(configs[1].c_str(), &pEnd, 10);
                    }

                    // add nodes to mem
                    if (strcmp(Common::trim(configs[0]).c_str(), "nodes") == 0) {
                        // transform host port pair to int number[nodeId]. use Hash
                        unsigned int nodeId = Common::GetHashCode(configs[1]);

                        std::vector<std::string> host;
                        Common::split(configs[1], host, ':');
                        LogWarnning("[Config]: %s-%s:%s, nodeId: %d\n", configs[0].c_str(), host[0].c_str(), host[1].c_str(), nodeId)

                        // add nodes to memory config
                        EndPoint endPoint{};
                        endPoint.host = host[0];
                        char *pEnd;
                        endPoint.port = (unsigned int) std::strtol(host[1].c_str(), &pEnd, 10);
                        config.endpoints.insert(std::pair<unsigned int, EndPoint>(nodeId, endPoint));
                    }
                }
            }
            std::cout << config << std::endl;
            is.close();
        } else {
            LogError("[Config] Config file laod failed. %s \n", "config/raft.conf")
        }
    }

    void RaftBootstrap::Run() {
        PrintSplash();

        LoadConfigFile();

        Raft::RaftServer::Configuration configuration;
        configuration.instancesNumbers = {2, 5, 6, 7, 11};
        configuration.selfInstanceNumber = 2;
        configuration.socketConfiguration.port = this->config.raftPort;
        this->raftServer->Configure(configuration);
        this->raftServer->SetTimeKeeper(this->timeKeeper);
        this->raftServer->SetRunning();
        this->raftServer->Mobilize();

        Raft::IHttpServer::Configuration httpServerConfiguration;
        httpServerConfiguration.socketConfiguration.port = this->config.httpPort;
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
