//
// Created by XingfengYang on 2020/1/1.
//

#include "../../include/timer/TimeKeeperImpl.h"
#include <ctime>

namespace Raft {
    TimeKeeperImpl::~TimeKeeperImpl() noexcept = default;

    TimeKeeperImpl::TimeKeeperImpl(Raft::TimeKeeperImpl &&) noexcept = default;

    TimeKeeperImpl &TimeKeeperImpl::operator=(TimeKeeperImpl &&) noexcept = default;

    TimeKeeperImpl::TimeKeeperImpl() {
    }

    double TimeKeeperImpl::GetCurrentTime() {
        return time(NULL);
    }
}
