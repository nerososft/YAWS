//#include "include/Server.h"
#include "include/SocketOps.h"
#include <unistd.h>

#define TEST_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n<h1>Raft Server Works</h1>"
void handler(char *buffer, int fdc) {
    char *buf = TEST_HTTP_RESPONSE;
    write(fdc, buf, strlen(buf));
}

int main() {
//    Raft::Server::Configuration configuration;
//    configuration.instancesNumbers = {2, 5, 6, 7, 11};
//    configuration.selfInstanceNumber = 2;
//
//    auto *server = new Raft::Server();
//    server->Configure(configuration);

//    server->Mobilize();

//    server->Listen();

    Raft::SocketOps *socketOps = new Raft::SocketOps();
    Raft::ISocket::Configuration configuration;
    configuration.port = 8899;
    socketOps->Configure(configuration);
    socketOps->SetUp();
    socketOps->Bind();
    socketOps->Listen();
    socketOps->Accept(handler);


    return 0;
}
