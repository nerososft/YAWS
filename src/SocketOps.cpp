//
// Created by XingfengYang on 2020/1/7.
//

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include "../include/SocketOps.h"

namespace Raft {

    SocketOps::~SocketOps() noexcept = default;

    SocketOps::SocketOps(Raft::SocketOps &&) noexcept = default;

    SocketOps &SocketOps::operator=(SocketOps &&) noexcept = default;

    SocketOps::SocketOps() : impl(new SocketOps()), fd(-1) {

    }

    bool SocketOps::Configure(const Configuration &configuration) {
        impl->configuration = configuration;
    }

    int SocketOps::SetUp() {
        memset(&impl->addr, 0, sizeof(impl->addr));
        impl->addr.sin_family = AF_INET;
        impl->addr.sin_port = htons(impl->configuration.port);
        impl->addr.sin_addr.s_addr = INADDR_ANY;
        if ((impl->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            std::cerr << "Unable to Open the Socket" << std::endl;
        }
    }

    int SocketOps::Bind() {
        if (bind(impl->fd, (struct sockaddr *) (&impl->addr), sizeof(impl->addr)) != 0) {
            std::cerr << "Unable to bind the Socket" << std::endl;
        }
    }

    int SocketOps::Listen() {
        if (listen(impl->fd, 50) == -1) {
            std::cerr << "Unable to listen the Socket" << std::endl;
        }
    }

    void SocketOps::SetSocketAcceptEventHandler(SocketAcceptEventHandler socketAcceptEventHandler) {
        impl->socketAcceptEventHandler = socketAcceptEventHandler;
    }

    int SocketOps::Accept(SocketAcceptEventHandler socketAcceptEventHandler) {
        sockaddr client_addr;
        unsigned int nLength;
        int fdc = accept(fd, &client_addr, &nLength);
        if (fdc == -1) {
            std::cerr << "Unable to Connect with the client" << std::endl;
        } else {
            char *request = new char[1000];
            memset(request, 0, 1000);
            read(fdc, request, 1000);

            impl->socketAcceptEventHandler(request);

            close(fdc);
            delete[] request;
        }
    }
}
