//
// Created by XingfengYang on 2020/2/5.
//

#ifndef RAFT_COMMON_H
#define RAFT_COMMON_H

#include <string>
#include <sstream>

namespace Raft {
    namespace Common {
        static unsigned int GetHashCode(const std::string &str) {
            unsigned int hash = 0;
            for (size_t i = 0; i < str.size(); i++) {
                hash = hash * 31 + static_cast<int>(str[i]);
            }
            return hash;
        }

        template<class Container>
        static void split(const std::string &str, Container &cont, char delim = ' ') {
            std::stringstream ss(str);
            std::string token;
            while (std::getline(ss, token, delim)) {
                cont.push_back(token);
            }
        }

        static std::string &trim(std::string &str) {
            if (str.empty()) {
                return str;
            }
            str.erase(0, str.find_first_not_of(" "));
            str.erase(str.find_last_not_of(" ") + 1);
            return str;
        }
    }
}

#endif //RAFT_COMMON_H
