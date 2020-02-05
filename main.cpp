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

    std::shared_ptr<Raft::RaftMessageImpl> message = raftMessage->DecodeMessage(encodeMessage);
    assert(message->conntentLength == 1024);
    assert(message->type == Raft::Type::RequestVote);
    assert(message->requestVoteDetails.candidateId == 2);
    assert(message->requestVoteDetails.term == 2);
}

void should_decode_http_message_header() {
    auto *httpMessage = new Raft::HttpMessageImpl();

    char *msg = "GET / HTTP/1.1\r\n"
                "Host: 192.241.213.46:6880\r\n"
                "Upgrade-Insecure-Requests: 1\r\n"
                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/602.4.8 (KHTML, like Gecko) Version/10.0.3 Safari/602.4.8\r\n"
                "Accept-Language: en-us\r\n"
                "Accept-Encoding: gzip, deflate\r\n"
                "Connection: keep-alive\r\n\r\n";

    Raft::HttpMessageImpl message = httpMessage->DecodeMessage(msg);

    assert(message.httpRequestHeader["Type"] == "GET");
    assert(message.httpRequestHeader["Path"] == "/");
    assert(message.httpRequestHeader["Version"] == "HTTP/1.1");
    assert(message.httpRequestHeader["Host"] == "192.241.213.46:6880");
    assert(message.httpRequestHeader["Upgrade-Insecure-Requests"] == "1");
    assert(message.httpRequestHeader["Accept"] == "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    assert(message.httpRequestHeader["User-Agent"] == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/602.4.8 (KHTML, like Gecko) Version/10.0.3 Safari/602.4.8");
    assert(message.httpRequestHeader["Accept-Language"] == "en-us");
    assert(message.httpRequestHeader["Accept-Encoding"] == "gzip, deflate");
    assert(message.httpRequestHeader["Connection"] == "keep-alive");
}

int main() {
    TEST("should_encode_raft_message", should_encode_decode_raft_message)
    TEST("should_decode_http_message_header", should_decode_http_message_header)

    auto *bootstrap = new Raft::RaftBootstrap();
    bootstrap->Run();
}
