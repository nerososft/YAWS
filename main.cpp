#include <sstream>
#include "include/Bootstrap.h"
#include "include/raft/RaftMessageImpl.h"
#include "include/log/Log.h"
#include "include/templateEngine/Loader.h"
#include "include/templateEngine/TemplateEngine.h"

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
    auto *httpMessage = new Http::HttpMessageImpl();

    char *msg = "GET / HTTP/1.1\r\n"
                "Host: 192.241.213.46:6880\r\n"
                "Upgrade-Insecure-Requests: 1\r\n"
                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/602.4.8 (KHTML, like Gecko) Version/10.0.3 Safari/602.4.8\r\n"
                "Accept-Language: en-us\r\n"
                "Accept-Encoding: gzip, deflate\r\n"
                "Connection: keep-alive\r\n\r\n";

    std::shared_ptr<Http::HttpMessageImpl> message = httpMessage->DecodeMessage(msg);

    assert(message->httpRequestHeader["Type"] == "GET");
    assert(message->httpRequestHeader["Path"] == "/");
    assert(message->httpRequestHeader["Version"] == "HTTP/1.1");
    assert(message->httpRequestHeader["Host"] == "192.241.213.46:6880");
    assert(message->httpRequestHeader["Upgrade-Insecure-Requests"] == "1");
    assert(message->httpRequestHeader["Accept"] == "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    assert(message->httpRequestHeader["User-Agent"] == "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/602.4.8 (KHTML, like Gecko) Version/10.0.3 Safari/602.4.8");
    assert(message->httpRequestHeader["Accept-Language"] == "en-us");
    assert(message->httpRequestHeader["Accept-Encoding"] == "gzip, deflate");
    assert(message->httpRequestHeader["Connection"] == "keep-alive");
}

void should_render_html_template() {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);
    templateEngine.Load("www/test/test.html");
    templateEngine.Set("text", "Hello, world");
    templateEngine.SetBlock("items").Repeat(3);

    const char *title[3] = {"title1", "title2", "title3"};
    const char *detail[3] = {"detail1", "detail2", "detail3"};

    for (int i = 0; i < 3; i++) {
        templateEngine.SetBlock("items")[i].Set("title", title[i]);
        templateEngine.SetBlock("items")[i].Set("text", "Lorem Ipsum");
        Template::Block &block = templateEngine.SetBlock("items")[i].SetBlock("detailBlock");
        block.Set("detail", detail[i]);

        if (i == 0) {
            block.Disable();
        }
    }

    std::stringbuf buf;
    std::ostream sout(&buf);
    templateEngine.Render(sout);

    assert(buf.str() == "<html xmlns=\"http://www.w3.org/1999/html\" xmlns=\"http://www.w3.org/1999/html\" lang=\"html\">\n"
                        "<body>\n"
                        "<p>Items:</p>\n"
                        "\n"
                        "<div>\n"
                        "    <h4>Title: title1</h4>\n"
                        "    <p>Text: Lorem Ipsum</p>\n"
                        "    \n"
                        "</div>\n"
                        "\n"
                        "<div>\n"
                        "    <h4>Title: title2</h4>\n"
                        "    <p>Text: Lorem Ipsum</p>\n"
                        "    \n"
                        "    <div>\n"
                        "        <p>Detail: detail2</p>\n"
                        "    </div>\n"
                        "    \n"
                        "</div>\n"
                        "\n"
                        "<div>\n"
                        "    <h4>Title: title3</h4>\n"
                        "    <p>Text: Lorem Ipsum</p>\n"
                        "    \n"
                        "    <div>\n"
                        "        <p>Detail: detail3</p>\n"
                        "    </div>\n"
                        "    \n"
                        "</div>\n"
                        "\n"
                        "</body>\n"
                        "</html>\n");
}

void should_render_multi_html_template() {
    Loader::MemoryLoader memoryLoader;
    Template::TemplateEngine templateEngine(memoryLoader);

    memoryLoader.Add("header", "<div>{{ title }}</div>\n{% include body %}\n<p>Hello, World</p>");
    memoryLoader.Add("body", "<p>Hi,I am {{ name }}</p>");


    templateEngine.Load("header");
    templateEngine.Set("title", "my title");
    templateEngine.Set("name", "nero yang");

    std::stringbuf buf;
    std::ostream sout(&buf);
    templateEngine.Render(sout);

    assert(buf.str() == "<div>my title</div>\n"
                        "<p>Hi,I am nero yang</p>\n"
                        "<p>Hello, World</p>");
}

void should_render_multi_html_file_template() {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);
    templateEngine.Load("www/test/test1.html");
    templateEngine.Set("text", "Hello, world");
    templateEngine.Set("name", "nero yang");
    templateEngine.SetBlock("items").Repeat(3);

    const char *title[3] = {"title1", "title2", "title3"};
    const char *detail[3] = {"detail1", "detail2", "detail3"};

    for (int i = 0; i < 3; i++) {
        templateEngine.SetBlock("items")[i].Set("title", title[i]);
        templateEngine.SetBlock("items")[i].Set("text", "Lorem Ipsum");
        Template::Block &block = templateEngine.SetBlock("items")[i].SetBlock("detailBlock");
        block.Set("detail", detail[i]);

        if (i == 0) {
            block.Disable();
        }
    }

    std::stringbuf buf;
    std::ostream sout(&buf);
    templateEngine.Render(sout);

    assert(buf.str() == "<html xmlns=\"http://www.w3.org/1999/html\" xmlns=\"http://www.w3.org/1999/html\" lang=\"html\">\n"
                        "<body>\n"
                        "<p>Items:</p>\n"
                        "\n"
                        "<div>\n"
                        "    <h4>Title: title1</h4>\n"
                        "    <p>Text: Lorem Ipsum</p>\n"
                        "    \n"
                        "</div>\n"
                        "\n"
                        "<div>\n"
                        "    <h4>Title: title2</h4>\n"
                        "    <p>Text: Lorem Ipsum</p>\n"
                        "    \n"
                        "    <div>\n"
                        "        <p>Detail: detail2</p>\n"
                        "    </div>\n"
                        "    \n"
                        "</div>\n"
                        "\n"
                        "<div>\n"
                        "    <h4>Title: title3</h4>\n"
                        "    <p>Text: Lorem Ipsum</p>\n"
                        "    \n"
                        "    <div>\n"
                        "        <p>Detail: detail3</p>\n"
                        "    </div>\n"
                        "    \n"
                        "</div>\n"
                        "\n"
                        "<p>Hi,I am nero yang</p>\n"
                        "\n"
                        "</body>\n"
                        "</html>\n");
}


int main() {
    TEST("should_encode_raft_message", should_encode_decode_raft_message)
    TEST("should_decode_http_message_header", should_decode_http_message_header)
    TEST("should_render_html_template", should_render_html_template)
    TEST("should_render_multi_html_template", should_render_multi_html_template)
    TEST("should_render_multi_html_file_template", should_render_multi_html_file_template)

    auto *bootstrap = new Bootstrap::RaftBootstrap();
    bootstrap->Run();
}
