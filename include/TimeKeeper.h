//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_TIMEKEEPER_H
#define RAFT_TIMEKEEPER_H

#include <memory>
#include "TimeKeeperImpl.h"
#include "ITimeKeeper.h"

namespace Raft {
    class TimeKeeper : public ITimeKeeper {
    public:
        ~TimeKeeper() noexcept;

        TimeKeeper(const TimeKeeper &) = delete;

        TimeKeeper(TimeKeeper &&) noexcept;

        TimeKeeper &operator=(const TimeKeeper &) = delete;

        TimeKeeper &operator=(TimeKeeper &&) noexcept;

    public:
        virtual double GetCurrentTime() override;

    public:
        TimeKeeper();

    private:
        std::shared_ptr<TimeKeeperImpl> impl;
    };
}
#endif //RAFT_TIMEKEEPER_H
