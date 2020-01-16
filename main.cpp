#include "include/Bootstrap.h"
#include "include/RaftMessageImpl.h"
#include "include/Log.h"

#define TEST(text, func){ PrintColor4(" [Test]: %s [TESTING]\n",text); func(); PrintColor3(" [Test]: %s [Ok]\n",text)}


void should_encode_decode_raft_message() {
    auto *raftMessage = new Raft::RaftMessageImpl();
    raftMessage->type = Raft::Type::RequestVote;
    raftMessage->requestVoteDetails.candidateId = 2;
    raftMessage->requestVoteDetails.term = 2;
    raftMessage->conntentLength = 1024;

    char *encodeMessage = raftMessage->EncodeMessage();
    assert(*encodeMessage != '\0');

    Raft::RaftMessageImpl message = raftMessage->DecodeMessage(encodeMessage);
    assert(message.conntentLength == 1024);
    assert(message.type == Raft::Type::RequestVote);
    assert(message.requestVoteDetails.candidateId == 2);
    assert(message.requestVoteDetails.term == 2);
}

int main() {
    TEST("should_encode_raft_message", should_encode_decode_raft_message)

    auto *bootstrap = new Raft::RaftBootstrap();
    bootstrap->Run();
}
