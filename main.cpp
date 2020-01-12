//#include "include/Server.h"
#include "include/SocketOps.h"
#include "include/RaftMessage.h"
#include <unistd.h>
#include <iostream>
#include "include/Log.h"

#define TEST(text, func){ PrintColor4(" test case: %s [TESTING]\n",text); func(); PrintColor3(" test case: %s [Ok]\n",text)}

#define TEST_HTTP_RESPONSE "HTTP/1.1 200 OK\r\nServer: Raft \r\nContent-Type: text/html;charset=utf-8\r\n\r\n"\
"<h1>Raft Server Status:\n</h1>"\
"<p style='color:green;'>HTTP Server works.\n</p>"\


void handler(char *buffer, int fdc) {
    while (*buffer != '\0') {
        std::cout << *buffer;
        buffer++;
    }
    char *buf = TEST_HTTP_RESPONSE;
    write(fdc, buf, strlen(buf));
    close(fdc);
}

void should_encode_decode_raft_message() {
    auto *raftMessage = new Raft::RaftMessage();
    raftMessage->type = Raft::Type::RequestVote;
    raftMessage->requestVoteDetails.candidateId = 2;
    raftMessage->requestVoteDetails.term = 2;
    raftMessage->conntentLength = 1024;

    char *encodeMessage = raftMessage->EncodeMessage();
    assert(*encodeMessage != '\0');

    Raft::RaftMessage message = raftMessage->DecodeMessage(encodeMessage);
    assert(message.conntentLength == 1024);
    assert(message.type == Raft::Type::RequestVote);
    assert(message.requestVoteDetails.candidateId == 2);
    assert(message.requestVoteDetails.term == 2);
}

int main() {
    TEST("should_encode_raft_message", should_encode_decode_raft_message)
//    Raft::Server::Configuration configuration;
//    configuration.instancesNumbers = {2, 5, 6, 7, 11};
//    configuration.selfInstanceNumber = 2;
//    auto *server = new Raft::Server();
//    server->Configure(configuration);
//    server->Mobilize();

//    auto *socketOps = new Raft::SocketOps();
//    Raft::ISocket::Configuration configuration;
//    configuration.port = 8899;
//    socketOps->Configure(configuration);
//    socketOps->SetUp();
//    socketOps->Bind();
//    socketOps->Listen();
//    while (socketOps->running) {
//        socketOps->Accept(handler);
//    }

}
