//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_TIMEKEEPER_IMPL_H
#define RAFT_TIMEKEEPER_IMPL_H

#include "ITimeKeeper.h"

namespace Timer {
    class TimeKeeperImpl : public ITimeKeeper {
    public:
        ~TimeKeeperImpl() noexcept;

        TimeKeeperImpl(const TimeKeeperImpl &) = delete;

        TimeKeeperImpl(TimeKeeperImpl &&) noexcept;

        TimeKeeperImpl &operator=(const TimeKeeperImpl &) = delete;

        TimeKeeperImpl &operator=(TimeKeeperImpl &&) noexcept;

    public:
        double GetCurrentTime();

    public:
        TimeKeeperImpl();

    private:
    };
}
#endif //RAFT_TIMEKEEPER_IMPL_H
