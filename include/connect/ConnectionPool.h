
//
// Created by XingfengYang on 2020/2/5.
//

#ifndef RAFT_CONNECTIONPOOL_H
#define RAFT_CONNECTIONPOOL_H

#include "../config/Config.h"
#include <map>
#include <netinet/in.h>

namespace Connect {
    struct Connection {
        sockaddr_in sockaddrIn;
        int socketFd;
    };

    class ConnectionPool {
    public:
        ~ConnectionPool() noexcept;

        ConnectionPool(const ConnectionPool &) = delete;

        ConnectionPool(ConnectionPool &&) noexcept;

        ConnectionPool &operator=(const ConnectionPool &) = delete;

        ConnectionPool &operator=(ConnectionPool &&) noexcept;

    public:
        ConnectionPool();

        Connection* GetConnection(EndPoint endPoint);

        void AddConnection(EndPoint endPoint, int socketFd);

    private:
        std::map<EndPoint, Connection*> connections;
    };
}

#endif //RAFT_CONNECTIONPOOL_H
