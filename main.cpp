#include <iostream>
#include "include/Server.h"

int main() {
    Raft::Server::Configuration configuration;
    configuration.instancesNumbers = {2, 5, 6, 7, 11};
    configuration.selfInstanceNumber = 2;

    auto *server = new Raft::Server();
    server->Configure(configuration);

//    server->Mobilize();

    server->Listen();
    return 0;
}
