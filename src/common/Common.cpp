//
// Created by XingfengYang on 2020/2/10.
//
#include <vector>
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

