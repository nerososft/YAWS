//
// Created by XingfengYang on 2020/1/2.
//
#include "../include/TimeKeeper.h"

namespace Raft {
    TimeKeeper::~TimeKeeper() noexcept = default;

    TimeKeeper::TimeKeeper(Raft::TimeKeeper &&) noexcept = default;

    TimeKeeper &TimeKeeper::operator=(TimeKeeper &&) noexcept = default;

    TimeKeeper::TimeKeeper() : impl(std::make_shared<TimeKeeperImpl>()) {
    }

    double TimeKeeper::GetCurrentTime() {
        return impl->GetCurrentTime();
    }
}
