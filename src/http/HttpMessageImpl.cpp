
//
// Created by XingfengYang on 2020/1/1.
//

#include "../../include/http/HttpMessageImpl.h"
#include <memory>
#include <map>

namespace Http {

    static std::string responseReasonMap[] = {
            [CONTINUE]="Continue",
            [SWITCHING_PROTOCOLS]="Switching Protocols",
            [PROCESSING]="",
            [CHECKPOINT]="",
            [OK]="OK",
            [CREATED]="Created",
            [ACCEPTED]="Accepted",
            [NON_AUTHORITATIVE_INFORMATION]="",
            [NO_CONTENT]="",
            [RESET_CONTENT]="",
            [PARTIAL_CONTENT]="",
            [MULTI_STATUS]="",
            [ALREADY_REPORTED]="",
            [IM_USED]="",
            [MULTIPLE_CHOICES]="",
            [MOVED_PERMANENTLY]="",
            [FOUND]="",
            [SEE_OTHER]="",
            [NOT_MODIFIED]="",
            [TEMPORARY_REDIRECT]="",
            [PERMANENT_REDIRECT]="",
            [BAD_REQUEST]="",
            [UNAUTHORIZED]="",
            [PAYMENT_REQUIRED]="",
            [FORBIDDEN]="",
            [NOT_FOUND]="Not Found",
            [METHOD_NOT_ALLOWED]="",
            [NOT_ACCEPTABLE]="",
            [PROXY_AUTHENTICATION_REQUIRED]="",
            [REQUEST_TIMEOUT]="",
            [CONFLICT]="Conflict",
            [GONE]="",
            [LENGTH_REQUIRED]="",
            [PRECONDITION_FAILED]="",
            [PAYLOAD_TOO_LARGE]="",
            [URI_TOO_LONG]="",
            [UNSUPPORTED_MEDIA_TYPE]="",
            [REQUESTED_RANGE_NOT_SATISFIABLE]="",
            [EXPECTATION_FAILED]="",
            [I_AM_A_TEAPOT]="",
            [UNPROCESSABLE_ENTITY]="",
            [LOCKED]="",
            [FAILED_DEPENDENCY]="",
            [UPGRADE_REQUIRED]="",
            [PRECONDITION_REQUIRED]="",
            [TOO_MANY_REQUESTS]="",
            [REQUEST_HEADER_FIELDS_TOO_LARGE]="",
            [UNAVAILABLE_FOR_LEGAL_REASONS]="",
            [INTERNAL_SERVER_ERROR]="",
            [NOT_IMPLEMENTED]="",
            [BAD_GATEWAY]="",
            [SERVICE_UNAVAILABLE]="",
            [GATEWAY_TIMEOUT]="",
            [HTTP_VERSION_NOT_SUPPORTED]="",
            [VARIANT_ALSO_NEGOTIATES]="",
            [INSUFFICIENT_STORAGE]="",
            [LOOP_DETECTED]="",
            [BANDWIDTH_LIMIT_EXCEEDED]="",
            [NOT_EXTENDED]="",
            [NETWORK_AUTHENTICATION_REQUIRED]="",
    };

    HttpMessageImpl::~HttpMessageImpl() noexcept = default;

    HttpMessageImpl::HttpMessageImpl(Http::HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl &HttpMessageImpl::operator=(HttpMessageImpl &&) noexcept = default;

    HttpMessageImpl::HttpMessageImpl() {
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("GET", GET));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("HEAD", HEAD));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("POST", POST));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("PUT", PUT));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("DELETE", DELETE));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("CONNECT", CONNECT));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("OPTIONS", OPTIONS));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("TRACE", TRACE));
        httpMethodMap.insert(std::pair<std::string, HttpMethod>("PATCH", PATCH));
    }

    std::string HttpMessageImpl::EncodeMessage(HttpResponseStatus responseStatus, const std::string &responseBody) {
        std::string response;

        // statusLine
        response.append("HTTP/1.1 ");
        response.append(std::to_string((int) responseStatus));
        response.append(" " + responseReasonMap[responseStatus] + " ");

        response.append("\r\n");
        // response header
        for (auto it = httpRequestHeader.begin(); it != httpRequestHeader.end(); ++it) {
            response.append(it->first);
            response.append(":");
            response.append(it->second);
            response.append("\r\n");
        }
        response.append("\r\n");
        response.append(responseBody);
        return response;
    }

    /**
       * GET /resource HTTP/1.1\n\n
       * header1: value1\r\n
       * header2: value2\r\n
       * ...
       * headern: valuen\r\n
       * \r\n
       * content
       */
    std::shared_ptr<HttpMessageImpl> HttpMessageImpl::DecodeMessage(char *buf) {

        std::string request(buf);
        std::map<std::string, std::string> httpRequestHeader;

        HttpRequest httpRequest;

        /**
         * PROCESS_METHOD->PROCESS_RESOURCE->PROCESS_PROTOCOL->PROCESS_HEADER_NAME->PROCESS_HEADER_VALUE->PROCESS_CONTENT
         *                                                    |                                          |
         *                                                     <-----------------------------------------
         */
        enum ProcessState {
            PROCESS_METHOD,
            PROCESS_RESOURCE,
            PROCESS_PROTOCOL,
            PROCESS_HEADER_NAME,
            PROCESS_HEADER_VALUE,
            PROCESS_CONTENT
        };

        ProcessState processState = ProcessState::PROCESS_METHOD;

        int index = 0;
        std::string buffer;
        size_t length = request.size();
        auto consume = [&]() -> char {
            assert(index < length);
            return buf[index++];
        };

        auto peek = [&](uint32_t offset) -> char {
            assert(index + offset < length);
            return buf[index + offset];
        };

        auto transformState = [&](ProcessState nextState) {
            buffer.clear();
            processState = nextState;
        };

        std::string currentHeaderName;
        while (index < length) {
            char currentChr = consume();
            switch (processState) {
                case PROCESS_METHOD:
                    if (currentChr == ' ') {
                        httpRequest.httpMethod = httpMethodMap.find(buffer)->second;
                        transformState(PROCESS_RESOURCE);
                    } else {
                        buffer.push_back(currentChr);
                    }
                    break;
                case PROCESS_RESOURCE:
                    if (currentChr == ' ') {
                        httpRequest.uri = buffer;
                        transformState(PROCESS_PROTOCOL);
                    } else {
                        buffer.push_back(currentChr);
                    }
                    break;
                case PROCESS_PROTOCOL:
                    if (currentChr == '\r' && consume() == '\n') {
                        httpRequest.protocol = buffer;
                        transformState(PROCESS_HEADER_NAME);
                    } else {
                        buffer.push_back(currentChr);
                    }
                    break;
                case PROCESS_HEADER_NAME:
                    if (currentChr == ':' && consume() == ' ') {
                        currentHeaderName = buffer;
                        transformState(PROCESS_HEADER_VALUE);
                    } else {
                        buffer.push_back(currentChr);
                    }
                    break;
                case PROCESS_HEADER_VALUE:
                    if (currentChr == '\r' && consume() == '\n') {
                        httpRequest.header.insert(std::pair<std::string, std::string>(currentHeaderName, buffer));
                        buffer.clear();
                        if (peek(0) == '\r' && peek(1) == '\n') {
                            consume();
                            consume();
                            processState = PROCESS_CONTENT;
                        } else {
                            processState = PROCESS_HEADER_NAME;
                        }
                    } else {
                        buffer.push_back(currentChr);
                    }
                    break;
                case PROCESS_CONTENT:
                    buffer.push_back(currentChr);
                    if (index == length) {
                        httpRequest.body = buffer;
                    }
                    break;
            }
        }


        std::shared_ptr<HttpMessageImpl> httpMessage = std::make_shared<HttpMessageImpl>();
        httpMessage->request = httpRequest;
        return httpMessage;
    }

    void HttpMessageImpl::SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const {
    }

    std::string HttpMessageImpl::methodToString(HttpMethod method) {
        switch (method) {
            case GET:
                return "GET";
            case HEAD:
                return "HEAD";
            case POST:
                return "POST";
            case PUT:
                return "PUT";
            case DELETE:
                return "DELETE";
            case CONNECT:
                return "CONNECT";
            case OPTIONS:
                return "OPTIONS";
            case TRACE:
                return "TRACE";
            case PATCH:
                return "PATCH";
        }
    }

}
