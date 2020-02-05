
//
// Created by XingfengYang on 2020/2/5.
//

#ifndef RAFT_CONNECTIONPOOL_H
#define RAFT_CONNECTIONPOOL_H

#include "Config.h"
#include <map>
#include <netinet/in.h>

namespace Raft {
    struct Connection {
        EndPoint endPoint;
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

        Connection *GetConnection(int nodeId);

        void AddConnection(int nodeId, char *host, int port, int socketFd);

    private:
        std::map<int, Connection *> connections;
    };
}

#endif //RAFT_CONNECTIONPOOL_H
