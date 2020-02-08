
//
// Created by XingfengYang on 2020/1/1.
//

#include "../../include/http/HttpMessageImpl.h"
#include <memory>
#include <iostream>
#include <map>

#define CR '\r'
#define LF '\n'
#define CR_LF '\r\n'
static const char SPACE = ' ';
static const char *const SYMBOL_COLON = ":";

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

    HttpMessageImpl::HttpMessageImpl() {}

    std::string HttpMessageImpl::EncodeMessage(HttpResponseStatus responseStatus, std::string responseBody) {
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

    std::shared_ptr<HttpMessageImpl> HttpMessageImpl::DecodeMessage(char *buf) {
        std::shared_ptr<HttpMessageImpl> httpMessage = std::make_shared<HttpMessageImpl>();
        httpMessage->httpRequestHeader = this->ParseHeader(buf);
        return httpMessage;
    }

    std::map<std::string, std::string> HttpMessageImpl::ParseHeader(const char *msg) {
        std::map<std::string, std::string> httpRequestHeader;

        char *head = (char *) msg;
        char *mid;
        char *tail = head;

        // Find request type
        while (*head++ != SPACE);
        httpRequestHeader["Type"] = std::string((char *) msg).substr(0, (head - 1) - tail);

        // Find path
        tail = head;
        while (*head++ != SPACE);
        httpRequestHeader["Path"] = std::string((char *) msg).substr(tail - (char *) msg, (head - 1) - tail);

        // Find HTTP version
        tail = head;
        while (*head++ != CR);
        httpRequestHeader["Version"] = std::string((char *) msg).substr(tail - (char *) msg, (head - 1) - tail);

        // Map all headers from a key to a value
        while (true) {
            tail = head + 1;
            while (*head++ != CR);
            mid = strstr(tail, SYMBOL_COLON);

            // Look for the failed strstr
            if (tail > mid)
                break;

            httpRequestHeader[std::string((char *) msg).substr(tail - (char *) msg, (mid) - tail)] = std::string((char *) msg).substr(mid + 2 - (char *) msg, (head - 3) - mid);
        }

        return httpRequestHeader;
    }

    void HttpMessageImpl::SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const {
    }

    void HttpMessageImpl::WriteMem(char *mem, uint32_t offset, char value) {
        *(mem + offset) = value;
    }

    char HttpMessageImpl::ReadMem(char *mem, uint32_t offset) {
        return *(mem + offset);
    }

    uint32_t HttpMessageImpl::ReadMemU32(char *mem, uint32_t offset) {
        return (
                ((uint32_t)
                        this->ReadMem(mem, offset)) << 24 |
                ((uint32_t)
                        this->ReadMem(mem, offset + 1)) << 16 |
                ((uint32_t)
                        this->ReadMem(mem, offset + 2)) << 8 |
                ((uint32_t)
                        this->ReadMem(mem, offset + 3))
        );
    }
}
