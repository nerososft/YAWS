//
// Created by XingfengYang on 2020/2/7.
//

#ifndef RAFT_FILELOADER_H
#define RAFT_FILELOADER_H

#include <string>
#include <vector>

namespace Raft {
    struct Result {
        bool vaild;
        std::string data;
        std::string error;
    };

    class Loader {
    public:
        virtual ~Loader();

        virtual Result Load(const std::string &name) = 0;
    };

    class FileLoader : public Loader {
    public:
        Result Load(const std::string &fileName);
    };

    class MemoryLoader : public Loader {
    private:
        std::vector<std::pair<std::string, std::string>> files;
    public:
        void Add(const std::string &name, const std::string &body);

        Result Load(const std::string &name);
    };

}

#endif //RAFT_FILELOADER_H
