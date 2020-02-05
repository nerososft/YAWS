//
// Created by XingfengYang on 2020/2/5.
//
#include "../include/ConnectionPool.h"


namespace Raft {
    ConnectionPool::~ConnectionPool() noexcept = default;

    ConnectionPool::ConnectionPool(Raft::ConnectionPool &&) noexcept = default;

    ConnectionPool &ConnectionPool::operator=(ConnectionPool &&) noexcept = default;

    ConnectionPool::ConnectionPool() {
    }

    Connection *ConnectionPool::GetConnection(int nodeId) {
        if (connections.count(nodeId)) {
            return this->connections.find(nodeId)->second;
        }
        return nullptr;
    }


    void ConnectionPool::AddConnection(int nodeId, char *host, int port, int socketFd) {
        auto *pConnection = new Connection();
        EndPoint endPoint{};
        endPoint.host = host;
        endPoint.port = port;
        pConnection->endPoint = endPoint;
        pConnection->socketFd = socketFd;
        this->connections.insert(std::pair<int, Connection *>(nodeId, pConnection));
    }

}


