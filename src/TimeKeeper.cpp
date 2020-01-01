//
// Created by XingfengYang on 2020/1/2.
//
#include <time.h>

namespace Raft {
    TimeKeeper::~TimeKeeper() noexcept = default;

    TimeKeeper::TimeKeeper(Raft::TimeKeeper &&) noexcept = default;

    TimeKeeper &TimeKeeper::operator=(TimeKeeper &&) noexcept = default;


    Message::Message() {

    }

    double TimeKeeper::GetCurrentTime() {
        return time(NULL);
    }
}
