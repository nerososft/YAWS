//
// Created by XingfengYang on 2020/2/5.
//
#include "../include/ConnectionPool.h"


namespace Raft {
    ConnectionPool::~ConnectionPool() noexcept = default;

    ConnectionPool::ConnectionPool(Raft::ConnectionPool &&) noexcept = default;

    ConnectionPool &ConnectionPool::operator=(ConnectionPool &&) noexcept = default;

    ConnectionPool::ConnectionPool(){

    }

    Connection *ConnectionPool::GetConnection(EndPoint endPoint) {
        if (connections.count(endPoint)) {
            return this->connections.find(endPoint)->second;
        }
        return nullptr;
    }


    void ConnectionPool::AddConnection(EndPoint endPoint, int socketFd) {
        auto *pConnection = new Connection();
        pConnection->socketFd = socketFd;
        this->connections.insert(std::pair<EndPoint, Connection *>(endPoint, pConnection));
    }

}


