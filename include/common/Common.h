//
// Created by XingfengYang on 2020/2/5.
//

#ifndef RAFT_COMMON_H
#define RAFT_COMMON_H

#include <string>
#include <sstream>
#include <set>


namespace Common {
    unsigned int GetHashCode(const std::string &str);

    void Split(const std::string &str, std::vector<std::string> &container, char delim);

    std::string &Trim(std::string &str);


    struct EncodingOptions {
        bool escapeNonAscii = false;
    };

    std::string Escape(const std::string &str, const EncodingOptions &options);

    std::string UnEscape(const std::string &str);

    void WriteMem(char *mem, uint32_t offset, char value);

    char ReadMem(char *mem, uint32_t offset);

    uint32_t ReadMemU32(char *mem, uint32_t offset);
}

#endif //RAFT_COMMON_H
