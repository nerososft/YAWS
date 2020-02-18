//
// Created by XingfengYang on 2020/2/10.
//
#include <vector>
#include <map>
#include "../../include/common/Common.h"

namespace Common {
    unsigned int GetHashCode(const std::string &str) {
        std::hash<std::string> str_hash;
        return str_hash(str);
    }

    void Split(const std::string &str, std::vector<std::string> &container, char delim) {
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delim)) {
            container.push_back(token);
        }
    }

    std::string &Trim(std::string &str) {
        if (str.empty()) {
            return str;
        }
        str.erase(0, str.find_first_not_of(' '));
        str.erase(str.find_last_not_of(' ') + 1);
        return str;
    }

    const std::set<char> CHARACTERS_TO_ESCAPE_IN_QUOTED_STRING{
            '"', '\\', '\b', '\f', '\n', '\r', '\t',
    };

    typedef uint32_t UnicodeCodePoint;
    const std::map<UnicodeCodePoint, UnicodeCodePoint> CODE_POINT_TO_ESCAPE_IN_QUOTED_STRING{
            {0x22, 0x22},//'"'
            {0x5C, 0x5C},//'\\'
            {0x2F, 0x2F},//'\'
            {0x62, 0x08},//'\b'
            {0x66, 0x0C},//'\f'
            {0x6E, 0x0A},//'\n'
            {0x72, 0x0D},//'\r'
            {0x74, 0x09},//'\t'
    };

    // TODO : should replace Unicode escape
    std::string Escape(const std::string &str, const EncodingOptions &options) {
        std::string output;
        for (size_t i = 0; i < str.length(); ++i) {
            if (CHARACTERS_TO_ESCAPE_IN_QUOTED_STRING.find(str[i]) != CHARACTERS_TO_ESCAPE_IN_QUOTED_STRING.end()) {
                output += '\\';
            }
            output += str[i];
        }
        return output;
    }

    // TODO : should replace Unicode escape
    std::string UnEscape(const std::string &str) {
        std::string output;
        bool escape = false;
        for (size_t i = 0; i < str.length(); ++i) {
            if (!escape && (str[i] == '\\')) {
                escape = true;
            } else {
                output += str[i];
                escape = false;
            }
        }
        return output;
    }

    void WriteMem(char *mem, uint32_t offset, char value) {
        *(mem + offset) = value;
    }

    char ReadMem(char *mem, uint32_t offset) {
        return *(mem + offset);
    }

    uint32_t ReadMemU32(char *mem, uint32_t offset) {
        return (
                ((uint32_t) ReadMem(mem, offset)) << 24 |
                ((uint32_t) ReadMem(mem, offset + 1)) << 16 |
                ((uint32_t) ReadMem(mem, offset + 2)) << 8 |
                ((uint32_t) ReadMem(mem, offset + 3))
        );
    }
}

