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
    enum class Type {
        INVALID,
        Null,
        Boolean,
        String,
        Integer,
        Float,
        Array,
    };

    class Json {
    public:
        ~Json() noexcept;

        Json(Json &&) noexcept;

        Json &operator=(const Json &) = default;

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

        Json(std::vector<std::shared_ptr<Json>> arrayValue);

        bool operator==(const Json &other);

        std::string ToString(const Common::EncodingOptions &options = Common::EncodingOptions()) const;

        Json FromString(const std::string &fromStr);

        Type GetType() const;

        size_t GetSize() const;

        std::shared_ptr<Json> operator[](size_t index) const;

    private:
        struct Impl;

        std::unique_ptr<struct Impl> impl;

        Json ParseFloat(const std::string &str);

        Json ParseInteger(const std::string &str);

        Json ParseArray(const std::string &str);

        std::string ParseValueStr(const std::string &str, size_t &offset);
    };
}


#endif //RAFT_JSON_H
