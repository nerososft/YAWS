//
// Created by XingfengYang on 2020/2/17.
//
#include "../../include/json/Json.h"

#include <utility>
#include "../../include/common/Common.h"

namespace Serialization {
    enum class Type {
        Null,
        Boolean,
        String,
    };

    struct Json::Impl {
        Type type = Type::Null;

        union {
            bool booleanValue;
            std::string *strValue;
        };


        ~Impl() {
            switch (type) {
                case Type::String:
                    delete strValue;
                    break;
                default:
                    break;
            }
        }

        Impl(Impl &&) = delete;

        Impl &operator=(const Impl &) = delete;

        Impl &operator=(Impl &&) = delete;

        Impl() = default;
    };

    Json::~Json() noexcept = default;

    Json::Json(Json &&) noexcept = default;

    Json &Json::operator=(Json &&) noexcept = default;

    Json::Json() : impl(new Impl) {}

    Json::Json(std::nullptr_t) : impl(new Impl) {
        impl->type = Type::Null;
    }

    Json::Json(bool value) : impl(new Impl) {
        impl->type = Type::Boolean;
        impl->booleanValue = value;
    }

    Json::Json(const char *value) : impl(new Impl) {
        impl->type = Type::String;
        impl->strValue = new std::string(value);
    }

    Json::Json(const std::string &value) : impl(new Impl) {
        impl->type = Type::String;
        impl->strValue = new std::string(value);
    }

    bool Json::operator==(const Json &other) {
        if (impl->type != other.impl->type) {
            return false;
        } else {
            switch (impl->type) {
                case Type::Null:
                    return true;
                case Type::Boolean:
                    return impl->booleanValue == other.impl->booleanValue;
                case Type::String:
                    return *impl->strValue == *other.impl->strValue;
                default:
                    return true;
            }
        }
    }

    std::string Json::ToString() const {
        switch (impl->type) {
            case Type::Null:
                return "null";
            case Type::Boolean:
                return impl->booleanValue ? "true" : "false";
            case Type::String:
                return "\"" + Common::Escape(*impl->strValue) + "\"";
            default:
                return "???";
        }
    }

    Json Json::FromString(const std::string &fromStr) {
        if (fromStr == "null") {
            return nullptr;
        } else if (fromStr == "true") {
            return true;
        } else if (fromStr == "false") {
            return false;
        } else if (!fromStr.empty() && fromStr[0] == '"' && fromStr[fromStr.length() - 1] == '"') {
            return Common::UnEscape(fromStr.substr(1, fromStr.length() - 2));
        } else {
            return Json();
        }
    }
}
