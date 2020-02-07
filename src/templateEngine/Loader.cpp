//
// Created by XingfengYang on 2020/2/7.
//
#include <fstream>
#include <iostream>
#include "../../include/templateEngine/Loader.h"

namespace Raft {
    Result FileLoader::Load(const std::string &fileName) {
        std::ifstream input;
        input.open(fileName);
        if (!input.is_open()) {
            return {false, nullptr, "Could not open file " + fileName};
        }
        std::string content((std::istreambuf_iterator<char>(input)),
                            (std::istreambuf_iterator<char>()));
        return {true, content, ""};
    }

    void MemoryLoader::Add(const std::string &name, const std::string &body) {
        files.emplace_back(name, body);
    }

    Result MemoryLoader::Load(const std::string &name) {
        for (auto &pair:files) {
            if (pair.first == name) {
                return {true, pair.second, ""};
            }
        }
        return {false, nullptr, name + "not found"};
    }

    Loader::~Loader() {};
}
