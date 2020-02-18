//
// Created by XingfengYang on 2020/2/17.
//
#include "../../include/json/Json.h"
#include "../../include/log/Log.h"

#include <cmath>

namespace Serialization {

    enum class Type {
        INVALID,
        Null,
        Boolean,
        String,
        Integer,
        Float,
    };

    struct Json::Impl {
        Type type = Type::INVALID;

        union {
            bool booleanValue;
            std::string *strValue;
            int intValue;
            double floatValue;
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

    Json::Json(int value) : impl(new Impl) {
        impl->type = Type::Integer;
        impl->intValue = value;
    }

    Json::Json(double value) : impl(new Impl) {
        impl->type = Type::Float;
        impl->floatValue = value;
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
                case Type::Integer:
                    return impl->intValue == other.impl->intValue;
                case Type::Float:
                    return impl->floatValue == other.impl->floatValue;
                default:
                    return true;
            }
        }
    }

    Json Json::FromString(const std::string &fromStr) {
        if (fromStr.empty()) {
            return Json();
        } else if (fromStr[0] == '{') {
            return Json(); // TODO : parse Object
        } else if (fromStr[0] == '[') {
            return Json(); // TODO : parse Array
        } else if (fromStr[0] == '"' && fromStr[fromStr.length() - 1] == '"') {
            return Common::UnEscape(fromStr.substr(1, fromStr.length() - 2));
        } else if (fromStr == "null") {
            return nullptr;
        } else if (fromStr == "true") {
            return true;
        } else if (fromStr == "false") {
            return false;
        } else {
            if (fromStr.find_first_of("+.eE") != std::string::npos) {
                return ParseFloat(fromStr);
            } else {
                return ParseInteger(fromStr);
            }
        }
    }

    Json Json::ParseFloat(const std::string &str) {
        return Json();
    }

    Json Json::ParseInteger(const std::string &str) {
        enum ParseState {
            PARSE_MINUS = 0,
            PARSE_NUMBER = 1,
            PARSE_ZERO = 2,
            PARSE_DIGIT = 3,
        };

        bool negative = false;
        int value = 0;

        ParseState parseState = ParseState::PARSE_MINUS;
        size_t index = 0;
        while (index < str.length()) {
            switch (parseState) {
                case PARSE_MINUS:
                    if (str[index] == '-') {
                        negative = true;
                        ++index;
                    }
                    parseState = ParseState::PARSE_NUMBER;
                    break;
                case PARSE_NUMBER:
                    if (str[index] == '0') {
                        parseState = ParseState::PARSE_ZERO;
                    } else if (str[index] >= '1' && str[index] <= '9') {
                        parseState = ParseState::PARSE_DIGIT;
                        value = (int) (str[index] - '0');
                    } else {
                        return Json();
                    }
                    ++index;
                    break;
                case PARSE_ZERO:
                    return Json();
                case PARSE_DIGIT:
                    if (str[index] >= '0' && str[index] <= '9') {
                        value *= 10;
                        value += (int) (str[index] - '0');
                        ++index;
                    } else {
                        return Json();
                    }
                    break;
                default:
                    break;
            }
        }

        if (parseState < ParseState::PARSE_NUMBER) {
            return Json();
        } else {
            return Json(value);
        }
    }

    std::string Json::ToString(const Common::EncodingOptions &options) const {
        switch (impl->type) {
            case Type::Null:
                return "null";
            case Type::Boolean:
                return impl->booleanValue ? "true" : "false";
            case Type::String:
                return "\"" + Common::Escape(*impl->strValue, options) + "\"";
            case Type::Integer:
                return std::to_string(impl->intValue);
            case Type::Float:
                return std::to_string(impl->floatValue);
            default:
                return "???";
        }
    }

    Json::operator bool() const {
        if (this->impl->type == Type::Boolean) {
            return this->impl->booleanValue;
        }
        return false;
    }

    Json::operator std::string() const {
        if (this->impl->type == Type::String) {
            return *this->impl->strValue;
        }
        return "";
    }

    Json::operator int() const {
        if (this->impl->type == Type::Integer) {
            return this->impl->intValue;
        } else if (this->impl->type == Type::Float) {
            return (int) this->impl->floatValue;
        }
        return 0;
    }

    Json::operator double() const {
        if (this->impl->type == Type::Float) {
            return this->impl->floatValue;
        } else if (this->impl->type == Type::Integer) {
            return (double) this->impl->intValue;
        }
        return 0;
    }
}
