//
// Created by XingfengYang on 2020/1/7.
//

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <sys/event.h>
#include <fcntl.h>
#include "../include/SocketImpl.h"

namespace Raft {

    SocketImpl::~SocketImpl() noexcept = default;

    SocketImpl::SocketImpl(Raft::SocketImpl &&) noexcept = default;

    SocketImpl &SocketImpl::operator=(SocketImpl &&) noexcept = default;

    SocketImpl::SocketImpl() : fd(-1), kq(-1), addr({}) {

    }

    bool SocketImpl::Configure(const SocketConfiguration &config) {
        this->configuration = config;
    }

    int SocketImpl::SetUp() {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(configuration.port);
        addr.sin_addr.s_addr = INADDR_ANY;
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            std::cerr << "Unable to Open the Socket" << std::endl;
            exit(0);
        }

        kq = kqueue();
        if (kq == -1) {
            std::cerr << "Unable to init kqueue" << std::endl;
            exit(0);
        }

        EV_SET(&eventSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &eventSet, 1, NULL, 0, NULL) == -1) {
            std::cerr << "Unable to init kevent" << std::endl;
            exit(0);
        }

    }

    int SocketImpl::Bind() {
        if (bind(fd, (struct sockaddr *) (&addr), sizeof(addr)) != 0) {
            std::cerr << "Unable to bind the Socket" << std::endl;
            exit(0);
        }
    }

    int SocketImpl::Listen() {
        if (listen(fd, 50) == -1) {
            std::cerr << "Unable to listen the Socket" << std::endl;
            exit(0);
        }
    }

    void SocketImpl::SetSocketAcceptEventHandler(SocketAcceptEventHandler acceptEventHandler) {
        this->socketAcceptEventHandler = acceptEventHandler;
    }

    int SocketImpl::Accept(SocketAcceptEventHandler acceptEventHandler) {
        int nev = kevent(kq, NULL, 0, eventList, 32, NULL);
        struct sockaddr_storage sockaddrStorage{};
        socklen_t socklen = sizeof(sockaddrStorage);

        for (int i = 0; i < nev; i++) {
            int fd_ = (int) eventList[i].ident;
            if (eventList[i].flags & EV_EOF) {
                close(fd_);
            } else if (fd_ == fd) {
                int connectedFd = accept(fd, (struct sockaddr *) &sockaddrStorage, &socklen);
                if (connectedFd == -1) {
                    std::cerr << "Unable to Connect with the client" << std::endl;
                } else {
                    EV_SET(&eventSet, connectedFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    kevent(kq, &eventSet, 1, NULL, 0, NULL);
                    printf("Got connection!\n");

                    int flags = fcntl(connectedFd, F_GETFL, 0);
                    assert(flags >= 0);
                    fcntl(connectedFd, F_SETFL, flags | O_NONBLOCK);

                    EV_SET(&eventSet, connectedFd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                    kevent(kq, &eventSet, 1, NULL, 0, NULL);
                }
            } else if (eventList[i].filter == EVFILT_READ) {
                char buf[1024];
                size_t bytes_read = recv(eventList[i].ident, buf, sizeof(buf), 0);
                acceptEventHandler(buf, eventList[i].ident);
            } else if (eventList[i].filter == EVFILT_WRITE) {

            }
        }
    }
}
