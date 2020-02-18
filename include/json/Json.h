//
// Created by XingfengYang on 2020/2/17.
// https://tools.ietf.org/html/rfc7159
//

#ifndef RAFT_JSON_H
#define RAFT_JSON_H

#include <memory>
#include <string>
#include "../common/Common.h"

namespace Serialization {
    class Json {
    public:
        ~Json() noexcept;

        Json(const Json &) = delete;

        Json(Json &&) noexcept;

        Json &operator=(const Json &) = delete;

        Json &operator=(Json &&) noexcept;

        operator bool() const;

        operator std::string() const;

        operator int() const;

        operator double() const;

    public:
        Json();

        Json(std::nullptr_t);

        Json(bool value);

        Json(const char *value);

        Json(int value);

        Json(double value);

        Json(const std::string &value);

        bool operator==(const Json &other);

        std::string ToString(const Common::EncodingOptions &options = Common::EncodingOptions()) const;

        Json FromString(const std::string &fromStr);

    private:
        struct Impl;

        std::unique_ptr<struct Impl> impl;

        Json ParseFloat(const std::string &str);

        Json ParseInteger(const std::string &str);
    };
}


#endif //RAFT_JSON_H
