//
// Created by XingfengYang on 2020/1/12.
//
#include "../include/Bootstrap.h"
#include <iostream>
#include <memory>
#include <fstream>
#include <iterator>
#include <map>
#include <ostream>

#include "../include/log/Log.h"
#include "../include/common/Common.h"
#include "../include/app/Dashboard.h"

namespace Bootstrap {

    RaftBootstrap::~RaftBootstrap() noexcept = default;

    RaftBootstrap::RaftBootstrap(Bootstrap::RaftBootstrap &&) noexcept = default;

    RaftBootstrap &RaftBootstrap::operator=(RaftBootstrap &&) noexcept = default;

    RaftBootstrap::RaftBootstrap() :
            raftServer(std::make_shared<Raft::RaftServer>()),
            httpServer(std::make_shared<Http::HttpServer>()),
            timeKeeper(std::make_shared<Timer::TimeKeeper>()) {}

    void RaftBootstrap::LoadConfigFile() {

        const char *configFile = "config/raft.conf";

        if (args.size() >= 2) {
            configFile = args.at(1).c_str();
        }

        std::ifstream is(configFile, std::ifstream::binary);
        if (is.is_open()) {
            std::string line;
            while (getline(is, line)) {
                if (line[0] != '#' && line.length() != 0) {
                    std::vector<std::string> configs;
                    Common::Split(line, configs, '=');

                    if (strcmp(Common::Trim(configs[0]).c_str(), "id") == 0) {
                        config.id = configs[1];
                    }
                    if (strcmp(Common::Trim(configs[0]).c_str(), "raft_port") == 0) {
                        char *pEnd;
                        config.raftPort = (int) std::strtol(configs[1].c_str(), &pEnd, 10);
                    }
                    if (strcmp(Common::Trim(configs[0]).c_str(), "http_port") == 0) {
                        char *pEnd;
                        config.httpPort = (int) std::strtol(configs[1].c_str(), &pEnd, 10);
                    }

                    // add nodes to mem
                    if (strcmp(Common::Trim(configs[0]).c_str(), "nodes") == 0) {
                        // transform host port pair to int number[nodeId]. use Hash
                        unsigned int nodeId = Common::GetHashCode(configs[1]);

                        std::vector<std::string> host;
                        Common::Split(configs[1], host, ':');
                        LogWarnning("[Config]: %s-%s:%s, nodeId: %d\n", configs[0].c_str(), host[0].c_str(), host[1].c_str(), nodeId)

                        // add nodes to memory config
                        Connect::EndPoint endPoint{};
                        endPoint.host = Common::Trim(host[0]);
                        char *pEnd;
                        endPoint.port = (unsigned int) std::strtol(host[1].c_str(), &pEnd, 10);
                        config.endpoints.insert(std::pair<unsigned int, Connect::EndPoint>(nodeId, endPoint));
                    }
                }
            }
            is.close();
            LogInfo("[Config] Nodes loaded, size %lu\n", config.endpoints.size());
        } else {
            LogError("[Config] Config file laod failed. %s \n", configFile)
        }
    }

    void RaftBootstrap::Run(int argc, char *argv[]) {
        for (int i = 0; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }
        PrintSplash();

        LoadConfigFile();

//        BootstrapRaftServer();
        BootstrapHttpServer();
        while (isRunning) {}
    }

    void RaftBootstrap::BootstrapHttpServer() const {
        ConfigHttpServer();

        AttachApps();

        httpServer->Mobilize();
    }

    void RaftBootstrap::AttachApps() const {
        App::AppConfig appConfig;
        appConfig.AttachApps(httpServer);
    }

    void RaftBootstrap::ConfigHttpServer() const {
        Http::IHttpServer::Configuration httpServerConfiguration;
        httpServerConfiguration.socketConfiguration.port = config.httpPort;
        httpServer->Configure(httpServerConfiguration);
        httpServer->SetTimeKeeper(timeKeeper);
        httpServer->SetRunning();
    }

    void RaftBootstrap::BootstrapRaftServer() const {
        Raft::RaftServer::Configuration configuration;

        // nodeId set of endpoints
        for (auto it = this->config.endpoints.begin(); it != config.endpoints.end(); ++it) {
            configuration.instancesNumbers.push_back(it->first);
        }

        // should be hash from self ip and port
        // TODO : hash (ip:port) is not a good idea, because it means every node's config should use either WAN ip or LAN ip
        configuration.selfInstanceNumber = Common::GetHashCode("127.0.0.1:" + std::to_string(config.raftPort));

        configuration.socketConfiguration.port = config.raftPort;
        configuration.endPoints = config.endpoints;
        raftServer->Configure(configuration);
        raftServer->SetTimeKeeper(timeKeeper);
        raftServer->SetRunning();
        raftServer->Mobilize();
    }

    void RaftBootstrap::PrintSplash() {
        NormalPrintColor2(" __ __    ___  ____    ____  _____  ______  \n")
        NormalPrintColor3("|  |  |  /  _]|    \\  /    ||     ||      |\n")
        NormalPrintColor4("|  |  | /  [_ |  D  )|  o  ||   __||      | \n")
        NormalPrintColor5("|  ~  ||    _]|    / |     ||  |_  |_|  |_| \n")
        NormalPrintColor6("|___, ||   [_ |    \\ |  _  ||   _]   |  |  \n")
        NormalPrintColor7("|     ||     ||  .  \\|  |  ||  |     |  |  \n")
        NormalPrintColor1("|____/ |_____||__|\\_||__|__||__|     |__|  \n")
        NormalPrintColor1("\n")
    }
}
