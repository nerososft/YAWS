//
// Created by XingfengYang on 2020/2/17.
//
#include "../../include/json/Json.h"

#include <cmath>
#include <utility>
#include <vector>
#include <stack>

namespace Serialization {
    struct Json::Impl {
        Type type = Type::INVALID;

        union {
            bool booleanValue;
            std::string *strValue;
            int intValue;
            double floatValue;
            std::vector<std::shared_ptr<Json>> *arrayValue;
        };


        ~Impl() {
            switch (type) {
                case Type::String:
                    delete strValue;
                    break;

                case Type::Array:
                    delete arrayValue;
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

    Json::Json(std::vector<std::shared_ptr<Json>> arrayValue) : impl(new Impl) {
        impl->type = Type::Array;
        impl->arrayValue = new std::vector<std::shared_ptr<Json>>(std::move(arrayValue));
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
        } else if (fromStr[0] == '[' && fromStr[fromStr.length() - 1] == ']') {
            return ParseArray(fromStr.substr(1, fromStr.length() - 2));
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

    Json Json::ParseArray(const std::string &str) {
        std::vector<std::shared_ptr<Json>> newArrayValue;
        size_t offset = 0;
        while (offset < str.length()) {
            const auto valueStr = ParseValueStr(str, offset);
            if (offset == std::string::npos) {
                break;
            }
            newArrayValue.push_back(std::make_shared<Json>(FromString(valueStr)));
        }
        return Json(newArrayValue);
    }

    std::string Json::ParseValueStr(const std::string &str, size_t &offset) {
        std::stack<char> expectedDelimiters;
        std::vector<char> values;
        auto ss = str.substr(offset);
        if (ss.empty()) {
            offset = std::string::npos;
            return "";
        }
        bool isInsideStr = false;
        for (const auto ch:ss) {
            values.push_back(ch);
            if (!expectedDelimiters.empty() && ch == expectedDelimiters.top()) {
                isInsideStr = false;
                expectedDelimiters.pop();
                continue;
            }
            if (!isInsideStr) {
                if (ch == '\"') {
                    isInsideStr = true;
                    expectedDelimiters.push('\"');
                } else if (ch == '[') {
                    expectedDelimiters.push(']');
                } else if (ch == ',' && expectedDelimiters.empty()) {
                    break;
                }
            }
        }

        if (expectedDelimiters.empty()) {
            offset += values.size();
            if (values.back() == ',') {
                values.pop_back();
            }
            return std::string(values.begin(), values.end());
        }

        return "";
    }


    Json Json::ParseFloat(const std::string &str) {
        enum ParseState {
            PARSE_MINUS = 0,
            PARSE_NUMBER = 1,
            PARSE_ZERO = 2,
            PARSE_DIGIT = 3,
            PARSE_FRAC_DIGIT = 4,
            PARSE_FRAC_DIGITS = 5,
            PARSE_EXP = 6,
            PARSE_EXP_DIGIT = 7,
            PARSE_EXP_DIGITS = 8,
        };

        bool negative = false;
        bool negativeExponent = false;
        double value = 0.0;
        double fraction = 0.0;
        double exponent = 0.0;

        size_t fractionDigits = 0;

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
                        value = (double) (str[index] - '0');
                    } else {
                        return Json();
                    }
                    ++index;
                    break;
                case PARSE_ZERO:
                    parseState = ParseState::PARSE_DIGIT;
                case PARSE_DIGIT:
                    if (str[index] >= '0' && str[index] <= '9') {
                        value *= 10.0;
                        value += (double) (str[index] - '0');
                    } else if (str[index] == '.') {
                        parseState = PARSE_FRAC_DIGIT;
                    } else if (str[index] == 'e' || str[index] == 'E') {
                        parseState = PARSE_EXP;
                    } else {
                        return Json();
                    }
                    ++index;
                    break;

                case PARSE_FRAC_DIGIT:
                    if (str[index] >= '0' && str[index] <= '9') {
                        ++fractionDigits;
                        fraction += (double) (str[index] - '0') / pow(10.0, (double) fractionDigits);
                    } else {
                        return Json();
                    }
                    parseState = PARSE_FRAC_DIGITS;
                    ++index;
                    break;
                case PARSE_FRAC_DIGITS:
                    if (str[index] >= '0' && str[index] <= '9') {
                        ++fractionDigits;
                        fraction += (double) (str[index] - '0') / pow(10.0, (double) fractionDigits);
                    } else if (str[index] == 'e' || str[index] == 'E') {
                        parseState = PARSE_EXP;
                    } else {
                        return Json();
                    }
                    ++index;
                    break;

                case PARSE_EXP:
                    if (str[index] == '-') {
                        negativeExponent = true;
                        ++index;
                    } else if (str[index] == '+') {
                        ++index;
                    }
                    parseState = PARSE_EXP_DIGIT;
                    break;
                case PARSE_EXP_DIGIT:
                    parseState = PARSE_EXP_DIGITS;
                    break;
                case PARSE_EXP_DIGITS:
                    if (str[index] >= '0' && str[index] <= '9') {
                        exponent *= 10.0;
                        exponent += (double) (str[index] - '0');
                    } else {
                        return Json();
                    }
                    ++index;
                    break;
            }
        }

        if (parseState < ParseState::PARSE_NUMBER || parseState == PARSE_FRAC_DIGIT || parseState == PARSE_EXP || parseState == PARSE_EXP_DIGIT) {
            return Json();
        } else {
            return Json(
                    (value + fraction)
                    * pow(10.0, exponent * (negativeExponent ? -1.0 : 1.0))
                    * (negative ? -1.0 : 1.0));
        }
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
            }
        }

        if (parseState < ParseState::PARSE_NUMBER) {
            return Json();
        } else {
            return Json(value * (negative ? -1 : 1));
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
                return "Invalid";
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

    Type Json::GetType() const {
        return this->impl->type;
    }

    size_t Json::GetSize() const {
        if (this->impl->type == Type::Array) {
            return this->impl->arrayValue->size();
        }
        return 0;
    }

    std::shared_ptr<Json> Json::operator[](size_t index) const {
        if (this->impl->type == Type::Array) {
            return (*this->impl->arrayValue)[index];
        }
        return std::shared_ptr<Json>();
    }

}
