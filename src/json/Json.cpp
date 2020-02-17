//
// Created by XingfengYang on 2020/2/17.
//
#include "../../include/json/Json.h"

namespace Serialization {
    enum class Type {
        Invalid,
        Null,
        Boolean,
        String,
    };

    struct Json::Impl {
        Type type = Type::Invalid;

        union {
            bool booleanValue;
            std::string strValue;
        };


        ~Impl() {}

        Impl(Impl &&) = delete;

        Impl &operator=(const Impl &) = delete;

        Impl &operator=(Impl &&) = delete;

        Impl() {

        }
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

    Json::Json(std::string &value) : impl(new Impl) {
        impl->type = Type::String;
        impl->strValue = value;
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
        } else {
            return Json();
        }
    }
}
