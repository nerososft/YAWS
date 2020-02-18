#include <sstream>
#include "include/Bootstrap.h"
#include "include/raft/RaftMessageImpl.h"
#include "include/log/Log.h"
#include "include/templateEngine/Loader.h"
#include "include/templateEngine/TemplateEngine.h"
#include "include/json/Json.h"

#define TEST(text, func){ PrintColor4(" [Test]: %s [TESTING]\n",text); func(); PrintColor3(" [Test]: %s [Ok]\n",text)}
#define ASSERT_EQ(left, right){ assert(left==right); }
#define ASSERT_NEQ(left, right){ assert(left!=right); }
#define ASSERT_TRUE(value){ assert(value); }
#define ASSERT_FALSE(value){ assert(!value); }
#define ASSERT_EQ_FLOAT(left, right) {assert(abs(left-right)<0.0001);}


void should_encode_json() {
    Serialization::Json jsonNull(nullptr);
    ASSERT_EQ(jsonNull.ToString(), "null")

    Serialization::Json jsonTrue(true);
    ASSERT_EQ(jsonTrue.ToString(), "true");
    ASSERT_TRUE(Serialization::Json(true));

    Serialization::Json jsonFalse(false);
    ASSERT_EQ(jsonFalse.ToString(), "false");
    ASSERT_FALSE(Serialization::Json(false));

    Serialization::Json jsonStr(std::string("hello, world"));
    ASSERT_EQ(jsonStr.ToString(), "\"hello, world\"");
    Serialization::Json jsonStr1("hello, world");
    ASSERT_EQ(jsonStr1.ToString(), "\"hello, world\"");

    Serialization::Json jsonInteger(10086);
    ASSERT_EQ(jsonInteger.ToString(), "10086");

    Serialization::Json jsonInteger1(-10086);
    ASSERT_EQ(jsonInteger1.ToString(), "-10086");

    Serialization::Json jsonFloat(3.1415926);
//    ASSERT_EQ(jsonFloat.ToString(), "3.1415926");
}

void should_decode_json() {
    Serialization::Json jsonNull;
    ASSERT_EQ(jsonNull.FromString("null"), nullptr);

    Serialization::Json jsonTrue;
    ASSERT_EQ((bool) jsonTrue.FromString("true"), true);

    Serialization::Json jsonFalse;
    ASSERT_EQ((bool) jsonFalse.FromString("false"), false);

    Serialization::Json jsonStr;
    ASSERT_EQ(jsonStr.FromString("\"hello, world\""), std::string("hello, world"));

    Serialization::Json jsonInteger;
    ASSERT_EQ((int) jsonInteger.FromString("10086"), 10086);

    Serialization::Json jsonInteger1;
    ASSERT_EQ((int) jsonInteger1.FromString("-10086"), -10086);

    Serialization::Json jsonFloat;
    ASSERT_EQ_FLOAT((double) jsonFloat.FromString("3.1415926"), 3.1415926);

    Serialization::Json jsonFloat1;
    ASSERT_EQ_FLOAT((double) jsonFloat1.FromString("-3.1415926"), -3.1415926);

    Serialization::Json jsonFloat3;
    ASSERT_EQ_FLOAT((double) jsonFloat3.FromString("0.7"), 0.7);

    Serialization::Json jsonFloat2;
    ASSERT_EQ_FLOAT((double) jsonFloat2.FromString("3.14e-4"), 3.14e-4);

    Serialization::Json jsonArray;
    const Serialization::Json &json = jsonArray.FromString("[1,\"hello\",true]");
    ASSERT_EQ(json.GetType(), Serialization::Type::Array);
    ASSERT_EQ(json.GetSize(), 3);
    ASSERT_EQ(json[0]->GetType(), Serialization::Type::Integer);
    ASSERT_EQ((int) *json[0], 1);
    ASSERT_EQ(json[1]->GetType(), Serialization::Type::String);
    ASSERT_EQ((std::string) *json[1], "hello");
    ASSERT_EQ(json[2]->GetType(), Serialization::Type::Boolean);
    ASSERT_EQ((bool) *json[2], true);
}

void should_encode_decode_raft_message() {
    auto *raftMessage = new Raft::RaftMessageImpl();
    raftMessage->type = Raft::Type::RequestVote;
    raftMessage->requestVoteDetails.candidateId = 2;
    raftMessage->requestVoteDetails.term = 2;
    raftMessage->nodeId = 10086;
    raftMessage->contentLength = 1024;

    char *encodeMessage = raftMessage->EncodeMessage();
    ASSERT_NEQ(*encodeMessage, '\0');

    std::shared_ptr<Raft::RaftMessageImpl> message = raftMessage->DecodeMessage(encodeMessage);
    ASSERT_EQ(message->type, Raft::Type::RequestVote);
    ASSERT_EQ(message->requestVoteDetails.candidateId, 2);
    ASSERT_EQ(message->requestVoteDetails.term, 2);
    LogError("%d\n", message->nodeId)
    ASSERT_EQ(message->nodeId, 10086);
    ASSERT_EQ(message->contentLength, 1024);
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
                "Connection: keep-alive\r\n\r\n"
                "xxx";

    std::shared_ptr<Http::HttpMessageImpl> message = httpMessage->DecodeMessage(msg);

    ASSERT_EQ(message->request.httpMethod, Http::GET);
    ASSERT_EQ(message->request.uri, "/");
    ASSERT_EQ(message->request.protocol, "HTTP/1.1");
    ASSERT_EQ(message->request.header["Host"], "192.241.213.46:6880");
    ASSERT_EQ(message->request.header["Upgrade-Insecure-Requests"], "1");
    ASSERT_EQ(message->request.header["Accept"], "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    ASSERT_EQ(message->request.header["User-Agent"], "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_3) AppleWebKit/602.4.8 (KHTML, like Gecko) Version/10.0.3 Safari/602.4.8");
    ASSERT_EQ(message->request.header["Accept-Language"], "en-us");
    ASSERT_EQ(message->request.header["Accept-Encoding"], "gzip, deflate");
    ASSERT_EQ(message->request.header["Connection"], "keep-alive");
    ASSERT_EQ(message->request.body, "xxx");

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

    ASSERT_EQ(buf.str(), "<html xmlns=\"http://www.w3.org/1999/html\" xmlns=\"http://www.w3.org/1999/html\" lang=\"html\">\n"
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

    ASSERT_EQ(buf.str(), "<div>my title</div>\n"
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

    ASSERT_EQ(buf.str(), "<html xmlns=\"http://www.w3.org/1999/html\" xmlns=\"http://www.w3.org/1999/html\" lang=\"html\">\n"
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


int main(int argc, char *argv[]) {
    TEST("should_encode_json", should_encode_json)
    TEST("should_decode_json", should_decode_json)
//    TEST("should_encode_raft_message", should_encode_decode_raft_message)
    TEST("should_decode_http_message_header", should_decode_http_message_header)
    TEST("should_render_html_template", should_render_html_template)
    TEST("should_render_multi_html_template", should_render_multi_html_template)
    TEST("should_render_multi_html_file_template", should_render_multi_html_file_template)
    auto *bootstrap = new Bootstrap::RaftBootstrap();
    bootstrap->Run(argc, argv);
}
