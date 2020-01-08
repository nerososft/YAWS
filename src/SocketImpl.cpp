//
// Created by XingfengYang on 2020/1/7.
//

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <sys/event.h>
#include "../include/SocketImpl.h"

namespace Raft {

    SocketImpl::~SocketImpl() noexcept = default;

    SocketImpl::SocketImpl(Raft::SocketImpl &&) noexcept = default;

    SocketImpl &SocketImpl::operator=(SocketImpl &&) noexcept = default;

    SocketImpl::SocketImpl() : fd(-1), kq(-1), addr({}) {

    }

    bool SocketImpl::Configure(const Configuration &configuration) {
        this->configuration = configuration;
    }

    int SocketImpl::SetUp() {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(configuration.port);
        addr.sin_addr.s_addr = INADDR_ANY;
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            std::cerr << "Unable to Open the Socket" << std::endl;
        }

        kq = kqueue();
        if (kq == -1) {
            std::cerr << "Unable to init kqueue" << std::endl;
        }
    }

    int SocketImpl::Bind() {
        if (bind(fd, (struct sockaddr *) (&addr), sizeof(addr)) != 0) {
            std::cerr << "Unable to bind the Socket" << std::endl;
        }
    }

    int SocketImpl::Listen() {
        if (listen(fd, 50) == -1) {
            std::cerr << "Unable to listen the Socket" << std::endl;
        }
    }

    void SocketImpl::SetSocketAcceptEventHandler(SocketAcceptEventHandler socketAcceptEventHandler) {
        socketAcceptEventHandler = socketAcceptEventHandler;
    }

    int SocketImpl::Accept(SocketAcceptEventHandler socketAcceptEventHandler) {
        sockaddr client_addr;
        unsigned int nLength;
        int fdc = accept(fd, &client_addr, &nLength);
        if (fdc == -1) {
            std::cerr << "Unable to Connect with the client" << std::endl;
        } else {
            char *request = new char[1000];
            memset(request, 0, 1000);
            read(fdc, request, 1000);

            socketAcceptEventHandler(request, fdc);

            close(fdc);
            delete[] request;
        }
    }
}
