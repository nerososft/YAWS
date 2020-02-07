//
// Created by XingfengYang on 2020/1/1.
//

#ifndef HTTP_MESSAGEIMPL_H
#define HTTP_MESSAGEIMPL_H

#include <memory>
#include <map>
#include <string>

namespace Raft {
    enum HttpResponseStatus {
        CONTINUE = 100,
        SWITCHING_PROTOCOLS = 101,
        PROCESSING = 102,
        CHECKPOINT = 103,
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NON_AUTHORITATIVE_INFORMATION = 203,
        NO_CONTENT = 204,
        RESET_CONTENT = 205,
        PARTIAL_CONTENT = 206,
        MULTI_STATUS = 207,
        ALREADY_REPORTED = 208,
        IM_USED = 226,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        TEMPORARY_REDIRECT = 307,
        PERMANENT_REDIRECT = 308,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        PAYMENT_REQUIRED = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PROXY_AUTHENTICATION_REQUIRED = 407,
        REQUEST_TIMEOUT = 408,
        CONFLICT = 409,
        GONE = 410,
        LENGTH_REQUIRED = 411,
        PRECONDITION_FAILED = 412,
        PAYLOAD_TOO_LARGE = 413,
        URI_TOO_LONG = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        REQUESTED_RANGE_NOT_SATISFIABLE = 416,
        EXPECTATION_FAILED = 417,
        I_AM_A_TEAPOT = 418,
        UNPROCESSABLE_ENTITY = 422,
        LOCKED = 423,
        FAILED_DEPENDENCY = 424,
        UPGRADE_REQUIRED = 426,
        PRECONDITION_REQUIRED = 428,
        TOO_MANY_REQUESTS = 429,
        REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
        UNAVAILABLE_FOR_LEGAL_REASONS = 451,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505,
        VARIANT_ALSO_NEGOTIATES = 506,
        INSUFFICIENT_STORAGE = 507,
        LOOP_DETECTED = 508,
        BANDWIDTH_LIMIT_EXCEEDED = 509,
        NOT_EXTENDED = 510,
        NETWORK_AUTHENTICATION_REQUIRED = 511,
    };

    class HttpMessageImpl {
    public:
        ~HttpMessageImpl() noexcept;

        HttpMessageImpl(const HttpMessageImpl &) = delete;

        HttpMessageImpl(HttpMessageImpl &&) noexcept;

        HttpMessageImpl &operator=(const HttpMessageImpl &) = delete;

        HttpMessageImpl &operator=(HttpMessageImpl &&) noexcept;

        std::map<std::string, std::string> httpRequestHeader;
    private:
        void WriteMem(char *mem, uint32_t offset, char value);

        char ReadMem(char *mem, uint32_t offset);

        uint32_t ReadMemU32(char *mem, uint32_t offset);

    public:
        std::string EncodeMessage(HttpResponseStatus responseStatus,std::string responseBody);

        std::shared_ptr<HttpMessageImpl> DecodeMessage(char *buf);


    public:
        HttpMessageImpl();

    private:
        void SetProtocolPayload(const char *buf, char *baseLine, char *header, char *content, int processPhase) const;

        std::map<std::string, std::string> ParseHeader(const char *buf);

    private:


    };
}

#endif //HTTP_MESSAGEIMPL_H
