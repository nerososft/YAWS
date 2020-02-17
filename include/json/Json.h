//
// Created by XingfengYang on 2020/2/17.
// https://tools.ietf.org/html/rfc7159
//

#ifndef RAFT_JSON_H
#define RAFT_JSON_H

#include <memory>
#include <string>

namespace Serialization {
    class Json {
    public:
        ~Json() noexcept;

        Json(const Json &) = delete;

        Json(Json &&) noexcept;

        Json &operator=(const Json &) = delete;

        Json &operator=(Json &&) noexcept;

    public:
        Json();

        Json(std::nullptr_t);

        Json(bool value);

        Json(const char* value);

        Json(const std::string &value);

        bool operator==(const Json &other);

        std::string ToString() const;

        static Json FromString(const std::string &fromStr);

    private:
        struct Impl;

        std::unique_ptr<struct Impl> impl;
    };
}


#endif //RAFT_JSON_H
