//
// Created by XingfengYang on 2020/2/5.
//
#include "../../include/connect/ConnectionPool.h"
#include "../../include/log/Log.h"


namespace Connect {
    ConnectionPool::~ConnectionPool() noexcept = default;

    ConnectionPool::ConnectionPool(Connect::ConnectionPool &&) noexcept = default;

    ConnectionPool &ConnectionPool::operator=(ConnectionPool &&) noexcept = default;

    ConnectionPool::ConnectionPool() {

    }

    Connection *ConnectionPool::GetConnection(EndPoint endPoint) {
        if (connections.count(endPoint)) {
            return connections.find(endPoint)->second;
        }
        return nullptr;
    }


    void ConnectionPool::AddConnection(EndPoint endPoint, int socketFd) {
        Connection *connection = new Connection();
        connection->socketFd = socketFd;
        this->connections.insert(
                std::pair<EndPoint, Connection *>({endPoint.host, endPoint.port}, connection)
        );
    }

}


